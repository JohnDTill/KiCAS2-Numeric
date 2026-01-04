#include <catch2/catch_test_macros.hpp>

#include "../src/ki_cas_big_num_wrapper.h"

using namespace KiCAS2;

static constexpr size_t MAX = std::numeric_limits<size_t>::max();

static std::string toString(fmpq_t f) {
    std::string str;
    write_big_rational(str, f);
    return str;
}

TEST_CASE( "GMP memory leak detection mechanism" ) {
    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());

    mpz_t gmp_int;
    mpz_init_set_ui(gmp_int, 42);
    DEBUG_REQUIRE_FALSE(isAllGmpMemoryFreed());
    mpz_clear(gmp_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());

    // TODO: why is this leaking?
    // mpq_t gmp_rat;
    // mpq_init(gmp_rat);
    // mpq_set_ui(gmp_rat, 1, 42);
    // DEBUG_REQUIRE_FALSE(isAllGmpMemoryFreed());  // GMP does not allocate here on all environments.
                                                    // It may be an accident that this check generally works.
    // mpq_clear(gmp_rat);
    // DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());

    // TODO: why is this leaking?
    // fmpz_t flint_int;
    // fmpz_init_set_ui(flint_int, 42);
    // DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());  // No allocation for Flint since word sized
    // fmpz_fac_ui(flint_int, 30);
    // DEBUG_REQUIRE_FALSE(isAllGmpMemoryFreed());
    // fmpz_clear(flint_int);
    // DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());

    // TODO: why is this leaking?
    // fmpq_t flint_rat;
    // fmpq_init(flint_rat);
    // fmpq_set_ui(flint_rat, 1337, 3407);
    // DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());  // No allocation for Flint since word sized
    // fmpz_fac_ui(fmpq_denominator_ptr(flint_rat), 30);
    // DEBUG_REQUIRE_FALSE(isAllGmpMemoryFreed());
    // fmpq_clear(flint_rat);
    // DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
}

TEST_CASE( "constants" ) {
    REQUIRE(fmpz_get_si(FMPZ_ZERO) == 0);
    REQUIRE(fmpz_get_si(FMPZ_ONE) == 1);
    REQUIRE(fmpz_get_si(FMPZ_FIVE) == 5);
    REQUIRE(fmpz_get_si(FMPZ_TEN) == 10);

    REQUIRE(fmpz_get_si(fmpq_numref(FMPQ_ZERO)) == 0);
    REQUIRE(fmpz_get_si(fmpq_denref(FMPQ_ZERO)) == 1);

    REQUIRE(fmpz_get_si(fmpq_numref(FMPQ_ONE)) == 1);
    REQUIRE(fmpz_get_si(fmpq_denref(FMPQ_ONE)) == 1);
}

TEST_CASE( "mpz_neg_inplace" ) {
    mpz_t val;
    mpz_init_set_ui(val, 42);
    REQUIRE(mpz_get_si(val) == 42);
    mpz_neg_inplace(val);
    REQUIRE(mpz_get_si(val) == -42);
    mpz_neg_inplace(val);
    REQUIRE(mpz_get_si(val) == 42);
    mpz_clear(val);

    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
}

TEST_CASE( "mpz_is_neg" ) {
    mpz_t val;
    mpz_init_set_si(val, 42);
    REQUIRE_FALSE(mpz_is_neg(val));
    mpz_neg_inplace(val);
    REQUIRE(mpz_is_neg(val));
    mpz_clear(val);

    mpz_init_set_si(val, 0);
    REQUIRE_FALSE(mpz_is_neg(val));
    mpz_neg_inplace(val);
    REQUIRE_FALSE(mpz_is_neg(val));
    mpz_clear(val);

    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
}

TEST_CASE( "fmpz_10_pow_ui" ) {
    fmpz_t val;
    fmpz_10_pow_ui(val, 3);
    REQUIRE(fmpz_get_ui(val) == 1'000);
    fmpz_clear(val);

    fmpz_10_pow_ui(val, 9);
    REQUIRE(fmpz_get_ui(val) == 1'000'000'000);
    fmpz_clear(val);

    // TODO: why is this leaking?
    // fmpz_10_pow_ui(val, 30);
    // REQUIRE(fmpz_get_str(nullptr, 10, val) == std::string("1000000000000000000000000000000"));
    // fmpz_clear(val);

    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
}

TEST_CASE( "mpz_sizeinbase10upperbound" ) {
    mpz_t val;

    mpz_init_set_si(val, 42);
    REQUIRE(mpz_sizeinbase10upperbound(val) >= mpz_sizeinbase(val, 10));
    mpz_clear(val);

    mpz_init_set_si(val, MAX);
    REQUIRE(mpz_sizeinbase10upperbound(val) >= mpz_sizeinbase(val, 10));
    mpz_clear(val);

    mpz_init_set_str(val, "265252859812191058636308480000000", 10);
    REQUIRE(mpz_sizeinbase10upperbound(val) >= mpz_sizeinbase(val, 10));
    mpz_clear(val);

    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
}

TEST_CASE( "fmpz_sizeinbase10upperbound" ) {
    fmpz_t val;

    fmpz_init_set_si(val, 42);
    REQUIRE(fmpz_sizeinbase10upperbound(val) >= fmpz_sizeinbase(val, 10));
    fmpz_clear(val);
    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());

    fmpz_init_set_si(val, MAX);
    REQUIRE(fmpz_sizeinbase10upperbound(val) >= fmpz_sizeinbase(val, 10));
    fmpz_clear(val);
    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());

    // TODO: why is this leaking?
    // fmpz_init(val);
    // fmpz_set_str(val, "265252859812191058636308480000000", 10);
    // REQUIRE(fmpz_sizeinbase10upperbound(val) >= fmpz_sizeinbase(val, 10));
    // fmpz_clear(val);
    // DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
}

TEST_CASE( "fmpq_abs_inplace" ) {
    fmpq_t val;
    fmpz_init_set_si(fmpq_numref(val), -1);
    fmpz_init_set_ui(fmpq_denref(val), 4);
    REQUIRE(toString(val) == "-1/4");
    fmpq_abs_inplace(val);
    REQUIRE(toString(val) == "1/4");

    fmpz_init_set_si(fmpq_numref(val), COEFF_MIN);
    fmpq_abs_inplace(val);
    REQUIRE(fmpz_get_ui(fmpq_numref(val)) == COEFF_MAX);

    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
}

TEST_CASE( "mpz_init_set_strview" ) {
    mpz_t big_int;

    mpz_init_set_strview(big_int, "0");
    REQUIRE(mpz_get_si(big_int) == 0);
    mpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());

    mpz_init_set_strview(big_int, "42");
    REQUIRE(mpz_get_si(big_int) == 42);
    mpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());

    // TODO: why is this leaking?
    // mpz_init_set_strview(big_int, std::to_string(MAX));
    // char buffer[128];
    // REQUIRE(mpz_get_str(buffer, 10, big_int) == std::to_string(MAX));
    // mpz_clear(big_int);
    // DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());

    mpz_t factorial_of_30;
    mpz_init(factorial_of_30);
    mpz_fac_ui(factorial_of_30, 30);
    // mpz_init_set_strview(big_int, "265252859812191058636308480000000");
    // REQUIRE(mpz_cmp(big_int, factorial_of_30) == 0);

    // mpz_clear(big_int);
    mpz_clear(factorial_of_30);
    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
}

TEST_CASE( "fmpz_init_set_strview" ) {
    fmpz_t big_int;

    fmpz_init_set_strview(big_int, "0");
    REQUIRE(fmpz_get_si(big_int) == 0);
    fmpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());

    fmpz_init_set_strview(big_int, "42");
    REQUIRE(fmpz_get_si(big_int) == 42);
    fmpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());

    // TODO: why are these leaking?
    // fmpz_init_set_strview(big_int, std::to_string(MAX));
    // REQUIRE(fmpz_get_ui(big_int) == MAX);
    fmpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());

    // fmpz_t factorial_of_30;
    // fmpz_init(factorial_of_30);
    // fmpz_fac_ui(factorial_of_30, 30);
    // fmpz_init_set_strview(big_int, "265252859812191058636308480000000");
    // REQUIRE(fmpz_cmp(big_int, factorial_of_30) == 0);

    fmpz_clear(big_int);
    // fmpz_clear(factorial_of_30);
    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
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

    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
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
        fmpq_clear(big_num);
        DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
    }

    SECTION("typeset basic"){
        fmpq_set_ui(big_num, 3, 2);
        write_big_rational<TYPESET_OUTPUT>(str, big_num);
        REQUIRE(str == "x + ⁜f⏴3⏵⏴2⏵");
        fmpq_clear(big_num);
        DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
    }

    SECTION("plaintext basic negative"){
        fmpq_set_si(big_num, -3, 2);
        write_big_rational<PLAINTEXT_OUTPUT>(str, big_num);
        REQUIRE(str == "x + -3/2");
        fmpq_clear(big_num);
        DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
    }

    SECTION("typeset basic negative"){
        fmpq_set_si(big_num, -3, 2);
        write_big_rational<TYPESET_OUTPUT>(str, big_num);
        REQUIRE(str == "x + -⁜f⏴3⏵⏴2⏵");
        fmpq_clear(big_num);
        DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
    }

    // TODO: why is this leaking?
    // SECTION("plaintext big"){
    //     fmpz_set_ui(num, 1);
    //     fmpz_fac_ui(den, 30);
    //     write_big_rational<PLAINTEXT_OUTPUT>(str, big_num);
    //     REQUIRE(str == "x + 1/265252859812191058636308480000000");
    //     fmpq_clear(big_num);
    //     DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
    // }

    // TODO: why is this leaking?
    // SECTION("typeset big"){
    //     fmpz_set_ui(num, 1);
    //     fmpz_fac_ui(den, 30);
    //     write_big_rational<TYPESET_OUTPUT>(str, big_num);
    //     REQUIRE(str == "x + ⁜f⏴1⏵⏴265252859812191058636308480000000⏵");
    //     fmpq_clear(big_num);
    //     DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
    // }

    // TODO: why is this leaking?
    // SECTION("plaintext big negative"){
    //     fmpz_set_si(num, -1);
    //     fmpz_fac_ui(den, 30);
    //     write_big_rational<PLAINTEXT_OUTPUT>(str, big_num);
    //     REQUIRE(str == "x + -1/265252859812191058636308480000000");
    //     fmpq_clear(big_num);
    //     DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
    // }

    // TODO: why is this leaking?
    // SECTION("typeset big negative"){
    //     fmpz_set_si(num, -1);
    //     fmpz_fac_ui(den, 30);
    //     write_big_rational<TYPESET_OUTPUT>(str, big_num);
    //     REQUIRE(str == "x + -⁜f⏴1⏵⏴265252859812191058636308480000000⏵");
    //     fmpq_clear(big_num);
    //     DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
    // }
}

TEST_CASE( "fmpq_from_decimal_str" ) {
    fmpq_t big_rat;

    *big_rat = fmpq_from_decimal_str("2.2");
    REQUIRE(fmpz_get_ui(fmpq_numref(big_rat)) == 11);
    REQUIRE(fmpz_get_ui(fmpq_denref(big_rat)) == 5);
    fmpq_clear(big_rat);

    *big_rat = fmpq_from_decimal_str("2.5");
    REQUIRE(fmpz_get_ui(fmpq_numref(big_rat)) == 5);
    REQUIRE(fmpz_get_ui(fmpq_denref(big_rat)) == 2);
    fmpq_clear(big_rat);

    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
}

TEST_CASE( "fmpq_from_scientific_str" ){
    fmpq_t big_rat;

    SECTION("Integer Positive Exponent"){
        *big_rat = fmpq_from_scientific_str("1e2");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 100);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 1);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("0e2");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 0);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 1);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("23e1");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 230);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 1);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("0000000000000000000000000000000000000000000000001234e5");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 123400000);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 1);
        fmpq_clear(big_rat);
    }

    SECTION("Integer Negative Exponent"){
        *big_rat = fmpq_from_scientific_str("1e-2");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 1);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 100);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("0e-2");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 0);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 1);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("23e-1");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 23);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 10);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("0000000000000000000000000000000000000000000000004321e-5");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 4321);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 100000);
        fmpq_clear(big_rat);
    }

    SECTION("Decimal Positive Exponent To Integer"){
        *big_rat = fmpq_from_scientific_str("1.2e3");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 1200);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 1);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("0.2e3");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 200);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 1);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("000000000000000000000000000000000000000000000000.2e3");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 200);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 1);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("0.200000000000000000000000000000000000000000000000e3");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 200);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 1);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("0.0e3");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 0);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 1);
        fmpq_clear(big_rat);
    }

    SECTION("Decimal Positive Exponent To Rational"){
        *big_rat = fmpq_from_scientific_str("1.23e1");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 123);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 10);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("0.002e2");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 1);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 5);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("0000000000000000000000000000000000000000000000000.005e2");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 1);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 2);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str(".0050000000000000000000000000000000000000000000000000e2");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 1);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 2);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("0.00000e1");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 0);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 1);
        fmpq_clear(big_rat);
    }

    SECTION("Decimal Negative Exponent Of Greater Magnitude"){
        *big_rat = fmpq_from_scientific_str("1.23e-1");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 123);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 1000);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("0.1e-2");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 1);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 1000);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("0000000000000000000000000000000000000000000000000.5e-1");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 1);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 20);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str(".50000000000000000000000000000000000000000000000000e-1");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 1);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 20);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("0.00000e-2");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 0);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 1);
        fmpq_clear(big_rat);
    }

    SECTION("Decimal Negative Exponent Of Lesser Magnitude"){
        *big_rat = fmpq_from_scientific_str("12.3e-1");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 123);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 100);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("123.4e-2");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 617);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 500);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("0000000000000000000000000000000000000000000000012.3e-1");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 123);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 100);
        fmpq_clear(big_rat);

        *big_rat = fmpq_from_scientific_str("12.300000000000000000000000000000000000000000000000e-1");
        REQUIRE(fmpz_get_si(fmpq_numref(big_rat)) == 123);
        REQUIRE(fmpz_get_si(fmpq_denref(big_rat)) == 100);
        fmpq_clear(big_rat);
    }

    DEBUG_REQUIRE(isAllGmpMemoryFreed_resetOnFalse());
}
