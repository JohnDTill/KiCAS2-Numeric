#include "ki_cas_number.h"

#include <cassert>

namespace KiCAS2 {

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

size_t Number::Data::getNumerator() const noexcept {
    assert(isMachineRat());
    return data.machine_rational.numerator;
}

void Number::Data::setNumerator(size_t val) noexcept {
    assert(isMachineRat());
    data.machine_rational.numerator = val;
}

size_t Number::Data::getDenominator() const noexcept {
    assert(isMachineRat());
    return data.machine_rational.denominator;
}

void Number::Data::setDenominator(size_t val) noexcept {
    assert(isMachineRat());
    data.machine_rational.denominator = val;
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

bool Number::Data::isMachineRat() const noexcept {
    return tag == PosRat || tag == NegRat;
}
#endif

}
