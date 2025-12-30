#include <catch2/catch_test_macros.hpp>

#include "../src/ki_cas_number.h"

using namespace KiCAS2;

TEST_CASE( "Move constructor" ) {
    {
        DEBUG_REQUIRE(isAllGmpMemoryFreed());
        Number num = Number::fromIntegerString(std::string("265252859812191058636308480000000"));
        DEBUG_REQUIRE_FALSE(isAllGmpMemoryFreed());

        {
            Number other_num = std::move(num);
        }
        DEBUG_REQUIRE(isAllGmpMemoryFreed());

        {
            Number other_num = Number::fromIntegerString(std::string("265252859812191058636308480000000"));
            num = std::move(other_num);
        }
        DEBUG_REQUIRE_FALSE(isAllGmpMemoryFreed());
    }

    DEBUG_REQUIRE(isAllGmpMemoryFreed());
}

TEST_CASE( "fromIntegerString" ) {
    std::string in;
    std::string out;

    SECTION("Native int"){
        in = "5";
        Number num = Number::fromIntegerString(in);
        num.writeString<false>(out);
        REQUIRE(out == "5");
    }

    SECTION("Big int"){
        in = "265252859812191058636308480000000";
        Number num = Number::fromIntegerString(in);
        num.writeString<false>(out);
        REQUIRE(out == "265252859812191058636308480000000");
    }

    #if !defined(__GNUC__) || __GNUC__ > 8
    SECTION("Float"){
        in = "265252859812191058636308480000000";
        Number num = Number::fromIntegerStringOverflowToFloat(in);
        num.writeString<false>(out);
        REQUIRE(out.substr(0, 4) == "2.65");
        REQUIRE(out.substr(out.size()-4) == "e+32");
    }
    #endif

    DEBUG_REQUIRE(isAllGmpMemoryFreed());
}

TEST_CASE( "fromDecimalString" ) {
    std::string in;
    std::string out;

    SECTION("Native int"){
        in = "5.0";
        Number num = Number::fromDecimalString(in);
        num.writeString<false>(out);
        REQUIRE(out == "5");
    }

    SECTION("Native rat"){
        in = "2.5";
        Number num = Number::fromDecimalString(in);
        num.writeString<false>(out);
        REQUIRE(out == "5/2");
    }

    SECTION("Big rat"){
        in = "265252859812191058636308480000000.5";
        Number num = Number::fromDecimalString(in);
        num.writeString<false>(out);
        REQUIRE(out == "530505719624382117272616960000001/2");
    }

    #if !defined(__GNUC__) || __GNUC__ > 8
    SECTION("Float"){
        in = "265252859812191058636308480000000.5";
        Number num = Number::fromDecimalStringOverflowToFloat(in);
        num.writeString<false>(out);
        REQUIRE(out.substr(0, 4) == "2.65");
        REQUIRE(out.substr(out.size()-4) == "e+32");
    }
    #endif

    DEBUG_REQUIRE(isAllGmpMemoryFreed());
}

TEST_CASE( "fromScientificString" ) {
    std::string in;
    std::string out;

    SECTION("Native int"){
        in = "1.23e2";
        Number num = Number::fromScientificString(in);
        num.normaliseInPlace();
        num.writeString<false>(out);
        REQUIRE(out == "123");
    }

    SECTION("Native rat"){
        in = "0.025e2";
        Number num = Number::fromScientificString(in);
        num.normaliseInPlace();
        num.writeString<false>(out);
        REQUIRE(out == "5/2");
    }

    SECTION("Native rat with trailing zero"){
        in = "25.0e-1";
        Number num = Number::fromScientificString(in);
        num.normaliseInPlace();
        num.writeString<false>(out);
        REQUIRE(out == "5/2");
    }

    SECTION("Big rat with small exponent"){
        in = "265252859812191058636308480000000.5e0";
        Number num = Number::fromScientificString(in);
        num.writeString<false>(out);
        REQUIRE(out == "530505719624382117272616960000001/2");
    }

    SECTION("Big rat with large exponent"){
        in = "1.0e-10";
        Number num = Number::fromScientificString(in);
        num.writeString<false>(out);
        REQUIRE(out == "1/10000000000");
    }

#if !defined(__GNUC__) || __GNUC__ > 8
    SECTION("Float"){
        in = "2.652528e32";
        Number num = Number::fromScientificStringOverflowToFloat(in);
        num.writeString<false>(out);
        REQUIRE(out.substr(0, 4) == "2.65");
        REQUIRE(out.substr(out.size()-4) == "e+32");
    }
#endif

    DEBUG_REQUIRE(isAllGmpMemoryFreed());
}

TEST_CASE( "fromSciIntString" ) {
    std::string in;
    std::string out;

    SECTION("Native int"){
        in = "3e2";
        Number num = Number::fromSciIntString(in);
        num.writeString<false>(out);
        REQUIRE(out == "300");
    }

    SECTION("Native rat"){
        in = "3e-2";
        Number num = Number::fromSciIntString(in);
        num.writeString<false>(out);
        REQUIRE(out == "3/100");
    }

    SECTION("Big int"){
        in = "3e25";
        Number num = Number::fromSciIntString(in);
        num.writeString<false>(out);
        REQUIRE(out == "30000000000000000000000000");
    }

    SECTION("Big rat"){
        in = "3e-25";
        Number num = Number::fromSciIntString(in);
        num.writeString<false>(out);
        REQUIRE(out == "3/10000000000000000000000000");
    }

#if !defined(__GNUC__) || __GNUC__ > 8
    SECTION("Float Positive"){
        in = "3e25";
        Number num = Number::fromSciIntStringOverflowToFloat(in);
        num.writeString<false>(out);
        REQUIRE(out == "3e+25");
    }

    SECTION("Float Negative"){
        in = "3e-25";
        Number num = Number::fromSciIntStringOverflowToFloat(in);
        num.writeString<false>(out);
        REQUIRE(out == "3e-25");
    }
#endif

    DEBUG_REQUIRE(isAllGmpMemoryFreed());
}
