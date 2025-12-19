#include <catch2/catch_test_macros.hpp>

#include "../src/ki_cas_integer_math.h"

#include <cmath>
#include <limits>
#include <optional>

using namespace KiCAS2;

TEST_CASE( "ckd_add" ) {
    size_t result;

    REQUIRE_FALSE(ckd_add(&result, 0, 0));
    REQUIRE(result == 0);

    REQUIRE_FALSE(ckd_add(&result, 1, 1));
    REQUIRE(result == 2);

    REQUIRE_FALSE(ckd_add(&result, 0, std::numeric_limits<size_t>::max()));
    REQUIRE(result == std::numeric_limits<size_t>::max());

    REQUIRE(true == ckd_add(&result, 1, std::numeric_limits<size_t>::max()));

    REQUIRE_FALSE(ckd_add(&result, std::numeric_limits<size_t>::max()/2, std::numeric_limits<size_t>::max()/2));
    REQUIRE(result == std::numeric_limits<size_t>::max()-1);

    REQUIRE(true == ckd_add(&result, 1+std::numeric_limits<size_t>::max()/2, 1+std::numeric_limits<size_t>::max()/2));
}

TEST_CASE( "ckd_mul" ) {
    size_t result;

    REQUIRE_FALSE(ckd_mul(&result, 0, 0));
    REQUIRE(result == 0);

    REQUIRE_FALSE(ckd_mul(&result, 1, 1));
    REQUIRE(result == 1);

    REQUIRE_FALSE(ckd_mul(&result, 2, 2));
    REQUIRE(result == 4);

    REQUIRE_FALSE(ckd_mul(&result, 1, std::numeric_limits<size_t>::max()));
    REQUIRE(result == std::numeric_limits<size_t>::max());

    REQUIRE(true == ckd_mul(&result, 2, std::numeric_limits<size_t>::max()));

    REQUIRE_FALSE(ckd_mul(&result, 2, std::numeric_limits<size_t>::max()/2));
    REQUIRE(result == std::numeric_limits<size_t>::max()-1);

    REQUIRE(true == ckd_mul(&result, 3, std::numeric_limits<size_t>::max()/2));
}

TEST_CASE( "ckd_sqrt" ) {
    size_t result;

    REQUIRE_FALSE(ckd_sqrt(&result, 0));
    REQUIRE(result == 0);

    REQUIRE_FALSE(ckd_sqrt(&result, 1));
    REQUIRE(result == 1);

    REQUIRE_FALSE(ckd_sqrt(&result, 4));
    REQUIRE(result == 2);

    REQUIRE(true == ckd_sqrt(&result, 2));
    REQUIRE(true == ckd_sqrt(&result, 3));

    constexpr size_t max_squarable_number = std::numeric_limits<size_t>::max() >> (sizeof(size_t)*8/2);
    REQUIRE_FALSE(ckd_sqrt(&result, max_squarable_number * max_squarable_number));
    REQUIRE(result == max_squarable_number);

    for(size_t i = max_squarable_number - 10; i < max_squarable_number; i++){
        REQUIRE_FALSE(ckd_sqrt(&result, i * i));
        REQUIRE(result == i);
    }
}

/* This is feasible for 64-bit size_t, but a ~1hr runtime. */
// TEST_CASE( "ckd_sqrt (Exhaustive)" ) {
//     size_t result;
//     constexpr size_t max_squarable_number = std::numeric_limits<size_t>::max() >> (sizeof(size_t)*8/2);
//     for (size_t i = 0; i <= max_squarable_number; i++){
//         const size_t square = i * i;
//         REQUIRE_FALSE(ckd_sqrt(&result, square));
//         REQUIRE(result == i);
//     }
// }

TEST_CASE( "ckd_cbrt" ) {
    size_t result;

    REQUIRE_FALSE(ckd_cbrt(&result, 0));
    REQUIRE(result == 0);

    REQUIRE_FALSE(ckd_cbrt(&result, 1));
    REQUIRE(result == 1);

    REQUIRE_FALSE(ckd_cbrt(&result, 8));
    REQUIRE(result == 2);

    REQUIRE_FALSE(ckd_cbrt(&result, 27));
    REQUIRE(result == 3);

    REQUIRE(true == ckd_cbrt(&result, 2));
    REQUIRE(true == ckd_cbrt(&result, 3));
    REQUIRE(true == ckd_cbrt(&result, 4));

    const size_t max_cubeable_number = std::cbrtl(std::numeric_limits<size_t>::max());
    REQUIRE_FALSE(ckd_cbrt(&result, max_cubeable_number * max_cubeable_number * max_cubeable_number));
    REQUIRE(result == max_cubeable_number);

    for(size_t i = max_cubeable_number - 10; i < max_cubeable_number; i++){
        REQUIRE_FALSE(ckd_cbrt(&result, i * i * i));
        REQUIRE(result == i);
    }
}

TEST_CASE( "ckd_cbrt (Exhaustive)" ) {
    size_t result;
    const size_t max_cubeable_number = std::cbrtl(std::numeric_limits<size_t>::max());
    std::optional<size_t> failing_number = std::nullopt;

    for (size_t i = 0; i <= max_cubeable_number; i++){
        const bool no_int_root = ckd_cbrt(&result, i*i*i);
        const bool incorrect = (result != i);

        if(no_int_root || incorrect){
            failing_number = i;
            break;
        }
    }

    REQUIRE(!failing_number.has_value());
}

TEST_CASE( "ckd_nrt" ) {
    size_t result;

    REQUIRE_FALSE(ckd_nrt(&result, 0, 4));
    REQUIRE(result == 0);

    REQUIRE_FALSE(ckd_nrt(&result, 1, 4));
    REQUIRE(result == 1);

    REQUIRE_FALSE(ckd_nrt(&result, 16, 4));
    REQUIRE(result == 2);

    REQUIRE_FALSE(ckd_nrt(&result, 81, 4));
    REQUIRE(result == 3);

    REQUIRE(true == ckd_nrt(&result, 2, 4));
    REQUIRE(true == ckd_nrt(&result, 3, 4));
    REQUIRE(true == ckd_nrt(&result, 4, 4));

    constexpr size_t max_exponent = sizeof(size_t)*8 - 1;
    REQUIRE_FALSE(ckd_nrt(&result, 1uLL << max_exponent, max_exponent));
    REQUIRE(result == 2);

    REQUIRE(true == ckd_nrt(&result, 1uLL << max_exponent, max_exponent-1));
}

TEST_CASE( "ckd_nrt (Exhaustive)" ) {
    size_t result;
    constexpr size_t max_exponent = sizeof(size_t)*8 - 1;
    std::optional<std::pair<size_t, size_t>> failing_nrt = std::nullopt;

    for(size_t exp = 4; exp <= max_exponent; exp++){
        constexpr double fudge_factor = 1e-6;  // EVENTUALLY: this keeps solving as 2^64, which is out of range
        const size_t max_base = std::powl(std::numeric_limits<size_t>::max(), 1.0/exp) - fudge_factor;

        for(size_t base = 0; base <= max_base; base++){
            const size_t arg = knownfit_pow(base, exp);

            const bool no_int_root = ckd_nrt(&result, arg, exp);
            const bool incorrect = (result != base);

            if(no_int_root || incorrect){
                failing_nrt = std::make_pair(base, exp);
                break;
            }
        }

        if(failing_nrt.has_value()) break;
    }

    REQUIRE(!failing_nrt.has_value());
}

TEST_CASE( "ckd_pow" ) {
    size_t result;

    REQUIRE_FALSE(ckd_pow(&result, 2, 3));
    REQUIRE(result == 8);

    REQUIRE_FALSE(ckd_pow(&result, 2, sizeof(size_t)*8-1));
    REQUIRE(result == (1uLL << (sizeof(size_t)*8-1)));

    REQUIRE(true == ckd_pow(&result, 2, sizeof(size_t)*8));
}

TEST_CASE( "ckd_pow (More thorough)" ) {
    size_t result;
    constexpr size_t max_exp = sizeof(size_t)*8-1;
    std::optional<std::pair<size_t, size_t>> failing_pow = std::nullopt;

    for(size_t exp = 2; exp <= max_exp; exp++){
        constexpr double fudge_factor = 1e-6;  // EVENTUALLY: this keeps solving as 2^64, which is out of range
        const size_t max_base = std::powl(std::numeric_limits<size_t>::max(), 1.0/exp) - fudge_factor;

        const bool overflow = ckd_pow(&result, max_base, exp);
        const bool incorrect = !overflow && knownfit_pow(max_base, exp) != result;

        if(overflow || incorrect){
            failing_pow = std::make_pair(max_base, exp);
            break;
        }
    }

    REQUIRE(!failing_pow.has_value());
}
