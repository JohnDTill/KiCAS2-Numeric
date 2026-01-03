#include <catch2/catch_test_macros.hpp>

#include "../src/ki_cas_big_num_wrapper.h"

using namespace KiCAS2;

static constexpr size_t MAX = std::numeric_limits<size_t>::max();

TEST_CASE( "GMP memory leak detection mechanism" ) {
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    mpz_t gmp_int;
    mpz_init_set_ui(gmp_int, 42);
    DEBUG_REQUIRE_FALSE(isAllGmpMemoryFreed());
    mpz_clear(gmp_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    mpq_t gmp_rat;
    mpq_init(gmp_rat);
    mpq_set_ui(gmp_rat, 1, 42);
    // DEBUG_REQUIRE_FALSE(isAllGmpMemoryFreed());  // GMP does not allocate here on all environments.
                                                    // It may be an accident that this check generally works.
    mpq_clear(gmp_rat);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    fmpz_t flint_int;
    fmpz_init_set_ui(flint_int, 42);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());  // No allocation for Flint since word sized
    fmpz_fac_ui(flint_int, 30);
    DEBUG_REQUIRE_FALSE(isAllGmpMemoryFreed());
    fmpz_clear(flint_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    fmpq_t flint_rat;
    fmpq_init(flint_rat);
    fmpq_set_ui(flint_rat, 1337, 3407);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());  // No allocation for Flint since word sized
    fmpz_fac_ui(fmpq_denominator_ptr(flint_rat), 30);
    DEBUG_REQUIRE_FALSE(isAllGmpMemoryFreed());
    fmpq_clear(flint_rat);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());
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

    DEBUG_REQUIRE(isAllGmpMemoryFreed());
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

    DEBUG_REQUIRE(isAllGmpMemoryFreed());
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

    DEBUG_REQUIRE(isAllGmpMemoryFreed());
}

TEST_CASE( "fmpz_sizeinbase10upperbound" ) {
    fmpz_t val;

    fmpz_init_set_si(val, 42);
    REQUIRE(fmpz_sizeinbase10upperbound(val) >= fmpz_sizeinbase(val, 10));
    fmpz_clear(val);

    fmpz_init_set_si(val, MAX);
    REQUIRE(fmpz_sizeinbase10upperbound(val) >= fmpz_sizeinbase(val, 10));
    fmpz_clear(val);

    fmpz_init(val);
    fmpz_set_str(val, "265252859812191058636308480000000", 10);
    REQUIRE(fmpz_sizeinbase10upperbound(val) >= fmpz_sizeinbase(val, 10));
    fmpz_clear(val);

    DEBUG_REQUIRE(isAllGmpMemoryFreed());
}

TEST_CASE( "fmpq_abs_inplace" ) {
    fmpq_t val;
    fmpz_init_set_si(fmpq_numref(val), -1);
    fmpz_init_set_ui(fmpq_denref(val), 4);
    REQUIRE(std::string(fmpq_get_str(NULL, 10, val)) == "-1/4");
    fmpq_abs_inplace(val);
    REQUIRE(std::string(fmpq_get_str(NULL, 10, val)) == "1/4");

    fmpz_init_set_si(fmpq_numref(val), COEFF_MIN);
    fmpq_abs_inplace(val);
    REQUIRE(fmpz_get_ui(fmpq_numref(val)) == COEFF_MAX);

    DEBUG_REQUIRE(isAllGmpMemoryFreed());
}

TEST_CASE( "mpz_init_set_strview (safe)" ) {
    mpz_t big_int;

    mpz_init_set_strview(big_int, "0");
    REQUIRE(mpz_get_si(big_int) == 0);
    mpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    mpz_init_set_strview(big_int, "42");
    REQUIRE(mpz_get_si(big_int) == 42);
    mpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    mpz_init_set_strview(big_int, std::to_string(MAX));
    char buffer[128];
    REQUIRE(mpz_get_str(buffer, 10, big_int) == std::to_string(MAX));
    mpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    mpz_t factorial_of_30;
    mpz_init(factorial_of_30);
    mpz_fac_ui(factorial_of_30, 30);
    mpz_init_set_strview(big_int, "265252859812191058636308480000000");
    REQUIRE(mpz_cmp(big_int, factorial_of_30) == 0);

    mpz_clear(big_int);
    mpz_clear(factorial_of_30);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());
}

TEST_CASE( "mpz_init_set_strview (unsafe)" ) {
    mpz_t big_int;
    std::string input;

    input = "0";
    mpz_init_set_mutable_str(big_int, input, 0, 1);
    REQUIRE(mpz_get_si(big_int) == 0);
    REQUIRE(input == "0");
    mpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    input = "42";
    mpz_init_set_mutable_str(big_int, input, 0, 2);
    REQUIRE(mpz_get_si(big_int) == 42);
    REQUIRE(input == "42");
    mpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    input = std::to_string(MAX);
    mpz_init_set_mutable_str(big_int, input, 0, input.size());
    char buffer[128];
    REQUIRE(mpz_get_str(buffer, 10, big_int) == std::to_string(MAX));
    REQUIRE(input == std::to_string(MAX));
    mpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    input = "x = 265252859812191058636308480000000";
    mpz_t factorial_of_30;
    mpz_init(factorial_of_30);
    mpz_fac_ui(factorial_of_30, 30);
    mpz_init_set_mutable_str(big_int, input, 4, input.size()-4);
    REQUIRE(mpz_cmp(big_int, factorial_of_30) == 0);
    REQUIRE(input == "x = 265252859812191058636308480000000");

    mpz_clear(big_int);
    mpz_clear(factorial_of_30);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());
}

TEST_CASE( "fmpz_init_set_strview (safe)" ) {
    fmpz_t big_int;

    fmpz_init_set_strview(big_int, "0");
    REQUIRE(fmpz_get_si(big_int) == 0);
    fmpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    fmpz_init_set_strview(big_int, "42");
    REQUIRE(fmpz_get_si(big_int) == 42);
    fmpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    fmpz_init_set_strview(big_int, std::to_string(MAX));
    REQUIRE(fmpz_get_ui(big_int) == MAX);
    fmpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    fmpz_t factorial_of_30;
    fmpz_init(factorial_of_30);
    fmpz_fac_ui(factorial_of_30, 30);
    fmpz_init_set_strview(big_int, "265252859812191058636308480000000");
    REQUIRE(fmpz_cmp(big_int, factorial_of_30) == 0);

    fmpz_clear(big_int);
    fmpz_clear(factorial_of_30);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());
}

TEST_CASE( "fmpz_init_set_mutable_str" ) {
    fmpz_t big_int;
    std::string input;

    input = "0";
    fmpz_init_set_mutable_str(big_int, input, 0, 1);
    REQUIRE(fmpz_get_si(big_int) == 0);
    REQUIRE(input == "0");
    fmpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    input = "42";
    fmpz_init_set_mutable_str(big_int, input, 0, 2);
    REQUIRE(fmpz_get_si(big_int) == 42);
    REQUIRE(input == "42");
    fmpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    input = std::to_string(MAX);
    fmpz_init_set_mutable_str(big_int, input, 0, input.size());
    REQUIRE(fmpz_get_ui(big_int) == MAX);
    REQUIRE(input == std::to_string(MAX));
    fmpz_clear(big_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    input = "x = 265252859812191058636308480000000";
    fmpz_t factorial_of_30;
    fmpz_init(factorial_of_30);
    fmpz_fac_ui(factorial_of_30, 30);
    fmpz_init_set_mutable_str(big_int, input, 4, input.size()-4);
    REQUIRE(fmpz_cmp(big_int, factorial_of_30) == 0);
    REQUIRE(input == "x = 265252859812191058636308480000000");

    fmpz_clear(big_int);
    fmpz_clear(factorial_of_30);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());
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
