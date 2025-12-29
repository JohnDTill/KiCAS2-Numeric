#include "ki_cas_number.h"

#include <cassert>
#include "ki_cas_str_conversions.h"

namespace KiCAS2 {

constexpr bool POSITIVE = true;
constexpr bool NEGATIVE = false;

Number::Number() noexcept {}

Number::Number(Number&& num) noexcept {
    data = num.data;
    num.data.setTag(PosInt);  // Overwrite tag to prevent big num deallocation
}

Number::~Number() noexcept {}

void Number::operator=(Number&& num) noexcept {
    data = num.data;
    num.data.setTag(PosInt);  // Overwrite tag to prevent big num deallocation
}

void Number::normaliseInPlace() {
    data.normaliseInPlace();
}

Number Number::fromIntegerString(std::string_view str) {
    Number num;
    num.data.setFromIntegerString(str);
    return num;
}

Number Number::fromIntegerStringOverflowToFloat(std::string_view str) noexcept {
    Number num;
    num.data.setFromIntegerStringOverflowToFloat(str);
    return num;
}

Number Number::fromDecimalString(std::string_view str) {
    const size_t decimal_index = str.find('.');
    assert(decimal_index != std::string::npos);

    return fromDecimalString(str, decimal_index);
}

Number Number::fromDecimalString(std::string_view str, size_t decimal_index) {
    Number num;
    num.data.setFromDecimalString(str, decimal_index);
    return num;
}

Number Number::fromDecimalStringOverflowToFloat(std::string_view str) noexcept {
    const size_t decimal_index = str.find('.');
    assert(decimal_index != std::string::npos);

    return fromDecimalStringOverflowToFloat(str, decimal_index);
}

Number Number::fromDecimalStringOverflowToFloat(std::string_view str, size_t decimal_index) noexcept {
    Number num;
    num.data.setFromDecimalStringOverflowToFloat(str, decimal_index);
    return num;
}

Number Number::fromScientificString(std::string_view str) {
    const size_t decimal_index = str.find('.');
    assert(decimal_index != std::string::npos);

    const size_t e_index = str.find('e', decimal_index);
    assert(e_index != std::string::npos);

    return fromScientificString(str, decimal_index, e_index);
}

Number Number::fromScientificString(std::string_view str, size_t decimal_index, size_t e_index) {
    Number num;
    num.data.setFromScientificString(str, decimal_index, e_index);
    return num;
}

Number Number::fromScientificStringOverflowToFloat(std::string_view str) noexcept {
    const size_t decimal_index = str.find('.');
    assert(decimal_index != std::string::npos);

    const size_t e_index =str.find('e', decimal_index);
    assert(e_index != std::string::npos);

    return fromScientificStringOverflowToFloat(str, decimal_index, e_index);
}

Number Number::fromScientificStringOverflowToFloat(
    std::string_view str, size_t decimal_index, size_t e_index) noexcept {
    Number num;
    num.data.setFromScientificStringOverflowToFloat(str, decimal_index, e_index);
    return num;
}

template<bool typeset_fraction> void Number::writeString(std::string& str) const {
    switch (data.getTag()) {
        case PosInt: write_int(str, data.getPosInt()); break;
        case NegInt: str += '-'; write_int(str, data.getNegInt()); break;
        case PosRat: write_rational<typeset_fraction>(str, data.getPosRat()); break;
        case NegRat: str += '-'; write_rational<typeset_fraction>(str, data.getNegRat()); break;
        case BigInt: write_big_int(str, data.getBigInt()); break;
        case BigRat: write_big_rational<typeset_fraction>(str, data.getBigRat()); break;
        case Float: write_float(str, data.getFloatingPoint()); break;
        default: assert(false); break;
    }
}
template void Number::writeString<false>(std::string&) const;
template void Number::writeString<true>(std::string&) const;


Number::Data::~Data() noexcept {
    switch (tag) {
        case BigInt: mpz_clear(data.big_int); break;
        case BigRat: fmpq_clear(data.big_rational); break;
        default: break;
    }
}

void Number::Data::setTag(Tag set_tag) noexcept {
    tag = set_tag;
}

Number::Tag Number::Data::getTag() const noexcept {
    return tag;
}

size_t Number::Data::getPosInt() const noexcept {
    assert(isPosInt());
    return data.machine_int;
}

void Number::Data::setPosInt(size_t val) noexcept {
    assert(isPosInt());
    data.machine_int = val;
}

size_t Number::Data::getNegInt() const noexcept {
    assert(isNegInt());
    return data.machine_int;
}

void Number::Data::setNegInt(size_t val) noexcept {
    assert(isNegInt());
    data.machine_int = val;
}

size_t Number::Data::getMachineInt() const noexcept {
    assert(isMachineInt());
    return data.machine_int;
}

void KiCAS2::Number::Data::setMachineInt(size_t val) noexcept {
    assert(isMachineInt());
    data.machine_int = val;
}

NativeRational Number::Data::getPosRat() const noexcept {
    assert(isPosNativeRat());
    return data.native_rational;
}

NativeRational Number::Data::getNegRat() const noexcept {
    assert(isNegNativeRat());
    return data.native_rational;
}

template<bool is_positive>
void Number::Data::canonicaliseNativeRat() noexcept {
    assert(isNativeRat());
    data.native_rational.reduceInPlace();
    if(data.native_rational.den == 1) tag = (is_positive ? PosInt : NegInt);
}

size_t Number::Data::getNumerator() const noexcept {
    assert(isNativeRat());
    return data.native_rational.num;
}

void Number::Data::setNumerator(size_t val) noexcept {
    assert(isNativeRat());
    data.native_rational.num = val;
}

size_t Number::Data::getDenominator() const noexcept {
    assert(isNativeRat());
    return data.native_rational.den;
}

void Number::Data::setDenominator(size_t val) noexcept {
    assert(isNativeRat());
    data.native_rational.den = val;
}

const BigInteger& Number::Data::getBigInt() const noexcept {
    assert(isBigInt());
    return data.big_int;
}

void Number::Data::setBigInt(BigInteger val) noexcept {
    assert(isBigInt());
    *data.big_int = *val;
}

void Number::Data::tryConvertToSmallInt() noexcept {
    assert(isBigInt());
    if(!mpz_fits_ulong_p(data.big_int)) return;
    tag = mpz_sgn(data.big_int) >= 0 ? PosInt : NegInt;
    data.machine_int = mpz_get_ui(data.big_int);
}

const BigRational& Number::Data::getBigRat() const noexcept {
    assert(isBigRat());
    return data.big_rational;
}

void Number::Data::canonicaliseBigRat() {
    assert(isBigRat());
    fmpq_canonicalise(data.big_rational);
    if(fmpz_is_one(fmpq_denref(data.big_rational))){
        tag = BigInt;
        tryConvertToSmallInt();
    }else if(fmpz_abs_fits_ui(fmpq_numref(data.big_rational)) && fmpz_abs_fits_ui(fmpq_denref(data.big_rational))){
        tag = fmpq_sgn(data.big_rational) >= 0 ? PosRat : NegRat;
        absValInPlace(data.big_rational);
        data.native_rational.num = fmpz_get_ui(fmpq_numref(data.big_rational));
        data.native_rational.den = fmpz_get_ui(fmpq_denref(data.big_rational));
    }
}

FloatingPoint Number::Data::getFloatingPoint() const noexcept {
    assert(isFloatingPoint());
    return data.floating_point;
}

void Number::Data::setFloatingPoint(FloatingPoint val) noexcept {
    assert(isFloatingPoint());
    data.floating_point = val;
}

void Number::Data::setFromIntegerString(std::string_view str) {
    if(ckd_str2int(&data.machine_int, str) == false){
        tag = PosInt;
    }else{
        str2bigint_NULL_TERMINATED__NOT_THREADSAFE(data.big_int, str);
        tag = BigInt;
    }
}

void Number::Data::setFromIntegerStringOverflowToFloat(std::string_view str) noexcept {
    if(ckd_str2int(&data.machine_int, str) == false){
        tag = PosInt;
    }else{
        data.floating_point = strdecimal2floatingpoint(str);
        tag = Float;
    }
}

void Number::Data::setFromDecimalString(std::string_view str, size_t decimal_index) {
    if(ckd_strdecimal2rat(&data.native_rational, str, decimal_index) == false){
        tag = data.native_rational.den == 1 ? PosInt : PosRat;
    }else{
        strdecimal2bigrat_NULL_TERMINATED__NOT_THREADSAFE(data.big_rational, str, decimal_index);
        tag = BigRat;
    }
}

void Number::Data::setFromDecimalStringOverflowToFloat(std::string_view str, size_t decimal_index) noexcept {
    if(ckd_strdecimal2rat(&data.native_rational, str, decimal_index) == false){
        tag = data.native_rational.den == 1 ? PosInt : PosRat;
    }else{
        data.floating_point = strdecimal2floatingpoint(str);
        tag = Float;
    }
}

void Number::Data::setFromScientificString(std::string_view str, size_t decimal_index, size_t e_index) {
    if(ckd_strscientific2rat(&data.native_rational, str, decimal_index, e_index) == false){
        tag = data.native_rational.den == 1 ? PosInt : PosRat;
    }else{
        strscientific2bigrat_NULL_TERMINATED__NOT_THREADSAFE(data.big_rational, str, decimal_index, e_index);
        tag = BigRat;
    }
}

void Number::Data::setFromScientificStringOverflowToFloat(
    std::string_view str, size_t decimal_index, size_t e_index) noexcept {
    if(ckd_strscientific2rat(&data.native_rational, str, decimal_index, e_index) == false){
        tag = data.native_rational.den == 1 ? PosInt : PosRat;
    }else{
        data.floating_point = strscientific2floatingpoint(str);
        tag = Float;
    }
}

void Number::Data::normaliseInPlace() {
    switch(getTag()){
        case PosInt: break;
        case NegInt: break;
        case BigInt: tryConvertToSmallInt(); break;
        case PosRat: canonicaliseNativeRat<POSITIVE>(); break;
        case NegRat: canonicaliseNativeRat<NEGATIVE>(); break;
        case BigRat: canonicaliseBigRat(); break;
        case Float: break;
        default: assert(false); break;
    }
}

#ifndef NDEBUG
bool Number::Data::isPosInt() const noexcept {
    return tag == PosInt;
}

bool Number::Data::isNegInt() const noexcept {
    return tag == NegInt;
}

bool Number::Data::isMachineInt() const noexcept {
    return tag == PosInt || tag == NegInt;
}

bool Number::Data::isPosNativeRat() const noexcept {
    return tag == PosRat;
}

bool Number::Data::isNegNativeRat() const noexcept {
    return tag == NegRat;
}

bool Number::Data::isNativeRat() const noexcept {
    return tag == PosRat || tag == NegRat;
}

bool Number::Data::isBigInt() const noexcept {
    return tag == BigInt;
}

bool Number::Data::isBigRat() const noexcept {
    return tag == BigRat;
}

bool Number::Data::isFloatingPoint() const noexcept {
    return tag == Float;
}
#endif

}  // namespace KiCAS2
