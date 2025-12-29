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

TEST_CASE( "strdecimal2floatingpoint" ){
    REQUIRE( std::abs(strdecimal2floatingpoint("4.2") - 4.2) < 1e-9 );
    REQUIRE( std::abs(strdecimal2floatingpoint("1234") - 1234.0) < 1e-9 );
}

TEST_CASE( "strscientific2floatingpoint" ){
    REQUIRE( std::abs(strscientific2floatingpoint("0.42e2")) - 42.0 < 1e-9 );
    REQUIRE( std::abs(strscientific2floatingpoint("12.34e-2")) - 0.1234 < 1e-9 );
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
    BigInteger big_int;
    std::string input;

    input = "0";
    str2bigint_NULL_TERMINATED__NOT_THREADSAFE(big_int, input);
    REQUIRE(mpz_get_si(big_int) == 0);
    REQUIRE(input == "0");
    mpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    input = "42";
    str2bigint_NULL_TERMINATED__NOT_THREADSAFE(big_int, input);
    REQUIRE(mpz_get_si(big_int) == 42);
    REQUIRE(input == "42");
    mpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    input = "265252859812191058636308480000000";
    mpz_t factorial_of_30;
    mpz_init(factorial_of_30);
    mpz_fac_ui(factorial_of_30, 30);
    str2bigint_NULL_TERMINATED__NOT_THREADSAFE(big_int, input);
    REQUIRE(mpz_cmp(big_int, factorial_of_30) == 0);
    REQUIRE(input == "265252859812191058636308480000000");

    mpz_clear(big_int);
    mpz_clear(factorial_of_30);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());
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

    SECTION("No factors"){
        REQUIRE_FALSE(ckd_strdecimal2rat(&result, "1.3"));
        REQUIRE(result.num == 13);
        REQUIRE(result.den == 10);

        REQUIRE_FALSE(ckd_strdecimal2rat(&result, "127.589"));
        REQUIRE(result.num == 127589);
        REQUIRE(result.den == 1000);
    }

    SECTION("Factor of 2"){
        REQUIRE_FALSE(ckd_strdecimal2rat(&result, "2.2"));
        REQUIRE(result.num == 11);
        REQUIRE(result.den == 5);
    }

    SECTION("Factor of 5"){
        REQUIRE_FALSE(ckd_strdecimal2rat(&result, "2.5"));
        REQUIRE(result.num == 5);
        REQUIRE(result.den == 2);
    }

    SECTION("Multiple factors of 5"){
        REQUIRE_FALSE(ckd_strdecimal2rat(&result, "1.125"));
        REQUIRE(result.num == 9);
        REQUIRE(result.den == 8);
    }

    SECTION("Trailing zeros"){
        REQUIRE_FALSE(ckd_strdecimal2rat(&result, "1.30"));
        REQUIRE(result.num == 13);
        REQUIRE(result.den == 10);
    }

    SECTION("All trailing zeros"){
        REQUIRE_FALSE(ckd_strdecimal2rat(&result, "3.00"));
        REQUIRE(result.num == 3);
        REQUIRE(result.den == 1);
    }

    SECTION("Longest fitting decimal"){
        // Test the longest 0.9999999... which fits
        const std::string den_str = "1" + std::string(std::numeric_limits<size_t>::digits10-1, '0');
        const size_t expected_den = std::stoull(den_str);
        const size_t test_num = expected_den - 1;
        const std::string test_str = "0." + std::to_string(test_num);

        REQUIRE_FALSE(ckd_strdecimal2rat(&result, test_str));
        REQUIRE(result.num == test_num);
        REQUIRE(result.den == expected_den);
    }

    SECTION("Overflow"){
        std::string test_str;

        // Test the shortest 0.9999999... which does not fit
        test_str = "0." + std::string(std::numeric_limits<size_t>::digits10, '9');
        REQUIRE(true == ckd_strdecimal2rat(&result, test_str));

        test_str = "0." + std::string(std::numeric_limits<size_t>::digits10-1, '9');
        REQUIRE(false == ckd_strdecimal2rat(&result, test_str));

        // Test a combination of leading and trailing values which does not fit
        test_str = "999." + std::string(std::numeric_limits<size_t>::digits10-1, '9');
        REQUIRE(true == ckd_strdecimal2rat(&result, test_str));

        test_str = "0." + std::string(std::numeric_limits<size_t>::digits10-1, '9');
        REQUIRE(false == ckd_strdecimal2rat(&result, test_str));

        // Test a leading value which does not fit
        REQUIRE(true == ckd_strdecimal2rat(&result, "265252859812191058636308480000000.1"));
    }
}

TEST_CASE( "ckd_strscientific2rat" ) {
    NativeRational result;

    REQUIRE_FALSE(ckd_strscientific2rat(&result, "1.0e-2"));
    REQUIRE(result.num == 1);
    REQUIRE(result.den == 100);

    REQUIRE_FALSE(ckd_strscientific2rat(&result, "0.1e-2"));
    REQUIRE(result.num == 1);
    REQUIRE(result.den == 1000);

    REQUIRE_FALSE(ckd_strscientific2rat(&result, "1.01e2"));
    REQUIRE(result.num == 10100);
    REQUIRE(result.den == 100);

    REQUIRE(true == ckd_strscientific2rat(&result, "1.0e20"));

    REQUIRE(true == ckd_strscientific2rat(&result, "1.0e-20"));
}

TEST_CASE( "strdecimal2bigrat_NULL_TERMINATED__NOT_THREADSAFE" ) {
    BigRational big_rat;
    std::string input;

    SECTION("Small decimal"){
        input = "2.5";
        strdecimal2bigrat_NULL_TERMINATED__NOT_THREADSAFE(big_rat, input);
        REQUIRE(fmpz_get_ui(fmpq_numref(big_rat)) == 5);
        REQUIRE(fmpz_get_ui(fmpq_denref(big_rat)) == 2);
        REQUIRE(input == "2.5");
        fmpq_clear(big_rat);
        DEBUG_REQUIRE(isAllGmpMemoryFreed());
    }

    SECTION("Large integer with decimal zero"){
        input = "265252859812191058636308480000000.0";
        fmpz_t factorial_of_30;
        fmpz_init(factorial_of_30);
        fmpz_fac_ui(factorial_of_30, 30);
        strdecimal2bigrat_NULL_TERMINATED__NOT_THREADSAFE(big_rat, input);
        REQUIRE(fmpz_cmp(fmpq_numref(big_rat), factorial_of_30) == 0);
        REQUIRE(fmpz_get_ui(fmpq_denref(big_rat)) == 1);
        REQUIRE(input == "265252859812191058636308480000000.0");

        fmpq_clear(big_rat);
        fmpz_clear(factorial_of_30);
        DEBUG_REQUIRE(isAllGmpMemoryFreed());
    }

    SECTION("Big rat"){
        input = "265252859812191058636308480000000.5";
        strdecimal2bigrat_NULL_TERMINATED__NOT_THREADSAFE(big_rat, input);
        std::string out;
        write_big_rational<false>(out, big_rat);
        REQUIRE(out == "530505719624382117272616960000001/2");
        REQUIRE(input == "265252859812191058636308480000000.5");
        fmpq_clear(big_rat);
        DEBUG_REQUIRE(isAllGmpMemoryFreed());
    }
}

TEST_CASE( "write_big_int" ) {
    std::string str = "x + ";
    mpz_t big_num;

    SECTION("Basic"){
        mpz_init_set_ui(big_num, 42);
        write_big_int(str, big_num);
        REQUIRE(str == "x + 42");
    }

    SECTION("Negative"){
        mpz_init_set_si(big_num, -42);
        write_big_int(str, big_num);
        REQUIRE(str == "x + -42");
    }

    SECTION("Big number"){
        mpz_init(big_num);
        mpz_fac_ui(big_num, 30);
        write_big_int(str, big_num);
        REQUIRE(str == "x + 265252859812191058636308480000000");
    }

    SECTION("Big number negative"){
        mpz_init(big_num);
        mpz_fac_ui(big_num, 30);
        big_num->_mp_size *= -1;
        write_big_int(str, big_num);
        REQUIRE(str == "x + -265252859812191058636308480000000");
    }

    mpz_clear(big_num);

    DEBUG_REQUIRE(isAllGmpMemoryFreed());
}

TEST_CASE( "write_big_rational" ) {
    std::string str = "x + ";
    fmpq_t big_num;
    fmpq_init(big_num);
    fmpz* num = fmpq_numref(big_num);
    fmpz* den = fmpq_denref(big_num);

    SECTION("plaintext basic"){
        fmpq_set_ui(big_num, 3, 2);
        write_big_rational<PLAINTEXT_OUTPUT>(str, big_num);
        REQUIRE(str == "x + 3/2");
    }

    SECTION("typeset basic"){
        fmpq_set_ui(big_num, 3, 2);
        write_big_rational<TYPESET_OUTPUT>(str, big_num);
        REQUIRE(str == "x + ⁜f⏴3⏵⏴2⏵");
    }

    SECTION("plaintext basic negative"){
        fmpq_set_si(big_num, -3, 2);
        write_big_rational<PLAINTEXT_OUTPUT>(str, big_num);
        REQUIRE(str == "x + -3/2");
    }

    SECTION("typeset basic negative"){
        fmpq_set_si(big_num, -3, 2);
        write_big_rational<TYPESET_OUTPUT>(str, big_num);
        REQUIRE(str == "x + -⁜f⏴3⏵⏴2⏵");
    }

    SECTION("plaintext big"){
        fmpz_set_ui(num, 1);
        fmpz_fac_ui(den, 30);
        write_big_rational<PLAINTEXT_OUTPUT>(str, big_num);
        REQUIRE(str == "x + 1/265252859812191058636308480000000");
    }

    SECTION("typeset big"){
        fmpz_set_ui(num, 1);
        fmpz_fac_ui(den, 30);
        write_big_rational<TYPESET_OUTPUT>(str, big_num);
        REQUIRE(str == "x + ⁜f⏴1⏵⏴265252859812191058636308480000000⏵");
    }

    SECTION("plaintext big negative"){
        fmpz_set_si(num, -1);
        fmpz_fac_ui(den, 30);
        write_big_rational<PLAINTEXT_OUTPUT>(str, big_num);
        REQUIRE(str == "x + -1/265252859812191058636308480000000");
    }

    SECTION("typeset big negative"){
        fmpz_set_si(num, -1);
        fmpz_fac_ui(den, 30);
        write_big_rational<TYPESET_OUTPUT>(str, big_num);
        REQUIRE(str == "x + -⁜f⏴1⏵⏴265252859812191058636308480000000⏵");
    }

    fmpq_clear(big_num);

    DEBUG_REQUIRE(isAllGmpMemoryFreed());
}
