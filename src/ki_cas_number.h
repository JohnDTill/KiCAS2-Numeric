#ifndef KI_CAS_NUMBER_H
#define KI_CAS_NUMBER_H

#include <cstddef>
#include "ki_cas_big_num_wrapper.h"
#include "ki_cas_float.h"
#include "ki_cas_native_rational.h"
#include <string>

namespace KiCAS2 {

class Number {
public:
    Number() noexcept;
    Number(Number&& num) noexcept;
    ~Number() noexcept;
    void operator=(Number&& num) noexcept;
    void normaliseInPlace();
    static Number fromIntegerString(std::string_view str);
    static Number fromIntegerStringOverflowToFloat(std::string_view str) noexcept;
    static Number fromDecimalString(std::string_view str);
    static Number fromDecimalString(std::string_view str, size_t decimal_index);
    static Number fromDecimalStringOverflowToFloat(std::string_view str) noexcept;
    static Number fromDecimalStringOverflowToFloat(std::string_view str, size_t decimal_index) noexcept;
    static Number fromScientificString(std::string_view str);
    static Number fromScientificString(std::string_view str, size_t decimal_index, size_t e_index);
    static Number fromScientificStringOverflowToFloat(std::string_view str) noexcept;
    static Number fromScientificStringOverflowToFloat(
        std::string_view str, size_t decimal_index, size_t e_index) noexcept;
    template<bool typeset_fraction> void writeString(std::string& str) const;

private:
    enum Tag {
        PosInt,
        NegInt,
        PosRat,
        NegRat,
        BigInt,
        BigRat,
        Float,
    };

    struct Data {
    public:
        ~Data() noexcept;
        Tag getTag() const noexcept;
        void setTag(Tag set_tag) noexcept;
        inline size_t getPosInt() const noexcept;
        inline void setPosInt(size_t val) noexcept;
        inline size_t getNegInt() const noexcept;
        inline void setNegInt(size_t val) noexcept;
        inline size_t getMachineInt() const noexcept;
        inline void setMachineInt(size_t val) noexcept;
        inline NativeRational getPosRat() const noexcept;
        inline NativeRational getNegRat() const noexcept;
        template<bool is_positive> inline void canonicaliseNativeRat() noexcept;
        inline size_t getNumerator() const noexcept;
        inline void setNumerator(size_t val) noexcept;
        inline size_t getDenominator() const noexcept;
        inline void setDenominator(size_t val) noexcept;
        inline const BigInteger& getBigInt() const noexcept;
        inline void setBigInt(BigInteger val) noexcept;
        inline void tryConvertToSmallInt() noexcept;
        inline const BigRational& getBigRat() const noexcept;
        inline void canonicaliseBigRat();
        inline FloatingPoint getFloatingPoint() const noexcept;
        inline void setFloatingPoint(FloatingPoint val) noexcept;
        void setFromIntegerString(std::string_view str);
        void setFromIntegerStringOverflowToFloat(std::string_view str) noexcept;
        void setFromDecimalString(std::string_view str, size_t decimal_index);
        void setFromDecimalStringOverflowToFloat(std::string_view str, size_t decimal_index) noexcept;
        void setFromScientificString(std::string_view str, size_t decimal_index, size_t e_index);
        void setFromScientificStringOverflowToFloat(
            std::string_view str, size_t decimal_index, size_t e_index) noexcept;

        void normaliseInPlace();

    private:
        Tag tag;

        union {
            size_t machine_int;
            NativeRational native_rational;
            BigInteger big_int;
            BigRational big_rational;
            FloatingPoint floating_point;
        } data;

        #ifndef NDEBUG
        bool isPosInt() const noexcept;
        bool isNegInt() const noexcept;
        bool isMachineInt() const noexcept;
        bool isPosNativeRat() const noexcept;
        bool isNegNativeRat() const noexcept;
        bool isNativeRat() const noexcept;
        bool isBigInt() const noexcept;
        bool isBigRat() const noexcept;
        bool isFloatingPoint() const noexcept;
        #endif
    } data;
};

}  // namespace KiCAS2

#endif  // #ifndef KI_CAS_NUMBER_H
