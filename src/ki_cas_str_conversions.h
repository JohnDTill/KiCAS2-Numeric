#ifndef KI_CAS_STR_CONVERSIONS_H
#define KI_CAS_STR_CONVERSIONS_H

// TODO: finish revising these functions and delete this file

#ifdef _MSC_VER
#include <malloc.h>
#endif

#include <flint/fmpz.h>
#include "ki_cas_big_num_wrapper.h"
#include "ki_cas_native_rational.h"
#include <string>

namespace KiCAS2 {

/// Set a big rational from a decimal number string.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strdecimal2bigrat_NULL_TERMINATED__NOT_THREADSAFE(fmpq_t f, std::string_view str);

/// Set a big rational from a decimal number string.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strdecimal2bigrat_NULL_TERMINATED__NOT_THREADSAFE(fmpq_t f, std::string_view str, size_t decimal_index);

/// Set a big rational from a decimal number string.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strdecimal2bigrat_NULL_TERMINATED__NOT_THREADSAFE(
    fmpq_t f, std::string_view str_lead, std::string_view str_trail);

/// Set a big rational from a scientific number string.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strscientific2bigrat_NULL_TERMINATED__NOT_THREADSAFE(fmpq_t f, std::string_view str);

/// Set a big rational from a scientific number string.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strscientific2bigrat_NULL_TERMINATED__NOT_THREADSAFE(
    fmpq_t f, std::string_view str, size_t decimal_index, size_t e_index);

/// Set a big integer from a scientific integer string with positive exponent.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strsciint2bigint_NULL_TERMINATED__NOT_THREADSAFE(mpz_t f, std::string_view str);

/// Set a big integer from a scientific integer string with positive exponent.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strsciint2bigint_NULL_TERMINATED__NOT_THREADSAFE(mpz_t f, std::string_view str, size_t e_index);

/// Set a big rational from a scientific integer string with negative exponent.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strsciint2bigrat_NULL_TERMINATED__NOT_THREADSAFE(fmpq_t f, std::string_view str);

/// Set a big rational from a scientific integer string with negative exponent.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strsciint2bigrat_NULL_TERMINATED__NOT_THREADSAFE(fmpq_t f, std::string_view str, size_t e_index);

#ifndef NDEBUG
bool& spoofBignumPath();
#endif

}  // namespace KiCAS2

#endif // KI_CAS_STR_CONVERSIONS_H
