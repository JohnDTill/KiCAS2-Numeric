#include <catch2/catch_test_macros.hpp>

#include "../src/ki_cas_str_conversions.h"

using namespace KiCAS2;

static constexpr size_t MAX = std::numeric_limits<size_t>::max();

TEST_CASE( "write_int" ) {
    std::string str;

    str.clear();
    write_int(str, 0uLL);
    REQUIRE(str == "0");

    str.clear();
    write_int(str, 42uLL);
    REQUIRE(str == "42");

    str.clear();
    write_int(str, 1234uLL);
    REQUIRE(str == "1234");

    str.clear();
    write_int(str, MAX);
    REQUIRE(str == std::to_string(MAX));
}

TEST_CASE( "ckd_str2int" ) {
    size_t result;

    REQUIRE_FALSE(ckd_str2int(&result, "0"));
    REQUIRE(result == 0);

    REQUIRE_FALSE(ckd_str2int(&result, "42"));
    REQUIRE(result == 42);

    REQUIRE_FALSE(ckd_str2int(&result, "4321"));
    REQUIRE(result == 4321);

    REQUIRE_FALSE(ckd_str2int(&result, std::to_string(MAX)));
    REQUIRE(result == MAX);

    std::string too_large_int = std::to_string(MAX);
    too_large_int.back()++;
    assert(too_large_int.back() >= '0' && too_large_int.back() <= '9');
    REQUIRE(true == ckd_str2int(&result, too_large_int));
}

TEST_CASE( "fmpz_set_str_NULL_TERMINATED_SOURCE__NOT_THREADSAFE" ) {
    fmpz_t big_int;
    fmpz_init(big_int);
    fmpz_set_str_NULL_TERMINATED_SOURCE__NOT_THREADSAFE(big_int, std::string("0"));
    REQUIRE(fmpz_get_si(big_int) == 0);

    fmpz_set_str_NULL_TERMINATED_SOURCE__NOT_THREADSAFE(big_int, std::string("42"));
    REQUIRE(fmpz_get_si(big_int) == 42);

    fmpz_t factorial_of_30;
    fmpz_init(factorial_of_30);
    fmpz_fac_ui(factorial_of_30, 30);
    fmpz_set_str_NULL_TERMINATED_SOURCE__NOT_THREADSAFE(big_int, std::string("265252859812191058636308480000000"));
    REQUIRE(fmpz_cmp(big_int, factorial_of_30) == 0);

    fmpz_clear(big_int);
    fmpz_clear(factorial_of_30);
}

TEST_CASE( "write_rational" ) {
    std::string str = "x + ";
    NativeRational num(3,2);

    SECTION("plaintext"){
        write_rational<PLAINTEXT_OUTPUT>(str, num);
        REQUIRE(str == "x + 3/2");
    }

    SECTION("typeset"){
        write_rational<TYPESET_OUTPUT>(str, num);
        REQUIRE(str == "x + ⁜f⏴3⏵⏴2⏵");
    }
}

#include <iostream>  // TODO: delete

TEST_CASE( "ckd_strdecimal2rat" ) {
    NativeRational result;

    // SECTION("No factors"){
    //     REQUIRE_FALSE(ckd_strdecimal2rat(&result, "1.3"));
    //     REQUIRE(result.num == 13);
    //     REQUIRE(result.den == 10);

    //     REQUIRE_FALSE(ckd_strdecimal2rat(&result, "127.589"));
    //     REQUIRE(result.num == 127589);
    //     REQUIRE(result.den == 1000);
    // }

    // SECTION("Factor of 2"){
    //     REQUIRE_FALSE(ckd_strdecimal2rat(&result, "2.2"));
    //     REQUIRE(result.num == 11);
    //     REQUIRE(result.den == 5);
    // }

    // SECTION("Factor of 5"){
    //     REQUIRE_FALSE(ckd_strdecimal2rat(&result, "2.5"));
    //     REQUIRE(result.num == 5);
    //     REQUIRE(result.den == 2);
    // }

    // SECTION("Multiple factors of 5"){
    //     REQUIRE_FALSE(ckd_strdecimal2rat(&result, "1.125"));
    //     REQUIRE(result.num == 9);
    //     REQUIRE(result.den == 8);
    // }

    // SECTION("Trailing zeros"){
    //     REQUIRE_FALSE(ckd_strdecimal2rat(&result, "1.30"));
    //     REQUIRE(result.num == 13);
    //     REQUIRE(result.den == 10);
    // }

    // SECTION("All trailing zeros"){
    //     REQUIRE_FALSE(ckd_strdecimal2rat(&result, "3.00"));
    //     REQUIRE(result.num == 3);
    //     REQUIRE(result.den == 1);
    // }

    SECTION("Longest fitting decimal"){
        // Test the longest 0.9999999... which fits
        const std::string den_str = "1" + std::string(std::numeric_limits<size_t>::digits10-1, '0');
        const size_t expected_den = std::stoull(den_str);
        const size_t test_num = expected_den - 1;
        const std::string test_str = "0." + std::to_string(test_num);

        REQUIRE_FALSE(ckd_strdecimal2rat(&result, test_str));
        REQUIRE(result.num == test_num);
        REQUIRE(result.den == expected_den);  // TODO: why is this 0 on some targets?
    }

    // SECTION("Overflow"){
    //     std::string test_str;

    //     // Test the shortest 0.9999999... which does not fit
    //     test_str = "0." + std::string(std::numeric_limits<size_t>::digits10, '9');
    //     REQUIRE(true == ckd_strdecimal2rat(&result, test_str));

    //     test_str = "0." + std::string(std::numeric_limits<size_t>::digits10-1, '9');
    //     REQUIRE(false == ckd_strdecimal2rat(&result, test_str));

    //     // // Test a combination of leading and trailing values which does not fit
    //     test_str = "999." + std::string(std::numeric_limits<size_t>::digits10-1, '9');
    //     REQUIRE(true == ckd_strdecimal2rat(&result, test_str));  // TODO: why does this fail on some targets?

    //     test_str = "0." + std::string(std::numeric_limits<size_t>::digits10-1, '9');
    //     REQUIRE(false == ckd_strdecimal2rat(&result, test_str));

    //     // Test a leading value which does not fit
    //     REQUIRE(true == ckd_strdecimal2rat(&result, "265252859812191058636308480000000.1"));
    // }
}
