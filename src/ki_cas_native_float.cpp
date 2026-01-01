#include "ki_cas_native_float.h"

#include <cassert>
#include <charconv>

namespace KiCAS2 {

void write_float(std::string& str, FloatingPoint val) {
#if !defined(__GNUC__) || __GNUC__ > 8
    constexpr size_t max_digits = 64;
    char buffer[max_digits];
    const std::to_chars_result result = std::to_chars(buffer, buffer+max_digits, val, std::chars_format::general);
    assert(result.ec == std::errc());
    str.append(buffer, result.ptr - buffer);
#else
    // Older GCC versions don't implement std::to_chars for floats
    str += std::to_string(val);
#endif
}

FloatingPoint strdecimal2floatingpoint(std::string_view str) noexcept {
    long double result;

#if !defined(__GNUC__) || __GNUC__ > 8
    const auto parse_result = std::from_chars(str.data(), str.data() + str.size(), result, std::chars_format::fixed);
    assert(parse_result.ptr == str.data()+str.size());
#else
    result = std::strtold(str.data(), nullptr);
#endif

    return result;
}

FloatingPoint strscientific2floatingpoint(std::string_view str) noexcept {
    long double result;

#if !defined(__GNUC__) || __GNUC__ > 8
    const auto parse_result = std::from_chars(str.data(), str.data() + str.size(), result, std::chars_format::scientific);
    assert(parse_result.ptr == str.data()+str.size());
#else
    result = std::strtold(str.data(), nullptr);
#endif

    return result;
}

}
