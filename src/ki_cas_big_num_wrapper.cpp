#include "ki_cas_big_num_wrapper.h"

#include <cassert>
#include "ki_cas_native_integer.h"

#ifndef NDEBUG
#include <memory.h>
#include <shared_mutex>
#include <unordered_set>
#endif

#include <iostream>  // TODO

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

template<bool constant_str> void mpz_init_set_strview(mpz_t f, std::string_view str) {
    assert(str.find('.') == std::string::npos);
    assert(str.find('e') == std::string::npos);

    if(str.size() < std::numeric_limits<size_t>::digits10){
        mpz_init_set_ui(f, knownfit_str2int(str));
    #if (!defined(__x86_64__) && !defined(__aarch64__) && !defined(_WIN64)) || !defined(_MSC_VER)
    }else if(str.size() < std::numeric_limits<WideType>::digits10){
        const DoubleInt val = knownfit_str2wideint(str);
        mpz_init(f);
        fmpz ptr = PTR_TO_COEFF(f);
        fmpz_set_uiui(&ptr, val.high, val.low);
    #endif
    }else if(constant_str){
        std::string copy(str);
        mpz_init(f);
        const auto code = mpz_set_str(f, copy.data(), 10);
        assert(code == 0);
    }else{
        // Get the end of the std::string_view, violating the purported immutability of std::string_view!
        // The byte past "str" must be valid owned memory! (std::string is specified to be null-terminated since C++11)
        // This also means the string cannot be read from another thread during this operation.
        char* end_const_discarded = const_cast<char*>(str.data()+str.size());

        // Backup the character at the end, and replace with '\0' so that GMP knows to stop parsing the int.
        const char backup = *end_const_discarded;
        *end_const_discarded = '\0';

        // Parse the null-terminated str
        mpz_init(f);
        const auto code = mpz_set_str(f, str.data(), 10);
        assert(code == 0);

        // Restore the original str
        *end_const_discarded = backup;
    }
}
template void mpz_init_set_strview<false>(mpz_t f, std::string_view str);
template void mpz_init_set_strview<true>(mpz_t f, std::string_view str);

template<bool constant_str> void fmpz_init_set_strview(fmpz_t f, std::string_view str) {
    assert(str.find('.') == std::string::npos);
    assert(str.find('e') == std::string::npos);

    if(str.size() < std::numeric_limits<size_t>::digits10){
        fmpz_init_set_ui(f, knownfit_str2int(str));
    #if (!defined(__x86_64__) && !defined(__aarch64__) && !defined(_WIN64)) || !defined(_MSC_VER)
    }else if(str.size() < std::numeric_limits<WideType>::digits10){
        const DoubleInt val = knownfit_str2wideint(str);
        fmpz_init(f);
        fmpz_set_uiui(f, val.high, val.low);
    #endif
    }else if(constant_str){
        std::string copy(str);
        fmpz_init(f);
        const auto code = fmpz_set_str(f, copy.data(), 10);
        assert(code == 0);
    }else{
        // Get the end of the std::string_view, violating the purported immutability of std::string_view!
        // The byte past "str" must be valid owned memory! (std::string is specified to be null-terminated since C++11)
        // This also means the string cannot be read from another thread during this operation.
        char* end_const_discarded = const_cast<char*>(str.data()+str.size());

        // Backup the character at the end, and replace with '\0' so that GMP knows to stop parsing the int.
        const char backup = *end_const_discarded;
        *end_const_discarded = '\0';

        // Parse the null-terminated str
        fmpz_init(f);
        const auto code = fmpz_set_str(f, str.data(), 10);
        assert(code == 0);

        // Restore the original str
        *end_const_discarded = backup;
    }
}
template void fmpz_init_set_strview<false>(fmpz_t f, std::string_view str);
template void fmpz_init_set_strview<true>(fmpz_t f, std::string_view str);

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
