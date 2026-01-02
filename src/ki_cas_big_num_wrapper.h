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

/// Determine if an mpz_t is strictly negative, value in [-∞, 0)
bool mpz_is_neg(const mpz_t op) noexcept;

/// Reverse the sign of an mpz_t in place
void mpz_neg_inplace(mpz_t rop) noexcept;

void mpz_init_set_ui_mul_ui(mpz_t ans, ulong lhs, ulong rhs);  // TODO: reconsider this

/// Return an overestimate of how many base10 digits are required to express the mpz_t
size_t mpz_sizeinbase10upperbound(const mpz_t val) noexcept;

/// Return an overestimate of how many base10 digits are required to express the fmpz_t
size_t fmpz_sizeinbase10upperbound(const fmpz_t val) noexcept;

/// Take the absolute value of an fmpq_t in place
void fmpq_abs_inplace(fmpq_t val) noexcept;

/// Flag indicating string can be modified
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable.
inline constexpr bool ALLOW_STRING_MODIFICATION = false;

/// Flag indicating string inputs should not be mutated
inline constexpr bool MEMORY_SAFE = true;

/// Set an mpz_t from a string.
template<bool constant_str=true> void mpz_init_set_strview(mpz_t f, std::string_view str);

/// Set an fmpz_t from a string.
template<bool constant_str=true> void fmpz_init_set_strview(fmpz_t f, std::string_view str);

/// Append an mpz_t to the end of the string
void write_big_int(std::string& str, const mpz_t val);

/// Append an fmpq_t to the end of the string
template<bool typeset_fraction=false> void write_big_rational(std::string& str, const fmpq_t val);

#ifndef NDEBUG
bool isAllGmpMemoryFreed() noexcept;
#define DEBUG_REQUIRE(x) REQUIRE(x)
#define DEBUG_REQUIRE_FALSE(x) REQUIRE_FALSE(x)
#else
#define DEBUG_REQUIRE(x)
#define DEBUG_REQUIRE_FALSE(x)
#endif

}

#endif // KI_CAS_BIG_NUM_WRAPPER_H
