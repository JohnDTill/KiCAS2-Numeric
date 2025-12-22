#include "ki_cas_native_rational.h"

#include "ki_cas_integer_math.h"
#include <cassert>
#include <numeric>

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
    // Greedy approach with only multiplication
    // a/b == c/d  ⇒  a*d == c*b
    size_t a_num_times_b_den;
    size_t b_num_times_a_den;
    const bool a_num_times_b_den_overflow = ckd_mul(&a_num_times_b_den, a.num, b.den);
    const bool b_num_times_a_den_overflow = ckd_mul(&b_num_times_a_den, b.num, a.den);
    if(!a_num_times_b_den_overflow || !b_num_times_a_den_overflow)
        return (a_num_times_b_den == b_num_times_a_den)
               && (a_num_times_b_den_overflow == b_num_times_a_den_overflow);

    // Greedy approach with floating point comparison
    const double difference = static_cast<double>(a.num) * static_cast<double>(b.den)
                              - static_cast<double>(b.num) * static_cast<double>(a.den);
    constexpr double tol = 1e-12;
    if(difference > tol || difference < -tol) return false;

    // Robust approach
    // TODO: I don't think you can generally answer this without gcd

    return false;
}

bool operator!=(NativeRational a, NativeRational b) noexcept {
    return !(a == b);
}

bool operator>(NativeRational a, NativeRational b) noexcept {
    // Greedy approach with only multiplication
    // a/b > c/d  ⇒  a*d > c*b
    size_t a_num_times_b_den;
    size_t b_num_times_a_den;
    const bool a_num_times_b_den_overflow = ckd_mul(&a_num_times_b_den, a.num, b.den);
    const bool b_num_times_a_den_overflow = ckd_mul(&b_num_times_a_den, b.num, a.den);
    if(!a_num_times_b_den_overflow || !b_num_times_a_den_overflow)
        return (a_num_times_b_den > b_num_times_a_den)
               || a_num_times_b_den_overflow;

    // Greedy approach with floating point comparison
    const double difference = static_cast<double>(a.num) * static_cast<double>(b.den)
                              - static_cast<double>(b.num) * static_cast<double>(a.den);
    constexpr double tol = 1e-12;
    if(difference > tol || difference < -tol) return difference > 0;

    // Robust approach
    // TODO: I don't think you can generally answer this without wider types

    return false;
}

bool operator>=(NativeRational a, NativeRational b) noexcept {
    // Greedy approach with only multiplication
    // a/b ≥ c/d  ⇒  a*d ≥ c*b
    size_t a_num_times_b_den;
    size_t b_num_times_a_den;
    const bool a_num_times_b_den_overflow = ckd_mul(&a_num_times_b_den, a.num, b.den);
    const bool b_num_times_a_den_overflow = ckd_mul(&b_num_times_a_den, b.num, a.den);
    if(!a_num_times_b_den_overflow || !b_num_times_a_den_overflow)
        return (a_num_times_b_den >= b_num_times_a_den)
               || a_num_times_b_den_overflow;

    // Greedy approach with floating point comparison
    const double difference = static_cast<double>(a.num) * static_cast<double>(b.den)
                              - static_cast<double>(b.num) * static_cast<double>(a.den);
    constexpr double tol = 1e-12;
    if(difference > tol || difference < -tol) return difference >= 0;

    // Robust approach with division
    // TODO: I don't think you can generally answer this without wider types

    return false;
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

    return false;  // TODO
}

}  // namespace KiCAS2
