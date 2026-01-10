#ifndef KI_CAS_NATIVE_INTEGER_H
#define KI_CAS_NATIVE_INTEGER_H

#include <stdint.h>
#include <stddef.h>
#include <string>

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

/// Append an integer to the end of the string
void write_native_int(std::string& str, size_t val);

/// Set an integer from a string of the form `['0' - '9']+`. Returns true if the value is too large to fit.
bool ckd_str2int(size_t* result, std::string_view str) noexcept;

/// Set an integer from a string of the form `['0' - '9']+`.
/// Incudes debug assertion that the conversion does not overflow.
size_t knownfit_str2int(std::string_view str) noexcept;

#if (!defined(__x86_64__) && !defined(__aarch64__) && !defined(_WIN64)) || !defined(_MSC_VER)

#if !defined(__x86_64__) && !defined(__aarch64__) && !defined(_WIN64)
typedef uint64_t WideType;
#else
typedef __uint128_t WideType;
#endif

struct DoubleInt {
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    size_t low;
    size_t high;
    #else
    size_t high;
    size_t low;
    #endif
};

/// Set an integer from a string of the form `['0' - '9']+`.
/// Incudes debug assertion that the conversion does not overflow.
DoubleInt knownfit_str2wideint(std::string_view str) noexcept;
#endif

}  // namespace KiCAS2

#endif // KI_CAS_NATIVE_INTEGER_H
