#include <catch2/catch_test_macros.hpp>

#include "../src/ki_cas_str_conversions.h"

using namespace KiCAS2;

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
    write_int(str, std::numeric_limits<size_t>::max());
    REQUIRE(str == std::to_string(std::numeric_limits<size_t>::max()));
}

TEST_CASE( "ckd_str2int" ) {
    size_t result;

    REQUIRE_FALSE(ckd_str2int(&result, "0"));
    REQUIRE(result == 0);

    REQUIRE_FALSE(ckd_str2int(&result, "42"));
    REQUIRE(result == 42);

    REQUIRE_FALSE(ckd_str2int(&result, "4321"));
    REQUIRE(result == 4321);

    REQUIRE_FALSE(ckd_str2int(&result, std::to_string(std::numeric_limits<size_t>::max())));
    REQUIRE(result == std::numeric_limits<size_t>::max());

    std::string too_large_int = std::to_string(std::numeric_limits<size_t>::max());
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

TEST_CASE( "ckd_strdecimal2rat" ) {
    NativeRational result;
    ckd_strdecimal2rat(&result, "2", "5");
    REQUIRE(result.num == 25);
    REQUIRE(result.den == 10);

    ckd_strdecimal2rat(&result, "127", "589");
    REQUIRE(result.num == 127589);
    REQUIRE(result.den == 1000);

    REQUIRE(true == ckd_strdecimal2rat(&result, "265252859812191058636308480000000", "1"));
}
