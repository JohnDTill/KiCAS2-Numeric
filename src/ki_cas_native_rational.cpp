#include "ki_cas_native_rational.h"

#include "ki_cas_native_integer.h"
#include <cassert>
#include <limits>
#include <numeric>

#if !defined(__x86_64__) && !defined(__aarch64__) && !defined(_WIN64)  // 32-bit
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

#if defined( _WIN64 ) && defined(_MSC_VER)  // 64-bit MSVC
    size_t a_num_times_b_den_high;
    const size_t a_num_times_b_den_low = _umul128(a.num, b.den, &a_num_times_b_den_high);
    size_t b_num_times_a_den_high;
    const size_t b_num_times_a_den_low = _umul128(b.num, a.den, &b_num_times_a_den_high);

    return a_num_times_b_den_high == b_num_times_a_den_high
           && a_num_times_b_den_low == b_num_times_a_den_low;
#elif defined(__x86_64__) || defined(__aarch64__)  // 64-bit GCC or Clang
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

#if defined( _WIN64 ) && defined(_MSC_VER)  // 64-bit MSVC
    size_t a_num_times_b_den_high;
    const size_t a_num_times_b_den_low = _umul128(a.num, b.den, &a_num_times_b_den_high);
    size_t b_num_times_a_den_high;
    const size_t b_num_times_a_den_low = _umul128(b.num, a.den, &b_num_times_a_den_high);

    return a_num_times_b_den_high > b_num_times_a_den_high
           || (a_num_times_b_den_high == b_num_times_a_den_high && a_num_times_b_den_low > b_num_times_a_den_low);
#elif defined(__x86_64__) || defined(__aarch64__)  // 64-bit GCC or Clang
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

#if defined( _WIN64 ) && defined(_MSC_VER)  // 64-bit MSVC
    size_t a_num_times_b_den_high;
    const size_t a_num_times_b_den_low = _umul128(a.num, b.den, &a_num_times_b_den_high);
    size_t b_num_times_a_den_high;
    const size_t b_num_times_a_den_low = _umul128(b.num, a.den, &b_num_times_a_den_high);

    return a_num_times_b_den_high > b_num_times_a_den_high
           || (a_num_times_b_den_high == b_num_times_a_den_high && a_num_times_b_den_low >= b_num_times_a_den_low);
#elif defined(__x86_64__) || defined(__aarch64__)  // 64-bit GCC or Clang
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

NativeRational NativeRational::reciprocal() const noexcept {
    assert(num != 0);
    return NativeRational(den, num);
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

        if(ckd_mul(&result->num, a.num, b.num) == false && ckd_mul(&result->den, a.den, b.den) == false)
            return false;
    }

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

    return true;
}

bool ckd_div(NativeRational* result, NativeRational a, size_t b) noexcept {
    assert(b != 0);

    result->num = a.num;
    if(ckd_mul(&result->den, a.den, b) == false) return false;

    // Resist expanding if possible
    const size_t gcd_a_num_b = std::gcd(a.num, b);
    if(gcd_a_num_b != 1){
        b /= gcd_a_num_b;
        result->num /= gcd_a_num_b;
        if(ckd_mul(&result->den, a.den, b) == false) return false;
    }

    const size_t gcd_a = std::gcd(a.num, a.den);
    if(gcd_a != 1){
        a.den /= gcd_a;
        result->num /= gcd_a;
        return ckd_mul(&result->den, a.den, b);
    }

    return true;
}

bool ckd_div(NativeRational* result, NativeRational a, NativeRational b) noexcept {
    return ckd_mul(result, a, b.reciprocal());
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

    const size_t gcd_a_den_b_den = std::gcd(a.den, b.den);
    const size_t gcd_a = std::gcd(a.num, a.den);
    const size_t gcd_b = std::gcd(b.num, b.den);

    const size_t a_den_divisor = knownfit_mul(gcd_a, gcd_a_den_b_den);
    const size_t b_den_divior = knownfit_mul(gcd_b, gcd_a_den_b_den);

    if(a_den_divisor != 1) a.den /= a_den_divisor;
    if(b_den_divior != 1) b.den /= b_den_divior;
    if(gcd_a != 1) a.num /= gcd_a;
    if(gcd_b != 1) b.num /= gcd_b;

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

    const size_t gcd_a_den_b_den = std::gcd(a.den, b.den);
    const size_t gcd_a = std::gcd(a.num, a.den);
    const size_t gcd_b = std::gcd(b.num, b.den);

    const size_t a_den_divisor = knownfit_mul(gcd_a, gcd_a_den_b_den);
    const size_t b_den_divior = knownfit_mul(gcd_b, gcd_a_den_b_den);

    if(a_den_divisor != 1) a.den /= a_den_divisor;
    if(b_den_divior != 1) b.den /= b_den_divior;
    if(gcd_a != 1) a.num /= gcd_a;
    if(gcd_b != 1) b.num /= gcd_b;

    if(ckd_mul(&result->den, a.den, b.den * gcd_a_den_b_den) == false
        && ckd_mul(&a_num_times_b_den, a.num, b.den) == false
        && ckd_mul(&b_num_times_a_den, b.num, a.den) == false){
        result->num = knownfit_sub(a_num_times_b_den, b_num_times_a_den);
        return false;
    }

    return true;
}

template<bool typeset_fraction>
void write_native_rational(std::string& str, NativeRational val) {
    if(typeset_fraction) str += "⁜f⏴";
    write_native_int(str, val.num);
    if(typeset_fraction) str += "⏵⏴";
    else str += '/';
    write_native_int(str, val.den);
    if(typeset_fraction) str += "⏵";
}
template void write_native_rational<false>(std::string&, NativeRational);
template void write_native_rational<true>(std::string&, NativeRational);

constexpr size_t powers_of_ten[] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000uLL,
#if defined(__x86_64__) || defined(__aarch64__) || defined( _WIN64 )  // 64-bit
    10000000000,
    100000000000,
    1000000000000,
    10000000000000,
    100000000000000,
    1000000000000000,
    10000000000000000,
    100000000000000000,
    1000000000000000000,
    10000000000000000000uLL,
#endif
};

// Check this rather than specify it to make sure the macro worked
static_assert(sizeof(powers_of_ten)/sizeof(size_t) == std::numeric_limits<size_t>::digits10+1);

constexpr size_t powers_of_five[] = {
    1,
    5,
    25,
    125,
    625,
    3125,
    15625,
    78125,
    390625,
    1953125,
    9765625uLL,
#if defined(__x86_64__) || defined(__aarch64__) || defined( _WIN64 )  // 64-bit
    48828125,
    244140625,
    1220703125,
    6103515625,
    30517578125,
    152587890625,
    762939453125,
    3814697265625,
    19073486328125,
    95367431640625uLL,
#endif
};
static_assert(sizeof(powers_of_five)/sizeof(size_t) == std::numeric_limits<size_t>::digits10+2);

bool ckd_strdecimaltail2rat(NativeRational* result, std::string_view str) noexcept {
    assert(str.at(0) == '.');
    #ifndef NDEBUG
    for(size_t i = 1; i < str.size(); i++) assert(str[i] >= '0' && str[i] <= '9');
    #endif

    // Omit trailing zeros
    size_t back_index = str.size()-1;
    while(str[back_index] == '0') back_index--;
    if(back_index == 0){
        // Only trailing zeros
        result->num = 0;
        result->den = 1;
        return false;
    }
    const std::string_view digits = str.substr(1, back_index);

    // The denominator has factors (2 * 5)^len(digits)
    // Because we have eliminated all factors of 10 from the numerator,
    // the common factors are, mutually exclusively:
    //   Instances of 2
    //   Instances of 5

    if(digits.back() == '5'){
        // Give up if the numerator does not fit.
        // There are pathological cases where the numerator does not fit but the result would,
        // but we simply report overflow.
        size_t num;
        if(ckd_str2int(&num, digits)) return true;
        num /= 5;

        uint8_t den_num_5_factors = digits.size()-1;
        const uint8_t den_num_2_factors = digits.size();

        size_t num_div_5 = num / 5;
        size_t num_mod_5 = num % 5;
        while(num_mod_5 == 0){
            num = num_div_5;
            assert(den_num_5_factors > 0);
            den_num_5_factors--;
            if(den_num_5_factors == 0) break;

            num_div_5 = num / 5;
            num_mod_5 = num % 5;
        }

        result->num = num;
        const size_t den_2_factors = (1uLL << den_num_2_factors);
        const size_t den_5_factors = powers_of_five[den_num_5_factors];

        if(sizeof(size_t) == 4){
            // If the numerator fits, the denominator necessarily fits on 32-bit systems
            //                (2^32 - 1) = 4294967295
            // The largest denominator is 10000000000 = 10 ^ ⌈log₁₀(2^32 - 1)⌉
            // and (10 ^ ⌈log₁₀(2^32 - 1)⌉ / 5) < 2^32 - 1

            result->den = knownfit_mul(den_2_factors, den_5_factors);
            return false;
        }else{
            // One division by 5 is not sufficient to guarantee the denominator fits on 64-bit systems
            //                (2^64 - 1) = 18446744073709551615
            // The largest denominator is 100000000000000000000
            // Clearly                     20000000000000000000
            //                           > 18446744073709551615

            return ckd_mul(&result->den, den_2_factors, den_5_factors);
        }
    }else{
        // Give up if the numerator does not fit.
        // There are pathological cases where the numerator does not fit but the result would,
        // but we simply report overflow.
        size_t num;
        if(ckd_str2int(&num, digits)) return true;

        const uint8_t den_num_5_factors = digits.size();
        uint8_t den_num_2_factors = digits.size();

        while(num % 2 == 0 && den_num_2_factors){
            num /= 2;
            den_num_2_factors--;
        }

        result->num = num;
        const size_t den_2_factors = (1uLL << den_num_2_factors);
        const size_t den_5_factors = powers_of_five[den_num_5_factors];

        return ckd_mul(&result->den, den_2_factors, den_5_factors);
    }
}

bool ckd_strdecimal2rat(NativeRational* result, std::string_view str) noexcept {
    return ckd_strdecimal2rat(result, str, str.find('.'));
}

bool ckd_strdecimal2rat(NativeRational* result, std::string_view str, size_t decimal_index) noexcept {
    assert(str.at(decimal_index) == '.');
    assert(str.length() >= 2);
    #ifndef NDEBUG
    for(size_t i = 0; i < str.size(); i++) assert((str[i] >= '0' && str[i] <= '9') || (i == decimal_index));
    #endif

    if(decimal_index == 0) return ckd_strdecimaltail2rat(result, str);

    size_t leading;
    return ckd_strdecimaltail2rat(result, str.substr(decimal_index))
           || ckd_str2int(&leading, str.substr(0, decimal_index))
           || ckd_mul(&leading, leading, result->den)
           || ckd_add(&result->num, leading, result->num);
}

static inline void copy(char* dest, std::string_view str) noexcept {
    for(const char ch : str) *dest++ = ch;
}

bool ckd_strdecimal2rat_times10toPos(NativeRational* result, std::string_view str, size_t power) noexcept {
    const size_t decimal_index = str.find('.');
    if(decimal_index == std::string::npos){
        result->den = 1;
        return power >= (sizeof(powers_of_ten) / sizeof(size_t))
               || ckd_str2int(&result->num, str)
               || ckd_mul(&result->num, result->num, powers_of_ten[power]);
    }

    size_t leading_start = 0;
    while(str[leading_start] == '0') leading_start++;
    leading_start = std::min(leading_start, decimal_index-1);
    size_t trailing_end = str.size()-1;
    while(str[trailing_end] == '0') trailing_end--;

    const size_t num_leading_digits = decimal_index - leading_start;
    const std::string_view leading_digits = str.substr(leading_start, num_leading_digits);
    const size_t num_trailing_digits = trailing_end-decimal_index;
    const std::string_view trailing_digits = str.substr(decimal_index+1, num_trailing_digits);

    // Effectively shift the decimal to the right by power places
    char buffer[std::numeric_limits<size_t>::digits10+1];
    if(power >= num_trailing_digits){
        const size_t num_digits = num_leading_digits + num_trailing_digits;
        if(num_digits > std::numeric_limits<size_t>::digits10) return true;
        copy(buffer, leading_digits);
        copy(buffer+num_leading_digits, trailing_digits);

        power -= num_trailing_digits;
        result->den = 1;
        return power >= (sizeof(powers_of_ten) / sizeof(size_t))
               || ckd_str2int(&result->num, std::string_view(buffer, num_digits))
               || ckd_mul(&result->num, result->num, powers_of_ten[power]);
    }else{
        const size_t num_leading_digits_after_shift = num_leading_digits + power;
        const size_t num_trailing_digits_after_shift = num_trailing_digits - power;
        if(num_leading_digits_after_shift > std::numeric_limits<size_t>::digits10) return true;
        copy(buffer, leading_digits);
        copy(buffer+num_leading_digits, trailing_digits.substr(0, power));

        size_t leading;
        if(ckd_str2int(&leading, std::string_view(buffer, num_leading_digits_after_shift))) return true;

        if(num_trailing_digits_after_shift > std::numeric_limits<size_t>::digits10) return true;
        buffer[0] = '.';
        copy(buffer+1, trailing_digits.substr(power));

        return ckd_strdecimaltail2rat(result, std::string_view(buffer, 1+num_trailing_digits_after_shift))
               || ckd_mul(&leading, leading, result->den)
               || ckd_add(&result->num, leading, result->num);
    }
}

static bool ckd_strinteger2rat_times10toNeg(NativeRational* result, std::string_view str, size_t power) noexcept {
    size_t leading_start = 0;
    while(leading_start < str.size() && str[leading_start] == '0') leading_start++;

    if(leading_start == str.size()){
        result->num = 0;
        result->den = 1;
        return false;
    }

    const size_t num_digits = str.size() - leading_start;
    if(num_digits==0){
        result->num = 0;
        result->den = 1;
        return false;
    }
    const std::string_view digits = str.substr(leading_start, num_digits);

    // Effectively shift the decimal to the left by power places
    char buffer[std::numeric_limits<size_t>::digits10+1];
    if(num_digits <= power){
        if(num_digits > std::numeric_limits<size_t>::digits10) return true;
        buffer[0] = '.';
        copy(buffer+1, digits);

        power -= num_digits;
        result->den = 1;

        return power >= (sizeof(powers_of_ten) / sizeof(size_t))
               || ckd_strdecimaltail2rat(result, std::string_view(buffer, num_digits+1))
               || ckd_mul(&result->den, result->den, powers_of_ten[power]);
    }else{
        const size_t num_leading_digits_after_shift = num_digits - power;
        const size_t num_trailing_digits_after_shift = power;
        if(num_leading_digits_after_shift > std::numeric_limits<size_t>::digits10) return true;
        copy(buffer, digits.substr(0, num_leading_digits_after_shift));

        size_t leading;
        if(ckd_str2int(&leading, std::string_view(buffer, num_leading_digits_after_shift))) return true;

        if(num_trailing_digits_after_shift > std::numeric_limits<size_t>::digits10) return true;
        buffer[0] = '.';
        copy(buffer+1, digits.substr(power));

        return ckd_strdecimaltail2rat(result, std::string_view(buffer, 1+num_trailing_digits_after_shift))
               || ckd_mul(&leading, leading, result->den)
               || ckd_add(&result->num, leading, result->num);
    }
}

bool ckd_strdecimal2rat_times10toNeg(NativeRational* result, std::string_view str, size_t power) noexcept {
    const size_t decimal_index = str.find('.');
    if(decimal_index == std::string::npos) return ckd_strinteger2rat_times10toNeg(result, str, power);

    size_t leading_start = 0;
    while(str[leading_start] == '0') leading_start++;
    size_t trailing_end = str.size()-1;
    while(str[trailing_end] == '0') trailing_end--;

    const size_t num_leading_digits = decimal_index - leading_start;
    const std::string_view leading_digits = str.substr(leading_start, num_leading_digits);
    const size_t num_trailing_digits = trailing_end-decimal_index;
    const std::string_view trailing_digits = str.substr(decimal_index+1, num_trailing_digits);
    const size_t num_digits = num_leading_digits + num_trailing_digits;
    if(num_digits==0){
        result->num = 0;
        result->den = 1;
        return false;
    }

    // Effectively shift the decimal to the left by power places
    char buffer[std::numeric_limits<size_t>::digits10+1];
    if(num_leading_digits <= power){
        if(num_digits > std::numeric_limits<size_t>::digits10) return true;
        buffer[0] = '.';
        copy(buffer+1, leading_digits);
        copy(buffer+1+num_leading_digits, trailing_digits);

        power -= num_leading_digits;
        result->den = 1;

        return power >= (sizeof(powers_of_ten) / sizeof(size_t))
               || ckd_strdecimaltail2rat(result, std::string_view(buffer, num_digits+1))
               || ckd_mul(&result->den, result->den, powers_of_ten[power]);
    }else{
        const size_t num_leading_digits_after_shift = num_leading_digits - power;
        const size_t num_trailing_digits_after_shift = num_trailing_digits + power;
        if(num_leading_digits_after_shift > std::numeric_limits<size_t>::digits10) return true;
        copy(buffer, leading_digits.substr(0, num_leading_digits_after_shift));

        size_t leading;
        if(ckd_str2int(&leading, std::string_view(buffer, num_leading_digits_after_shift))) return true;

        if(num_trailing_digits_after_shift > std::numeric_limits<size_t>::digits10) return true;
        buffer[0] = '.';
        copy(buffer+1, leading_digits.substr(num_leading_digits_after_shift));
        copy(buffer+1+power, trailing_digits);

        return ckd_strdecimaltail2rat(result, std::string_view(buffer, 1+num_trailing_digits_after_shift))
               || ckd_mul(&leading, leading, result->den)
               || ckd_add(&result->num, leading, result->num);
    }
}

bool ckd_strscientific2rat(NativeRational* result, std::string_view str) noexcept {
    const size_t e_index = str.find('e');
    assert(e_index != std::string::npos);
    const size_t e_start = e_index+1;
    size_t exp;
    if(str[e_start] == '-'){
        return ckd_str2int(&exp, str.substr(e_start+1))
               || ckd_strdecimal2rat_times10toNeg(result, str.substr(0, e_index), exp);
    }else{
        return ckd_str2int(&exp, str.substr(e_start + (str[e_start]=='+')))
               || ckd_strdecimal2rat_times10toPos(result, str.substr(0, e_index), exp);
    }
}

}  // namespace KiCAS2
