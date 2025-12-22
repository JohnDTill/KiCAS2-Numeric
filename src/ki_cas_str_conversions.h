#ifndef KI_CAS_STR_CONVERSIONS_H
#define KI_CAS_STR_CONVERSIONS_H

#ifdef _MSC_VER
#include <malloc.h>
#endif

#include <flint/fmpz.h>
#include "ki_cas_native_rational.h"
#include <string>

namespace KiCAS2 {

/// Append an integer to the end of the string
void write_int(std::string& str, size_t val);

/// Set an integer from a string. Returns true if the value is too large to fit.
bool ckd_str2int(size_t* result, std::string_view str) noexcept;

/// Set a Flint integer from a string.
/// @warning The underlying string must be null-terminated and not accessed in another thread. Neither
///          precondition is verifiable here.
void fmpz_set_str_NULL_TERMINATED_SOURCE__NOT_THREADSAFE(fmpz_t f, std::string_view str);

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

}  // namespace KiCAS2

#endif // KI_CAS_STR_CONVERSIONS_H
