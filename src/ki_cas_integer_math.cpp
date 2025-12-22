#include "ki_cas_integer_math.h"

#include <cassert>
#include <cmath>

#if __cplusplus >= 202302L
#include <stdckdint.h>
#endif

namespace KiCAS2 {

bool ckd_add(size_t* result, size_t a, size_t b) noexcept {
#if __cplusplus >= 202302L
    return std::ckd_add(result, a, b);
#elif defined(__clang__)
    return __builtin_add_overflow(a, b, result);
#else
    *result = a + b;
    return *result < a;
#endif
}

bool ckd_mul(size_t* result, size_t a, size_t b) noexcept {
#if __cplusplus >= 202302L
    return std::ckd_mul(result, a, b);
#elif defined(__clang__)
    return __builtin_mul_overflow(a, b, result);
#else
    *result = a * b;
    return a != 0 && (*result) / a != b;
#endif
}

size_t knownfit_mul(size_t a, size_t b) noexcept {
    assert(a == 0 || (a * b) / a == b);
    return a * b;
}

size_t knownfit_sub(size_t a, size_t b) noexcept {
    assert(a >= b);
    return a - b;
}

bool ckd_sqrt(size_t* result, size_t arg) noexcept {
    // Adequate precision confirmed for 64-bit numbers, see tests
    *result = static_cast<size_t>(std::sqrt(static_cast<double>(arg)));
    return (*result) * (*result) != arg;
}

bool ckd_cbrt(size_t* result, size_t arg) noexcept {
    // Adequate precision confirmed for 64-bit numbers, see tests
    *result = static_cast<size_t>(std::cbrtf(static_cast<float>(arg))+0.5);
    return (*result) * (*result) * (*result) != arg;
}

bool ckd_nrt(size_t* result, size_t arg, size_t power) noexcept {
    assert(power >= 4);

    const auto fp_result = std::pow(arg, 1.0/static_cast<double>(power));
    *result = static_cast<size_t>(fp_result + 0.5);

    constexpr double tol = 1e-11;
    const auto error = fp_result - *result;
    if(error > tol || error < -tol) return true;

    // EVENTUALLY: it would be nice if we could rely on the tolerance only,
    //             but the search space to confirm no impure roots is enormous.
    return knownfit_pow(*result, power) != arg;
}

static size_t ckd_pow_helper(bool& overflowed, size_t base, size_t power) noexcept {
    if (power == 0) return 1;
    if (power == 1) return base;

    const size_t tmp = ckd_pow_helper(overflowed, base, power/2);
    if(overflowed) return 0;

    size_t result;
    if(power%2 == 0){
        overflowed = ckd_mul(&result, tmp, tmp);
    }else{
        overflowed = ckd_mul(&result, tmp, tmp);
        if(!overflowed) overflowed = ckd_mul(&result, base, result);
    }

    return result;
}

bool ckd_pow(size_t* result, size_t base, size_t power) noexcept {
    assert(base != 0 || power != 0);  // 0^0 is not generally defined
    bool overflowed = false;
    *result = ckd_pow_helper(overflowed, base, power);
    return overflowed;
}

size_t knownfit_pow(size_t base, size_t power) noexcept {
    assert(base != 0 || power != 0);  // 0^0 is not generally defined

    if(power == 0) return 1;
    if(power == 1) return base;

    const size_t tmp = knownfit_pow(base, power/2);
    if(power%2 == 0) return knownfit_mul(tmp, tmp);
    else return knownfit_mul(base, knownfit_mul(tmp, tmp));
}

}  // namespace KiCAS2
