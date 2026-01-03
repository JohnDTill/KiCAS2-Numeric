#include "ki_cas_big_num_wrapper.h"

#include <cassert>
#include "ki_cas_native_integer.h"

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

void mpz_init_set_ui_mul_ui(mpz_t ans, mp_limb_t lhs, mp_limb_t rhs) {
    mpz_init_set_ui(ans, lhs);
    mpz_mul_ui(ans, ans, rhs);
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

void mpz_init_set_mutable_str(mpz_t f, std::string& str, size_t pos, size_t len) {
    #ifndef NDEBUG
    assert(str.size() >= len);
    for(const char ch : std::string_view(str.data()+pos, len)) assert(ch >= '0' && ch <= '9');
    assert(!(str[pos+len] >= '0' && str[pos+len] <= '9'));
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
        const size_t end_index = pos+len;

        // Backup the character at the end, and replace with '\0' so that GMP knows to stop parsing the int.
        const char backup = str[end_index];
        str[end_index] = '\0';

        // Parse the null-terminated str
        mpz_init(f);
        const auto code = mpz_set_str(f, str.data()+pos, 10);
        assert(code == 0);

        // Restore the original str
        str[end_index] = backup;
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

fmpz fmpz_from_mutable_str(std::string& str, size_t pos, size_t len) {
    #ifndef NDEBUG
    assert(str.size() >= len);
    for(const char ch : std::string_view(str.data()+pos, len)) assert(ch >= '0' && ch <= '9');
    assert(!(str[pos+len] >= '0' && str[pos+len] <= '9'));
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
        const size_t end_index = pos+len;

        // Backup the character at the end, and replace with '\0' so that GMP knows to stop parsing the int.
        const char backup = str[end_index];
        str[end_index] = '\0';

        // Parse the null-terminated str
        fmpz f = 0;
        const auto code = fmpz_set_str(&f, str.data()+pos, 10);
        assert(code == 0);

        // Restore the original str
        str[end_index] = backup;

        return f;
    }
}

void fmpz_init_set_strview(fmpz_t f, std::string_view str) {
    *f = fmpz_from_strview(str);
}

void fmpz_init_set_mutable_str(fmpz_t f, std::string& str, size_t pos, size_t len) {
    *f = fmpz_from_mutable_str(str, pos, len);
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

        // TODO: depending on benchmarks, switch this function
        _fmpq_get_str(str.data() + start_index, base, num, den);

        // Resize where prior allocation exceeded the actual need
        const size_t null_terminator_index = str.find('\0', start_index);
        str.resize(null_terminator_index);
    }
}
template void write_big_rational<false>(std::string&, const fmpq_t);
template void write_big_rational<true>(std::string&, const fmpq_t);

fmpq fmpq_from_decimaltail_str(std::string_view str) {
    assert(str.at(0) == '.');
    #ifndef NDEBUG
    for(size_t i = 1; i < str.size(); i++) assert(str[i] >= '0' && str[i] <= '9');
    #endif

    // Omit trailing zeros
    size_t back_index = str.size()-1;
    while(str[back_index] == '0') back_index--;
    if(back_index == 0) return *FMPQ_ZERO;

    fmpz num = fmpz_from_strview(str.substr(1, back_index));

    // The denominator has factors (2 * 5)^len(digits)
    // Because we have eliminated all factors of 10 from the numerator,
    // the common factors are, mutually exclusively:
    //   Instances of 2
    //   Instances of 5

    if(str.back() == '5'){
        const size_t den_num_2_factors = str.size()-1;

        fmpz reduced_num = 0;
        fmpz_remove(&reduced_num, &num, FMPZ_FIVE);
        fmpz extracted_5_factors = 0;
        fmpz_divexact(&extracted_5_factors, &num, &reduced_num);

        fmpz den = 0;
        fmpz_pow_ui(&den, FMPZ_FIVE, str.size()-1);
        fmpz_mul_2exp(&den, &den, den_num_2_factors);
        fmpz_divexact(&den, &den, &extracted_5_factors);

        fmpz_clear(&num);
        fmpz_clear(&extracted_5_factors);

        return {reduced_num, den};
    }else{
        const size_t den_num_5_factors = str.size()-1;
        const size_t num_2_factors = fmpz_val2(&num);
        const size_t den_num_2_factors = den_num_5_factors - num_2_factors;

        fmpz_tdiv_q_2exp(&num, &num, num_2_factors);

        fmpz den = 0;
        fmpz_pow_ui(&den, FMPZ_FIVE, den_num_5_factors);
        fmpz_mul_2exp(&den, &den, den_num_2_factors);

        return {num, den};
    }
}

fmpq fmpq_from_decimal_str(std::string_view str) {
    return fmpq_from_decimal_str(str, str.find('.'));
}

fmpq fmpq_from_decimal_str(std::string_view str, size_t decimal_index) {
    assert(str.at(decimal_index) == '.');
    assert(str.length() >= 2);
    #ifndef NDEBUG
    for(size_t i = 0; i < str.size(); i++) assert((str[i] >= '0' && str[i] <= '9') || (i == decimal_index));
    #endif

    if(decimal_index == 0) return fmpq_from_decimaltail_str(str);

    fmpq tail = fmpq_from_decimaltail_str(str.substr(decimal_index));
    fmpz lead = fmpz_from_strview(str.substr(0, decimal_index));
    fmpq_add_fmpz(&tail, &tail, &lead);

    return tail;
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
