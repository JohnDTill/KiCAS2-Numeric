#ifndef KI_CAS_NUMBER_H
#define KI_CAS_NUMBER_H

#ifdef _MSC_VER
#include <malloc.h>
#endif

#include <cstddef>
#include <flint/fmpq.h>
#include <flint/fmpz.h>

namespace KiCAS2 {

class Number {
private:
    struct Data {
    public:
        inline size_t getPosInt() const noexcept;
        inline void setPosInt(size_t val) noexcept;
        inline size_t getNegInt() const noexcept;
        inline void setNegInt(size_t val) noexcept;
        inline size_t getMachineInt() const noexcept;
        inline void setMachineInt(size_t val) noexcept;
        inline size_t getNumerator() const noexcept;
        inline void setNumerator(size_t val) noexcept;
        inline size_t getDenominator() const noexcept;
        inline void setDenominator(size_t val) noexcept;

    private:
        struct MachineRational {
            size_t numerator;
            size_t denominator;
        };

        struct Complex {
            Number* real;
            Number* imaginary;
        };

        union {
            size_t machine_int;
            MachineRational machine_rational;
            fmpz_t big_int;
            fmpq_t big_rational;
            Complex complex;
        } data;

        enum {
            PosInt,
            NegInt,
            PosRat,
            NegRat,
            BigInt,
            BitRat,
            Complex,
        } tag;

        #ifndef NDEBUG
        bool isPosInt() const noexcept;
        bool isNegInt() const noexcept;
        bool isMachineInt() const noexcept;
        bool isMachineRat() const noexcept;
        #endif
    } data;
};

}  // namespace KiCAS2

#endif  // #ifndef KI_CAS_NUMBER_H
