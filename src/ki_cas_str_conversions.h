#ifndef KI_CAS_STR_CONVERSIONS_H
#define KI_CAS_STR_CONVERSIONS_H

#ifdef _MSC_VER
#include <malloc.h>
#endif

#include <flint/fmpz.h>
#include "ki_cas_big_num_wrapper.h"
#include "ki_cas_float.h"
#include "ki_cas_native_rational.h"
#include <string>

namespace KiCAS2 {

/// Append an integer to the end of the string
void write_int(std::string& str, size_t val);

/// Append a float to the end of the string
void write_float(std::string& str, FloatingPoint val);

/// Parse a string to a floating point number
FloatingPoint strdecimal2floatingpoint(std::string_view str) noexcept;

/// Parse a string to a floating point number
FloatingPoint strscientific2floatingpoint(std::string_view str) noexcept;

/// Set an integer from a string. Returns true if the value is too large to fit.
bool ckd_str2int(size_t* result, std::string_view str) noexcept;

/// Set a big integer from a string.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void str2bigint_NULL_TERMINATED__NOT_THREADSAFE(mpz_t f, std::string_view str);

/// Set a big integer from a string.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void str2bigint_NULL_TERMINATED__NOT_THREADSAFE(fmpz_t f, std::string_view str);

inline constexpr bool PLAINTEXT_OUTPUT = false;
inline constexpr bool TYPESET_OUTPUT = true;

/// Append a rational to the end of the string
template<bool typeset_fraction> void write_rational(std::string& str, NativeRational val);

/// Set an NativeRational from a string. Returns true if the value is too large to fit.
bool ckd_strdecimal2rat(NativeRational* result, std::string_view str) noexcept;

/// Set an NativeRational from a string. Returns true if the value is too large to fit.
bool ckd_strdecimal2rat(NativeRational* result, std::string_view str, size_t decimal_index) noexcept;

/// Set an NativeRational from a string. Returns true if the value is too large to fit.
bool ckd_strdecimal2rat(NativeRational* result, std::string_view str_lead, std::string_view str_trail) noexcept;

/// Set an NativeRational from a string. Returns true if the value is too large to fit.
bool ckd_strscientific2rat(NativeRational* result, std::string_view str) noexcept;

/// Set an NativeRational from a string. Returns true if the value is too large to fit.
bool ckd_strscientific2rat(
    NativeRational* result, std::string_view str, size_t decimal_index, size_t e_index) noexcept;

/// Set an integer from a string. Returns true if the value is too large to fit.
bool ckd_strsciint2int(size_t* result, std::string_view str) noexcept;

/// Set an integer from a string. Returns true if the value is too large to fit.
bool ckd_strsciint2int(size_t* result, std::string_view str, size_t e_index) noexcept;

/// Set an NativeRational from a string. Returns true if the value is too large to fit.
bool ckd_strsciint2rat(NativeRational* result, std::string_view str) noexcept;

/// Set an NativeRational from a string. Returns true if the value is too large to fit.
bool ckd_strsciint2rat(NativeRational* result, std::string_view str, size_t e_index) noexcept;

/// Set a big rational from a decimal number string.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strdecimal2bigrat_NULL_TERMINATED__NOT_THREADSAFE(BigRational f, std::string_view str);

/// Set a big rational from a decimal number string.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strdecimal2bigrat_NULL_TERMINATED__NOT_THREADSAFE(BigRational f, std::string_view str, size_t decimal_index);

/// Set a big rational from a decimal number string.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strdecimal2bigrat_NULL_TERMINATED__NOT_THREADSAFE(
    BigRational f, std::string_view str_lead, std::string_view str_trail);

/// Set a big rational from a scientific number string.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strscientific2bigrat_NULL_TERMINATED__NOT_THREADSAFE(BigRational f, std::string_view str);

/// Set a big rational from a scientific number string.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strscientific2bigrat_NULL_TERMINATED__NOT_THREADSAFE(
    BigRational f, std::string_view str, size_t decimal_index, size_t e_index);

/// Set a big integer from a scientific integer string with positive exponent.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strsciint2bigint_NULL_TERMINATED__NOT_THREADSAFE(BigInteger f, std::string_view str);

/// Set a big integer from a scientific integer string with positive exponent.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strsciint2bigint_NULL_TERMINATED__NOT_THREADSAFE(BigInteger f, std::string_view str, size_t e_index);

/// Set a big rational from a scientific integer string with negative exponent.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strsciint2bigrat_NULL_TERMINATED__NOT_THREADSAFE(BigRational f, std::string_view str);

/// Set a big rational from a scientific integer string with negative exponent.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void strsciint2bigrat_NULL_TERMINATED__NOT_THREADSAFE(BigRational f, std::string_view str, size_t e_index);

/// Append a big integer to the end of the string
void write_big_int(std::string& str, const BigInteger val);

/// Append a big rational to the end of the string
template<bool typeset_fraction> void write_big_rational(std::string& str, const BigRational val);

}  // namespace KiCAS2

#endif // KI_CAS_STR_CONVERSIONS_H
