#ifndef KI_CAS_INTEGER_MATH_H
#define KI_CAS_INTEGER_MATH_H

#include <stddef.h>

namespace KiCAS2 {

/// Returns true if the calculation overflows
bool ckd_add(size_t* result, size_t a, size_t b) noexcept;

/// Returns true if the calculation overflows
bool ckd_mul(size_t* result, size_t a, size_t b) noexcept;

/// Incudes debug assertion that the calculation does not overflow
size_t knownfit_mul(size_t a, size_t b) noexcept;

/// Incudes debug assertion that the calculation does not underflow
size_t knownfit_sub(size_t a, size_t b) noexcept;

/// Returns true if no pure integer root exists
bool ckd_sqrt(size_t* result, size_t arg) noexcept;

/// Returns true if no pure integer root exists
bool ckd_cbrt(size_t* result, size_t arg) noexcept;

/// Returns true if no pure integer root exists
bool ckd_nrt(size_t* result, size_t arg, size_t power) noexcept;

/// Returns true if the calculation overflows
bool ckd_pow(size_t* result, size_t base, size_t power) noexcept;

/// Incudes debug assertion that the calculation does not overflow
size_t knownfit_pow(size_t base, size_t power) noexcept;

}  // namespace KiCAS2

#endif // KI_CAS_INTEGER_MATH_H
