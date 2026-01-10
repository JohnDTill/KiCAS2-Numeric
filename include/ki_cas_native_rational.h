#ifndef KI_CAS_NATIVE_RATIONAL_H
#define KI_CAS_NATIVE_RATIONAL_H

#include "ki_cas_typesetting_flags.h"
#include <stddef.h>
#include <string>

namespace KiCAS2 {

struct NativeRational {
    size_t num;
    size_t den;

    NativeRational() noexcept = default;
    NativeRational(size_t numerator, size_t denominator) noexcept;
    operator long double() const noexcept;
    operator double() const noexcept;
    operator float() const noexcept;
    explicit operator size_t() const noexcept;

    friend bool operator==(NativeRational a, size_t b) noexcept;
    friend bool operator!=(NativeRational a, size_t b) noexcept;
    friend bool operator==(size_t a, NativeRational b) noexcept;
    friend bool operator!=(size_t a, NativeRational b) noexcept;
    friend bool operator>(NativeRational a, size_t b) noexcept;
    friend bool operator>=(NativeRational a, size_t b) noexcept;
    friend bool operator<(NativeRational a, size_t b) noexcept;
    friend bool operator<=(NativeRational a, size_t b) noexcept;
    friend bool operator>(size_t a, NativeRational b) noexcept;
    friend bool operator>=(size_t a, NativeRational b) noexcept;
    friend bool operator<(size_t a, NativeRational b) noexcept;
    friend bool operator<=(size_t a, NativeRational b) noexcept;

    friend bool operator==(NativeRational a, NativeRational b) noexcept;
    friend bool operator!=(NativeRational a, NativeRational b) noexcept;
    friend bool operator>(NativeRational a, NativeRational b) noexcept;
    friend bool operator>=(NativeRational a, NativeRational b) noexcept;
    friend bool operator<(NativeRational a, NativeRational b) noexcept;
    friend bool operator<=(NativeRational a, NativeRational b) noexcept;

    /// Canonicalise by eliminating common factors in numerator and denominator
    void reduceInPlace() noexcept;

    NativeRational reciprocal() const noexcept;
};

/// Returns true if the calculation overflows.
/// reduction is performed if required to fit, but the result is NOT canonicalised
bool ckd_mul(NativeRational* result, NativeRational a, size_t b) noexcept;

/// Returns true if the calculation overflows
/// reduction is performed if required to fit, but the result is NOT canonicalised
bool ckd_mul(NativeRational* result, NativeRational a, NativeRational b) noexcept;

/// Returns true if the calculation overflows.
/// reduction is performed if required to fit, but the result is NOT canonicalised
bool ckd_div(NativeRational* result, NativeRational a, size_t b) noexcept;

/// Returns true if the calculation overflows
/// reduction is performed if required to fit, but the result is NOT canonicalised
bool ckd_div(NativeRational* result, NativeRational a, NativeRational b) noexcept;

/// Returns true if the calculation overflows
/// reduction is performed if required to fit, but the result is NOT canonicalised
bool ckd_add(NativeRational* result, NativeRational a, size_t b) noexcept;

/// Returns true if the calculation overflows
/// reduction is performed if required to fit, but the result is NOT canonicalised
bool ckd_add(NativeRational* result, NativeRational a, NativeRational b) noexcept;

/// Requires a > b, asserts otherwise
/// This operation is fundamentally always possible without size considerations
NativeRational sub(NativeRational a, size_t b) noexcept;

/// Returns true if the calculation overflows
/// Requires a > b, asserts otherwise
/// reduction is performed if required to fit, but the result is NOT canonicalised
bool ckd_sub(NativeRational* result, size_t a, NativeRational b) noexcept;

/// Returns true if the calculation overflows
/// Requires a > b, asserts otherwise
/// reduction is performed if required to fit, but the result is NOT canonicalised
bool ckd_sub(NativeRational* result, NativeRational a, NativeRational b) noexcept;

/// Append a rational to the end of the string
template<bool typeset_fraction=false> void write_native_rational(std::string& str, NativeRational val);

/// Set a NativeRational from a string of the form `'.' ['0'-'9']*`.
/// The resulting NativeRational is fully reduced.
/// Returns true if the value is too large to fit.
bool ckd_strdecimaltail2rat(NativeRational* result, std::string_view str) noexcept;

/// Set a NativeRational from a string of the form `(['0'-'9']+ '.' ['0'-'9']*) | ['0'-'9']* '.' ['0'-'9']+`.
/// The resulting NativeRational is fully reduced.
/// Returns true if the value is too large to fit.
bool ckd_strdecimal2rat(NativeRational* result, std::string_view str) noexcept;

/// Set a NativeRational from a string of the form `(['0'-'9']+ '.' ['0'-'9']*) | ['0'-'9']* '.' ['0'-'9']+`.
/// The resulting NativeRational is fully reduced.
/// Returns true if the value is too large to fit.
bool ckd_strdecimal2rat(NativeRational* result, std::string_view str, size_t decimal_index) noexcept;

/// Set a NativeRational from a string of the form:
/// `['0'-'9']+ ('.' ['0'-'9']*)? 'e' ('+' | '-')? ['0'-'9']+`.
/// or `'.' ['0'-'9']+ 'e' ('+' | '-')? ['0'-'9']+`
/// The resulting NativeRational is fully reduced.
/// Returns true if the value is too large to fit.
bool ckd_strscientific2rat(NativeRational* result, std::string_view str) noexcept;

}  // namespace KiCAS2

#endif // KI_CAS_NATIVE_RATIONAL_H
