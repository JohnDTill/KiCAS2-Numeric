#ifndef KI_CAS_BIG_NUM_WRAPPER_H
#define KI_CAS_BIG_NUM_WRAPPER_H

#ifdef _MSC_VER
#include <malloc.h>  // MSC dependencies for GMP
#endif

#include <gmp.h>
#include <flint/fmpq.h>
#include <flint/fmpz.h>

#include "ki_cas_typesetting_flags.h"
#include <string>
#include <string_view>

namespace KiCAS2 {

/// fmpz_t representing 0
inline constexpr fmpz_t FMPZ_ZERO = {0};

/// fmpz_t representing 1
inline constexpr fmpz_t FMPZ_ONE = {1};

/// fmpz_t representing 5
inline constexpr fmpz_t FMPZ_FIVE = {5};

/// fmpz_t representing 10
inline constexpr fmpz_t FMPZ_TEN = {10};

/// fmpq_t representing 0
inline constexpr fmpq_t FMPQ_ZERO = {{*FMPZ_ZERO, *FMPZ_ONE}};

/// fmpq_t representing 1
inline constexpr fmpq_t FMPQ_ONE = {{*FMPZ_ONE, *FMPZ_ONE}};

/// Determine if an mpz_t is strictly negative, value in [-∞, 0)
bool mpz_is_neg(const mpz_t op) noexcept;

/// Reverse the sign of an mpz_t in place
void mpz_neg_inplace(mpz_t rop) noexcept;

/// Find 10 to the power of a size_t value
void fmpz_10_pow_ui(fmpz_t f, ulong rhs);

/// Find 10 to the power of an fmpz_t value
void fmpz_10_pow_fmpz(fmpz_t f, const fmpz_t rhs);

/// Return an overestimate of how many base10 digits are required to express the mpz_t
size_t mpz_sizeinbase10upperbound(const mpz_t val) noexcept;

/// Return an overestimate of how many base10 digits are required to express the fmpz_t
size_t fmpz_sizeinbase10upperbound(const fmpz_t val) noexcept;

/// Take the absolute value of an fmpq_t in place
void fmpq_abs_inplace(fmpq_t val) noexcept;

/// Set an mpz_t from a string.
void mpz_init_set_strview(mpz_t f, std::string_view str);

/// Create an mpz_t from a string.
fmpz fmpz_from_strview(std::string_view str);

/// Set an fmpz_t from a string.
void fmpz_init_set_strview(fmpz_t f, std::string_view str);

/// Append an mpz_t to the end of the string
void write_big_int(std::string& str, const mpz_t val);

/// Append an fmpq_t to the end of the string
template<bool typeset_fraction=false> void write_big_rational(std::string& str, const fmpq_t val);

/// Create an fmpq_t from a string of the form `(['0'-'9']+ '.' ['0'-'9']*) | ['0'-'9']* '.' ['0'-'9']+`..
fmpq fmpq_from_decimal_str(std::string_view str);

/// Create an fmpq_t from a string of the form `(['0'-'9']+ '.' ['0'-'9']*) | ['0'-'9']* '.' ['0'-'9']+`..
fmpq fmpq_from_decimal_str(std::string_view str, size_t decimal_index);

/// Create an fmpq_t from a string of the form:
/// `['0'-'9']+ ('.' ['0'-'9']*)? 'e' ('+' | '-')? ['0'-'9']+`.
/// or `'.' ['0'-'9']+ 'e' ('+' | '-')? ['0'-'9']+`
fmpq fmpq_from_scientific_str(std::string_view str);

#ifndef NDEBUG
bool isAllGmpMemoryFreed() noexcept;
void resetMemoryTracking() noexcept;
bool isAllGmpMemoryFreed_resetOnFalse() noexcept;  /// Resets to avoid cascading test failures
#define DEBUG_REQUIRE(x) REQUIRE(x)
#define DEBUG_REQUIRE_FALSE(x) REQUIRE_FALSE(x)
#else
#define DEBUG_REQUIRE(x)
#define DEBUG_REQUIRE_FALSE(x)
#endif

}

#endif // KI_CAS_BIG_NUM_WRAPPER_H
