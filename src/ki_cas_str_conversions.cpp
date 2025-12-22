#include "ki_cas_str_conversions.h"

#include <cassert>
#include <charconv>
#include <limits>

namespace KiCAS2 {

void write_int(std::string& str, size_t val) {
    constexpr size_t max_digits = std::numeric_limits<size_t>::digits10 + 1;
    str.resize(str.size() + max_digits);

    // Append the number to the end of str
    char* end = str.data() + str.size();
    char* begin = end - max_digits;
    std::to_chars_result result = std::to_chars(begin, end, val);
    assert(result.ec == std::errc());

    // Trim size where prior allocation exceeded the actual need
    str.resize(result.ptr - str.data());
}

bool ckd_str2int(size_t* result, std::string_view str) noexcept {
    const auto parse_result = std::from_chars(str.data(), str.data() + str.size(), *result);
    assert(parse_result.ec != std::errc::invalid_argument);

    return parse_result.ec == std::errc::result_out_of_range;
}

void fmpz_set_str_NULL_TERMINATED_SOURCE__NOT_THREADSAFE(fmpz_t f, std::string_view str) {
    // Get the end of the std::string_view, violating the purported immutability of std::string_view!
    // The byte past "str" must be valid owned memory! (std::string is specified to be null-terminated since C++11)
    // This also means the string cannot be read from another thread during this operation.
    char* end_const_discarded = const_cast<char*>(str.data()+str.size());

    // Backup the character at the end, and replace with '\0' so that GMP knows to stop parsing the int.
    const char backup = *end_const_discarded;
    *end_const_discarded = '\0';

    // Parse the null-terminated str
    const auto code = fmpz_set_str(f, str.data(), 10);
    assert(code == 0);

    // Restore the original str
    *end_const_discarded = backup;
}

template<bool typeset_fraction>
void write_rational(std::string& str, NativeRational val) {
    if(typeset_fraction) str += "⁜f⏴";
    write_int(str, val.num);
    if(typeset_fraction) str += "⏵⏴";
    else str += '/';
    write_int(str, val.den);
    if(typeset_fraction) str += "⏵";
}
template void write_rational<false>(std::string&, NativeRational);
template void write_rational<true>(std::string&, NativeRational);

bool ckd_strdecimal2rat(NativeRational* result, std::string_view str_lead, std::string_view str_trail) noexcept {
    // TODO: figure out the right function signature for ease and correctness
    // assert(str_trail.data() == str_lead.data() + str_lead.size() + 1);
    // assert(*(str_lead.data() + str_lead.size()) == '.');
    // assert(*(str_trail.data() - 1) == '.');

    // TODO: you know the factors of the denominator, so reducing here is cheap

    // TODO
    // a.b = f"{a}{b}/{10^len(b)}" if len(a) + len(b) fits

    // a.b = a + b/10^len(b)
    if(str_trail.size() >= std::numeric_limits<size_t>::digits10) return true;

    size_t lead;
    size_t trail;
    if(ckd_str2int(&lead, str_lead) || ckd_str2int(&trail, str_trail)) return true;

    constexpr size_t powers_of_ten[std::numeric_limits<size_t>::digits10] = {
        1,
        10,
        100,
        1000,
        10000,
        100000,
        1000000,
        10000000,
        100000000,
        #if __WORDSIZE >= 64
        1000000000,
        10000000000,
        100000000000,
        1000000000000,
        10000000000000,
        100000000000000,
        1000000000000000,
        10000000000000000,
        100000000000000000,
        1000000000000000000,
        #endif
    };

    return ckd_add(result, NativeRational(trail, powers_of_ten[str_trail.size()]), lead);
}

}  // namespace KiCAS2
