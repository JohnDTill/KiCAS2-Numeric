#include "ki_cas_big_num_wrapper.h"

#include <cassert>
#include "ki_cas_native_integer.h"
#include "ki_cas_native_rational.h"

#ifndef NDEBUG
#include <memory.h>
#include <shared_mutex>
#include <unordered_set>
#endif

namespace KiCAS2 {

bool mpz_is_neg(const mpz_t op) noexcept {
    return op->_mp_size < 0;
}

void mpz_neg_inplace(mpz_t rop) noexcept {
    rop->_mp_size *= -1;
}

void fmpz_10_pow_ui(fmpz_t f, ulong rhs) {
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

    if(rhs <= std::numeric_limits<size_t>::digits10){
        fmpz_init_set_ui(f, powers_of_ten[rhs]);
    }else{
        fmpz_init(f);
        _fmpz_promote(f);
        mpz_ui_pow_ui(COEFF_TO_PTR(*f), 10, rhs);
    }
}

void fmpz_10_pow_fmpz(fmpz_t f, const fmpz_t rhs) {
    fmpz_pow_fmpz(f, FMPZ_TEN, rhs);
}

size_t mpz_sizeinbase10upperbound(const mpz_t val) noexcept {
    // return mpz_sizeinbase(val, 10);  // Avoid computation, make a quick upper bound
    return mpz_size(val) * std::numeric_limits<mp_limb_t>::digits10;
}

size_t fmpz_sizeinbase10upperbound(const fmpz_t val) noexcept {
    // return fmpz_sizeinbase(val, 10);  // Avoid computation, make a quick upper bound
    return fmpz_size(val) * std::numeric_limits<mp_limb_t>::digits10;
}

void fmpq_abs_inplace(fmpq_t val) noexcept {
    val->num = std::abs(val->num);
}

void mpz_init_set_strview(mpz_t f, std::string_view str) {
    #ifndef NDEBUG
    for(const char ch : str) assert(ch >= '0' && ch <= '9');
    #endif

    if(str.size() <= std::numeric_limits<size_t>::digits10){
        mpz_init_set_ui(f, knownfit_str2int(str));
    #if (!defined(__x86_64__) && !defined(__aarch64__) && !defined(_WIN64)) || !defined(_MSC_VER)
    }else if(str.size() <= std::numeric_limits<WideType>::digits10){
        const DoubleInt val = knownfit_str2wideint(str);
        mpz_init(f);
        fmpz ptr = PTR_TO_COEFF(f);
        fmpz_set_uiui(&ptr, val.high, val.low);
    #endif
    }else{
        std::string copy(str);
        mpz_init(f);
        const auto code = mpz_set_str(f, copy.data(), 10);
        assert(code == 0);
    }
}

static constexpr size_t base10Digits(size_t number) noexcept {
    size_t digits = 1;
    while((number /= 10) != 0) digits++;
    return digits;
}
static constexpr size_t COEFF_MAX_DIGITS = base10Digits(COEFF_MAX);

fmpz fmpz_from_strview(std::string_view str) {
    #ifndef NDEBUG
    for(const char ch : str) assert(ch >= '0' && ch <= '9');
    #endif

    if(str.size() < COEFF_MAX_DIGITS){
        return knownfit_str2int(str);
#if (!defined(__x86_64__) && !defined(__aarch64__) && !defined(_WIN64)) || !defined(_MSC_VER)
    }else if(str.size() <= std::numeric_limits<WideType>::digits10){
        const DoubleInt val = knownfit_str2wideint(str);
        fmpz f = 0;
        fmpz_set_uiui(&f, val.high, val.low);
        return f;
#endif
    }else{
        std::string copy(str);
        fmpz f = 0;
        const auto code = fmpz_set_str(&f, copy.data(), 10);
        assert(code == 0);
        return f;
    }
}

void fmpz_init_set_strview(fmpz_t f, std::string_view str) {
    *f = fmpz_from_strview(str);
}

void write_big_int(std::string& str, const mpz_t val) {
    // Resize str to ensure sufficient capacity for the largest possible number
    static constexpr size_t base = 10;
    static constexpr size_t PLUS_ONE_FOR_SIGN = 1;
    static constexpr size_t PLUS_ONE_FOR_NULL_TERMINATOR = 1;

    const size_t max_digits = mpz_sizeinbase10upperbound(val) + (PLUS_ONE_FOR_SIGN + PLUS_ONE_FOR_NULL_TERMINATOR);
    const size_t start_index = str.size();
    str.resize(str.size() + max_digits);

    // Append the number to the end of str
    mpz_get_str(str.data() + start_index, base, val);

    // Resize where prior allocation exceeded the actual need
    const size_t null_terminator_index = str.find('\0', start_index);
    str.resize(null_terminator_index);
}

static void write_big_int(std::string& str, const fmpz_t val, bool is_negative) {
    // Resize str to ensure sufficient capacity for the largest possible number
    static constexpr size_t base = 10;
    static constexpr size_t PLUS_ONE_FOR_SIGN = 1;
    static constexpr size_t PLUS_ONE_FOR_NULL_TERMINATOR = 1;

    const size_t max_digits = fmpz_sizeinbase(val, base) + (PLUS_ONE_FOR_SIGN + PLUS_ONE_FOR_NULL_TERMINATOR);
    const size_t start_index = str.size();
    str.resize(str.size() + max_digits);

    // Append the number to the end of str, overwriting any sign character
    const char backup = str[start_index-1];
    fmpz_get_str(str.data() + start_index - is_negative, base, val);
    str[start_index-1] = backup;

    // Resize where prior allocation exceeded the actual need
    const size_t null_terminator_index = str.find('\0', start_index);
    str.resize(null_terminator_index);
}

template<bool typeset_fraction> void write_big_rational(std::string& str, const fmpq_t val) {
    const fmpz* num = fmpq_numref(val);
    const fmpz* den = fmpq_denref(val);

    if(typeset_fraction){
        const bool is_negative = (fmpz_sgn(num) == -1);
        if(is_negative) str += '-';
        str += "⁜f⏴";
        write_big_int(str, num, is_negative);
        str += "⏵⏴";
        write_big_int(str, den, false);
        str += "⏵";
    }else{
        static constexpr size_t base = 10;
        static constexpr size_t PLUS_ONE_FOR_SIGN = 1;
        static constexpr size_t PLUS_ONE_FOR_NULL_TERMINATOR = 1;
        static constexpr size_t PLUS_ONE_FOR_DIVISION = 1;
        const size_t max_digits = fmpz_sizeinbase10upperbound(num) + fmpz_sizeinbase10upperbound(den)
                                  + (PLUS_ONE_FOR_SIGN + PLUS_ONE_FOR_NULL_TERMINATOR + PLUS_ONE_FOR_DIVISION);
        const size_t start_index = str.size();
        str.resize(str.size() + max_digits);

        _fmpq_get_str(str.data() + start_index, base, num, den);

        // Resize where prior allocation exceeded the actual need
        const size_t null_terminator_index = str.find('\0', start_index);
        str.resize(null_terminator_index);
    }
}
template void write_big_rational<false>(std::string&, const fmpq_t);
template void write_big_rational<true>(std::string&, const fmpq_t);

inline static fmpq conv(NativeRational val) {
    fmpq ans {0, 0};
    fmpz_init_set_ui(&ans.num, val.num);
    fmpz_init_set_ui(&ans.den, val.den);

    return ans;
}

fmpq fmpq_from_decimal_str(std::string_view str) {
    const auto decimal_index = str.find('.');
    if(decimal_index == std::string::npos) return {fmpz_from_strview(str), *FMPZ_ONE};
    else return fmpq_from_decimal_str(str, decimal_index);
}

fmpq fmpq_from_decimal_str(std::string_view str, size_t decimal_index) {
    NativeRational result;
    if(ckd_strdecimal2rat(&result, str, decimal_index) == false)
        return conv(result);

    fmpz lead = fmpz_from_strview(str.substr(0, decimal_index));

    fmpz num = fmpz_from_strview(str.substr(decimal_index+1));
    fmpz den = 0;
    fmpz_10_pow_ui(&den, str.size()-(decimal_index+1));
    fmpq_t tail {{num, den}};
    fmpq_canonicalise(tail);

    fmpq_add_fmpz(tail, tail, &lead);
    fmpz_clear(&lead);

    return *tail;
}

fmpq fmpq_from_scientific_str(std::string_view str) {
    NativeRational result;
    if(ckd_strscientific2rat(&result, str) == false)
        return conv(result);

    const size_t e_index = str.find('e');
    assert(e_index != std::string::npos);

    fmpq lhs = fmpq_from_decimal_str(str.substr(0, e_index));

    size_t exp_start = e_index + 1;

    const bool hasNegativePrefix = (str[exp_start] == '-');
    const bool hasPositivePrefix = (str[exp_start] == '+');
    exp_start += (hasNegativePrefix || hasPositivePrefix);
    const std::string_view exp_digits = str.substr(exp_start);

    fmpz tenPower = 0;

    size_t native_exp;
    if(ckd_str2int(&native_exp, exp_digits) == false){
        fmpz_10_pow_ui(&tenPower, native_exp);
    }else{
        fmpz exp = fmpz_from_strview(exp_digits);
        fmpz_10_pow_fmpz(&tenPower, &exp);
        fmpz_clear(&exp);
    }

    const auto op = hasNegativePrefix ? &fmpq_div_fmpz : &fmpq_mul_fmpz;
    (*op)(&lhs, &lhs, &tenPower);
    fmpz_clear(&tenPower);

    return lhs;
}

#ifndef NDEBUG
static std::allocator<size_t> allocator;
static std::unordered_set<const void*> allocated_memory;
static std::shared_mutex allocation_mutex;

static void* leakTrackingAlloc(size_t n) {
    allocation_mutex.lock_shared();
    size_t* allocated = allocator.allocate(n);
    if(allocated){
        const auto result = allocated_memory.insert(allocated);
        assert(result.second);
    }
    allocation_mutex.unlock_shared();

    return allocated;
}

static void leakTrackingFree(void* p, size_t old) noexcept {
    allocation_mutex.lock_shared();
    allocated_memory.erase(p);
    allocator.deallocate(reinterpret_cast<size_t*>(p), old);
    allocation_mutex.unlock_shared();
}

static void* leakTrackingRealloc(void* p, size_t old, size_t n) {
    void* reallocated = nullptr;
    if(n != 0){
        reallocated = leakTrackingAlloc(n);
        memcpy(reallocated, p, sizeof(size_t)*std::min(old, n));
    }
    leakTrackingFree(p, old);
    return reallocated;
}

struct Init {
    Init(){ mp_set_memory_functions(leakTrackingAlloc, leakTrackingRealloc, leakTrackingFree); }
};

static Init memoryTrackingInit;

bool isAllGmpMemoryFreed() noexcept {
    return allocated_memory.empty();
}
#endif

}
