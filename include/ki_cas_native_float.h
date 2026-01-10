#ifndef KI_CAS_NATIVE_FLOAT_H
#define KI_CAS_NATIVE_FLOAT_H

#include <string>

namespace KiCAS2 {

typedef long double FloatingPoint;

/// Append a float to the end of the string
void write_float(std::string& str, FloatingPoint val);

/// Parse a string to a floating point number
FloatingPoint strdecimal2floatingpoint(std::string_view str) noexcept;

/// Parse a string to a floating point number
FloatingPoint strscientific2floatingpoint(std::string_view str) noexcept;

}

#endif // KI_CAS_NATIVE_FLOAT_H
