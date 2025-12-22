#include "ki_cas_native_rational.h"

#include "ki_cas_integer_math.h"
#include <cassert>
#include <numeric>

#if !defined(__x86_64) && !defined(__aarch64__) && !defined(_WIN64)  // 32-bit
static_assert(sizeof(size_t)*8 == 32);
#include <cstdint>
#endif

#if defined(_MSC_VER) && defined(_M_ARM64)  // 64-bit ARM MSVC
// MSVC on arm64 does not have any way to multiply 128-bit numbers using intrinsics.
// We would rather create this function than include boost_multiprecision.
inline uint64_t _umul128(uint64_t a, uint64_t b, uint64_t* result_high) {
    uint64_t a_lo = static_cast<uint32_t>(a);
    uint64_t a_hi = a >> 32;
    uint64_t b_lo = static_cast<uint32_t>(b);
    uint64_t b_hi = b >> 32;

    uint64_t p0 = a_lo * b_lo;
    uint64_t p1 = a_lo * b_hi;
    uint64_t p2 = a_hi * b_lo;
    uint64_t p3 = a_hi * b_hi;

    uint64_t mid = (p0 >> 32) + static_cast<uint32_t>(p1) + static_cast<uint32_t>(p2);

    *result_high = p3 + (p1 >> 32) + (p2 >> 32) + (mid >> 32);
    return (p0 & 0xffffffffull) | (mid << 32);
}
#endif

namespace KiCAS2 {

KiCAS2::NativeRational::NativeRational(size_t numerator, size_t denominator) noexcept
    : num(numerator), den(denominator) {
    assert(denominator != 0);
}

NativeRational::operator long double() const noexcept {
    return static_cast<long double>(num) / static_cast<long double>(den);
}

NativeRational::operator double() const noexcept {
    return static_cast<double>(num) / static_cast<double>(den);
}

NativeRational::operator float() const noexcept {
    return static_cast<float>(num) / static_cast<float>(den);
}

NativeRational::operator size_t() const noexcept {
    return num / den;
}

bool operator==(NativeRational a, size_t b) noexcept {
    // n/d = b  ⇒  n = d*b
    // d*b overflow  ⇒  n ≠ d*b  (because n fits in size_t)

    size_t b_times_a_den;
    return (ckd_mul(&b_times_a_den, a.den, b) == false) && (a.num == b_times_a_den);
}

bool operator!=(NativeRational a, size_t b) noexcept {
    size_t b_times_a_den;
    return ckd_mul(&b_times_a_den, a.den, b) || (a.num != b_times_a_den);
}

bool operator==(size_t a, NativeRational b) noexcept {
    return b == a;
}

bool operator!=(size_t a, NativeRational b) noexcept {
    return b != a;
}

bool operator>(NativeRational a, size_t b) noexcept {
    // n/d > b  ⇒  n > d*b
    // d*b overflow  ⇒  n < d*b  (because n fits in size_t)

    size_t b_times_a_den;
    return (ckd_mul(&b_times_a_den, a.den, b) == false) && (a.num > b_times_a_den);
}

bool operator>=(NativeRational a, size_t b) noexcept {
    size_t b_times_a_den;
    return (ckd_mul(&b_times_a_den, a.den, b) == false) && (a.num >= b_times_a_den);
}

bool operator<(NativeRational a, size_t b) noexcept {
    size_t b_times_a_den;
    return ckd_mul(&b_times_a_den, a.den, b) || (a.num < b_times_a_den);
}

bool operator<=(NativeRational a, size_t b) noexcept {
    size_t b_times_a_den;
    return ckd_mul(&b_times_a_den, a.den, b) || (a.num <= b_times_a_den);
}

bool operator>(size_t a, NativeRational b) noexcept {
    return b < a;
}

bool operator>=(size_t a, NativeRational b) noexcept {
    return b <= a;
}

bool operator<(size_t a, NativeRational b) noexcept {
    return b > a;
}

bool operator<=(size_t a, NativeRational b) noexcept {
    return b >= a;
}

bool operator==(NativeRational a, NativeRational b) noexcept {
    // a/b == c/d  ⇒  a*d == c*b
    // Widen so that overflow is never an issue

#if defined( _WIN64 )  // 64-bit MSVC
    size_t a_num_times_b_den_high;
    const size_t a_num_times_b_den_low = _umul128(a.num, b.den, &a_num_times_b_den_high);
    size_t b_num_times_a_den_high;
    const size_t b_num_times_a_den_low = _umul128(b.num, a.den, &b_num_times_a_den_high);

    return a_num_times_b_den_high == b_num_times_a_den_high
           && a_num_times_b_den_low == b_num_times_a_den_low;
#elif defined(__x86_64) || defined(__aarch64__)  // 64-bit GCC or Clang
    return static_cast<__uint128_t>(a.num) * static_cast<__uint128_t>(b.den)
           == static_cast<__uint128_t>(b.num) * static_cast<__uint128_t>(a.den);
#else  // 32-bit
    static_assert(sizeof(size_t)*8 == 32);
    return static_cast<uint64_t>(a.num) * static_cast<uint64_t>(b.den)
           == static_cast<uint64_t>(b.num) * static_cast<uint64_t>(a.den);
#endif
}

bool operator!=(NativeRational a, NativeRational b) noexcept {
    return !(a == b);
}

bool operator>(NativeRational a, NativeRational b) noexcept {
    // a/b > c/d  ⇒  a*d > c*b
    // Widen so that overflow is never an issue

#if defined( _WIN64 )  // 64-bit MSVC
    size_t a_num_times_b_den_high;
    const size_t a_num_times_b_den_low = _umul128(a.num, b.den, &a_num_times_b_den_high);
    size_t b_num_times_a_den_high;
    const size_t b_num_times_a_den_low = _umul128(b.num, a.den, &b_num_times_a_den_high);

    return a_num_times_b_den_high > b_num_times_a_den_high
           || (a_num_times_b_den_high == b_num_times_a_den_high && a_num_times_b_den_low > b_num_times_a_den_low);
#elif defined(__x86_64) || defined(__aarch64__)  // 64-bit GCC or Clang
    return static_cast<__uint128_t>(a.num) * static_cast<__uint128_t>(b.den)
           > static_cast<__uint128_t>(b.num) * static_cast<__uint128_t>(a.den);
#else  // 32-bit
    static_assert(sizeof(size_t)*8 == 32);
    return static_cast<uint64_t>(a.num) * static_cast<uint64_t>(b.den)
           > static_cast<uint64_t>(b.num) * static_cast<uint64_t>(a.den);
#endif
}

bool operator>=(NativeRational a, NativeRational b) noexcept {
    // a/b ≥ c/d  ⇒  a*d ≥ c*b
    // Widen so that overflow is never an issue

#if defined( _WIN64 )  // 64-bit MSVC
    size_t a_num_times_b_den_high;
    const size_t a_num_times_b_den_low = _umul128(a.num, b.den, &a_num_times_b_den_high);
    size_t b_num_times_a_den_high;
    const size_t b_num_times_a_den_low = _umul128(b.num, a.den, &b_num_times_a_den_high);

    return a_num_times_b_den_high > b_num_times_a_den_high
           || (a_num_times_b_den_high == b_num_times_a_den_high && a_num_times_b_den_low >= b_num_times_a_den_low);
#elif defined(__x86_64) || defined(__aarch64__)  // 64-bit GCC or Clang
    return static_cast<__uint128_t>(a.num) * static_cast<__uint128_t>(b.den)
           >= static_cast<__uint128_t>(b.num) * static_cast<__uint128_t>(a.den);
#else  // 32-bit
    static_assert(sizeof(size_t)*8 == 32);
    return static_cast<uint64_t>(a.num) * static_cast<uint64_t>(b.den)
           >= static_cast<uint64_t>(b.num) * static_cast<uint64_t>(a.den);
#endif
}

bool operator<(NativeRational a, NativeRational b) noexcept {
    return b > a;
}

bool operator<=(NativeRational a, NativeRational b) noexcept {
    return b >= a;
}

void NativeRational::reduceInPlace() noexcept {
    assert(den != 0);
    const size_t gcd = std::gcd(num, den);
    assert(gcd != 0);
    if(gcd != 1){
        num /= gcd;
        den /= gcd;
    }
}

bool ckd_mul(NativeRational* result, NativeRational a, size_t b) noexcept {
    result->den = a.den;
    if(ckd_mul(&result->num, a.num, b) == false) return false;

    // Resist expanding if possible
    const size_t gcd_a_den_b = std::gcd(a.den, b);
    if(gcd_a_den_b != 1){
        b /= gcd_a_den_b;
        result->den /= gcd_a_den_b;
        if(ckd_mul(&result->num, a.num, b) == false) return false;
    }

    const size_t gcd_a = std::gcd(a.num, a.den);
    if(gcd_a != 1){
        a.num /= gcd_a;
        result->den /= gcd_a;
        return ckd_mul(&result->num, a.num, b);
    }

    return true;
}

bool ckd_mul(NativeRational* result, NativeRational a, NativeRational b) noexcept {
    if(ckd_mul(&result->num, a.num, b.num) == false && ckd_mul(&result->den, a.den, b.den) == false)
        return false;

    const size_t gcd_a_num_b_den = std::gcd(a.num, b.den);
    if(gcd_a_num_b_den != 1){
        a.num /= gcd_a_num_b_den;
        b.den /= gcd_a_num_b_den;

        if(ckd_mul(&result->num, a.num, b.num) == false && ckd_mul(&result->den, a.den, b.den) == false)
            return false;
    }

    const size_t gcd_b_num_a_den = std::gcd(b.num, a.den);
    if(gcd_b_num_a_den != 1){
        b.num /= gcd_b_num_a_den;
        a.den /= gcd_b_num_a_den;

        if(ckd_mul(&result->num, a.num, b.num) == false && ckd_mul(&result->den, a.den, b.den) == false)
            return false;
    }

    const size_t gcd_a = std::gcd(a.num, a.den);
    if(gcd_a != 1){
        a.num /= gcd_a;
        a.den /= gcd_a;

        if(ckd_mul(&result->num, a.num, b.num) == false && ckd_mul(&result->den, a.den, b.den) == false)
            return false;
    }

    const size_t gcd_b = std::gcd(b.num, b.den);
    if(gcd_b != 1){
        b.num /= gcd_b;
        b.den /= gcd_b;

        return ckd_mul(&result->num, a.num, b.num) || ckd_mul(&result->den, a.den, b.den);
    }

    return true;
}

bool ckd_add(NativeRational* result, NativeRational a, size_t b) noexcept {
    // n/d + b = (n + b*d) / d

    size_t b_times_a_den;
    if(ckd_mul(&b_times_a_den, b, a.den)){
        const size_t gcd_a = std::gcd(a.num, a.den);
        if(gcd_a == 1) return true;
        a.den /= gcd_a;
        if(ckd_mul(&b_times_a_den, b, a.den)) return true;
        a.num /= gcd_a;
    }

    result->den = a.den;
    return ckd_add(&result->num, a.num, b_times_a_den);
}

bool ckd_add(NativeRational* result, NativeRational a, NativeRational b) noexcept {
    // a/b + c/d = (a*d + b*c) / (b*d)
    size_t a_num_times_b_den;
    size_t b_num_times_a_den;

    if(ckd_mul(&result->den, a.den, b.den) == false
       && ckd_mul(&a_num_times_b_den, a.num, b.den) == false
       && ckd_mul(&b_num_times_a_den, b.num, a.den) == false
       && ckd_add(&result->num, a_num_times_b_den, b_num_times_a_den) == false)
        return false;

    a.reduceInPlace();
    b.reduceInPlace();

    // (b*d) = [ a*(d/gcd(b,d)) + c*(b/gcd(b,d)) ] / [b*(d/gcd(b,d))]
    const size_t gcd_a_den_b_den = std::gcd(a.den, b.den);
    if(gcd_a_den_b_den != 1){
        a.den /= gcd_a_den_b_den;
        b.den /= gcd_a_den_b_den;
    }

    return ckd_mul(&result->den, a.den, b.den*gcd_a_den_b_den)
       || ckd_mul(&a_num_times_b_den, a.num, b.den)
       || ckd_mul(&b_num_times_a_den, b.num, a.den)
       || ckd_add(&result->num, a_num_times_b_den, b_num_times_a_den);
}

NativeRational sub(NativeRational a, size_t b) noexcept {
    assert(a >= b);
    // a ≥ b  ⇒  a.den * b <= std::numeric_limits<size_t>::max(), so no overflow logic
    // n/d - b = (n - b*d) / d
    return NativeRational(knownfit_sub(a.num, knownfit_mul(a.den, b)), a.den);
}

bool ckd_sub(NativeRational* result, size_t a, NativeRational b) noexcept {
    assert(a >= b);

    // a - n/d = (a*d - n) / d

    size_t a_times_b_den;
    if(ckd_mul(&a_times_b_den, a, b.den)){
        const size_t gcd_b = std::gcd(b.num, b.den);
        if(gcd_b == 1) return true;
        b.den /= gcd_b;
        if(ckd_mul(&a_times_b_den, a, b.den)) return true;
        b.num /= gcd_b;
    }

    result->den = b.den;
    result->num = knownfit_sub(a_times_b_den, b.num);

    return false;
}

bool ckd_sub(NativeRational* result, NativeRational a, NativeRational b) noexcept {
    assert(a >= b);

    // a/b - c/d = (a*d - b*c) / (b*d)
    size_t a_num_times_b_den;
    size_t b_num_times_a_den;

    if(ckd_mul(&result->den, a.den, b.den) == false
        && ckd_mul(&a_num_times_b_den, a.num, b.den) == false
        && ckd_mul(&b_num_times_a_den, b.num, a.den) == false){
        result->num = knownfit_sub(a_num_times_b_den, b_num_times_a_den);
        return false;
    }

    a.reduceInPlace();
    b.reduceInPlace();

    // (b*d) = [ a*(d/gcd(b,d)) + c*(b/gcd(b,d)) ] / [b*(d/gcd(b,d))]
    const size_t gcd_a_den_b_den = std::gcd(a.den, b.den);
    if(gcd_a_den_b_den != 1){
        a.den /= gcd_a_den_b_den;
        b.den /= gcd_a_den_b_den;
    }

    if(ckd_mul(&result->den, a.den, b.den * gcd_a_den_b_den) == false
        && ckd_mul(&a_num_times_b_den, a.num, b.den) == false
        && ckd_mul(&b_num_times_a_den, b.num, a.den) == false){
        result->num = knownfit_sub(a_num_times_b_den, b_num_times_a_den);
        return false;
    }

    return true;
}

}  // namespace KiCAS2
