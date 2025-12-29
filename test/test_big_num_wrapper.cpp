#include <catch2/catch_test_macros.hpp>

#include "../src/ki_cas_big_num_wrapper.h"

using namespace KiCAS2;

TEST_CASE( "GMP memory leak detection mechanism" ) {
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    mpz_t gmp_int;
    mpz_init_set_ui(gmp_int, 42);
    DEBUG_REQUIRE_FALSE(isAllGmpMemoryFreed());
    mpz_clear(gmp_int);
    DEBUG_REQUIRE(isAllGmpMemoryFreed());

    mpq_t gmp_rat;
    mpq_init(gmp_rat);
    mpq_set_ui(gmp_rat, 1337, 3407);
    DEBUG_REQUIRE_FALSE(isAllGmpMemoryFreed());
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
