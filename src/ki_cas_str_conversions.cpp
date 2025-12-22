#include "ki_cas_str_conversions.h"

#include <cassert>
#include <charconv>
#include "ki_cas_integer_math.h"
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

bool ckd_strdecimal2rat(NativeRational* result, std::string_view str) noexcept {
    const auto decimal_index = str.find('.');
    assert(decimal_index != std::string::npos);

    return ckd_strdecimal2rat(result, str, decimal_index);
}

bool ckd_strdecimal2rat(NativeRational* result, std::string_view str, size_t decimal_index) noexcept {
    assert(str.at(decimal_index) == '.');

    std::string_view lead = str.substr(0, decimal_index);

    // Omit trailing zeros
    size_t back_index = str.size()-1;
    while(str[back_index] == '0') back_index--;

    if(back_index == decimal_index){
        // Only trailing zeros, e.g. "2.0"
        result->den = 1;
        return ckd_str2int(&result->num, lead);
    }else{
        std::string_view trail = str.substr(decimal_index+1, back_index-decimal_index);
        return ckd_strdecimal2rat(result, lead, trail);
    }
}

bool ckd_strdecimal2rat(NativeRational* result, std::string_view str_lead, std::string_view str_trail) noexcept {
    // Precondition: strings are from the same number source, separated by a decimal point
    assert(str_trail.data() == str_lead.data() + str_lead.size() + 1);
    assert(*(str_lead.data() + str_lead.size()) == '.');
    assert(*(str_trail.data() - 1) == '.');

    // Precondition: there are no trailing zeros
    assert(str_trail.back() != '0');

    if(str_trail.size() >= std::numeric_limits<size_t>::digits10) return true;

    size_t lead;
    size_t trail;
    if(ckd_str2int(&lead, str_lead) || ckd_str2int(&trail, str_trail)) return true;

    // a.b = a + b/10^len(b)

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
        #if defined(__x86_64) || defined( _WIN64 )  // 64-bit
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
    size_t den = powers_of_ten[str_trail.size()];

    // The factors of the denominator are:
    //   Up to one instance of 2
    //   Arbitrarily many instances of 5
    //
    // Since the factors of the denominator are known, reducing here is cheap

    // Remove a factor of 2
    const bool is_even = (trail % 2 == 0);
    den >>= is_even;
    trail >>= is_even;

    // Remove all factors of 5
    // The first factor has a cheap test given the string representation
    const bool has_factor_of_5 = (str_trail.back() == '5');
    if(has_factor_of_5){
        trail /= 5;
        den /= 5;

        size_t trail_div_5 = trail / 5;
        size_t trail_mod_5 = trail % 5;
        while(trail_mod_5 == 0){
            trail = trail_div_5;
            den /= 5;

            trail_div_5 = trail / 5;
            trail_mod_5 = trail % 5;
        }
    }

    result->den = den;

    // No further reduction attempted since the denominator is fully reduced.

    size_t lead_times_den;
    return ckd_mul(&lead_times_den, lead, den) || ckd_add(&result->num, lead_times_den, trail);
}

}  // namespace KiCAS2
