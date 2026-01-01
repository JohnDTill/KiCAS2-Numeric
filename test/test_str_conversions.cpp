#include <catch2/catch_test_macros.hpp>

#include "../src/ki_cas_str_conversions.h"

using namespace KiCAS2;

static constexpr size_t MAX = std::numeric_limits<size_t>::max();

#ifndef NDEBUG
namespace KiCAS2 { extern bool spoof_bignum_path; }
#endif

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

TEST_CASE( "strscientific2bigrat_NULL_TERMINATED__NOT_THREADSAFE" ) {
    BigRational big_rat;
    std::string input;

    SECTION("Small decimal"){
        input = "2.5e-1";
        strscientific2bigrat_NULL_TERMINATED__NOT_THREADSAFE(big_rat, input);
        fmpq_canonicalise(big_rat);
        REQUIRE(fmpz_get_ui(fmpq_numref(big_rat)) == 1);
        REQUIRE(fmpz_get_ui(fmpq_denref(big_rat)) == 4);
        REQUIRE(input == "2.5e-1");
        fmpq_clear(big_rat);
        DEBUG_REQUIRE(isAllGmpMemoryFreed());
    }

    SECTION("Large exponent"){
        input = "0.3e-24";
        strscientific2bigrat_NULL_TERMINATED__NOT_THREADSAFE(big_rat, input);
        REQUIRE(std::string(fmpq_get_str(nullptr, 10, big_rat)) == "3/10000000000000000000000000");
        REQUIRE(input == "0.3e-24");
        fmpq_clear(big_rat);
        DEBUG_REQUIRE(isAllGmpMemoryFreed());
    }

    #ifndef NDEBUG
    SECTION("Large exponent big exponent path"){
        spoof_bignum_path = true;
        input = "0.3e-24";
        strscientific2bigrat_NULL_TERMINATED__NOT_THREADSAFE(big_rat, input);
        REQUIRE(std::string(fmpq_get_str(nullptr, 10, big_rat)) == "3/10000000000000000000000000");
        REQUIRE(input == "0.3e-24");
        fmpq_clear(big_rat);
        DEBUG_REQUIRE(isAllGmpMemoryFreed());
        DEBUG_REQUIRE_FALSE(spoof_bignum_path);  // Confirm bignum path was taken
    }
    #endif
}

TEST_CASE( "strsciint2bigint_NULL_TERMINATED__NOT_THREADSAFE" ) {
    BigInteger big_int;
    std::string input;

    SECTION("Small number"){
        input = "1e2";
        strsciint2bigint_NULL_TERMINATED__NOT_THREADSAFE(big_int, input);
        REQUIRE(mpz_get_ui(big_int) == 100);
        REQUIRE(input == "1e2");
        mpz_clear(big_int);
        DEBUG_REQUIRE(isAllGmpMemoryFreed());
    }

    SECTION("Big number"){
        input = "1e25";
        strsciint2bigint_NULL_TERMINATED__NOT_THREADSAFE(big_int, input);
        fmpz alias = PTR_TO_COEFF(big_int);
        REQUIRE(std::string(fmpz_get_str(nullptr, 10, &alias)) == "10000000000000000000000000");
        REQUIRE(input == "1e25");
        mpz_clear(big_int);
        DEBUG_REQUIRE(isAllGmpMemoryFreed());
    }

    #ifndef NDEBUG
    SECTION("Big number big exponent path"){
        spoof_bignum_path = true;
        input = "1e25";
        strsciint2bigint_NULL_TERMINATED__NOT_THREADSAFE(big_int, input);
        fmpz alias = PTR_TO_COEFF(big_int);
        REQUIRE(std::string(fmpz_get_str(nullptr, 10, &alias)) == "10000000000000000000000000");
        REQUIRE(input == "1e25");
        mpz_clear(big_int);
        DEBUG_REQUIRE(isAllGmpMemoryFreed());
        DEBUG_REQUIRE_FALSE(spoof_bignum_path);  // Confirm bignum path was taken
    }
    #endif
}

TEST_CASE( "strsciint2bigrat_NULL_TERMINATED__NOT_THREADSAFE" ) {
    BigRational big_rat;
    std::string input;

    SECTION("Small number"){
        input = "2e-1";
        strsciint2bigrat_NULL_TERMINATED__NOT_THREADSAFE(big_rat, input);
        fmpq_canonicalise(big_rat);
        REQUIRE(fmpz_get_ui(fmpq_numref(big_rat)) == 1);
        REQUIRE(fmpz_get_ui(fmpq_denref(big_rat)) == 5);
        REQUIRE(input == "2e-1");
        fmpq_clear(big_rat);
        DEBUG_REQUIRE(isAllGmpMemoryFreed());
    }

    SECTION("Big number"){
        input = "1e-25";
        strsciint2bigrat_NULL_TERMINATED__NOT_THREADSAFE(big_rat, input);
        REQUIRE(std::string(fmpq_get_str(nullptr, 10, big_rat)) == "1/10000000000000000000000000");
        REQUIRE(input == "1e-25");
        fmpq_clear(big_rat);
        DEBUG_REQUIRE(isAllGmpMemoryFreed());
    }

    #ifndef NDEBUG
    SECTION("Big number big exponent path"){
        spoof_bignum_path = true;
        input = "1e-25";
        strsciint2bigrat_NULL_TERMINATED__NOT_THREADSAFE(big_rat, input);
        REQUIRE(std::string(fmpq_get_str(nullptr, 10, big_rat)) == "1/10000000000000000000000000");
        REQUIRE(input == "1e-25");
        fmpq_clear(big_rat);
        DEBUG_REQUIRE(isAllGmpMemoryFreed());
        DEBUG_REQUIRE_FALSE(spoof_bignum_path);  // Confirm bignum path was taken
    }
    #endif
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
