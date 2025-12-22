#include <catch2/catch_test_macros.hpp>

#include "../src/ki_cas_native_rational.h"

using namespace KiCAS2;

static constexpr size_t MAX = std::numeric_limits<size_t>::max();

TEST_CASE( "NativeRational conversions" ) {
    NativeRational num(5, 2);
    REQUIRE(static_cast<long double>(num) == 2.5l);
    REQUIRE(static_cast<double>(num) == 2.5);
    REQUIRE(static_cast<float>(num) == 2.5f);
    REQUIRE(static_cast<size_t>(num) == 2);
}

static void compareHelper(NativeRational rat, size_t floor, bool is_floor) {
    assert(floor < MAX);

    REQUIRE((rat == floor) == is_floor);
    REQUIRE((floor == rat) == is_floor);
    REQUIRE((rat != floor) == !is_floor);
    REQUIRE((floor != rat) == !is_floor);

    REQUIRE( !(rat == floor+1) );
    REQUIRE( !(floor+1 == rat) );
    REQUIRE( rat != floor+1 );
    REQUIRE( floor+1 != rat );

    REQUIRE(rat >= floor);
    REQUIRE(floor <= rat);
    REQUIRE((rat <= floor) == is_floor);
    REQUIRE((floor >= rat) == is_floor);

    REQUIRE((rat > floor) == !is_floor);
    REQUIRE((floor < rat) == !is_floor);
    REQUIRE(!(rat < floor));
    REQUIRE(!(floor > rat));

    REQUIRE(!(rat >= floor+1));
    REQUIRE(!(floor+1 <= rat));
    REQUIRE(rat <= floor+1);
    REQUIRE(floor+1 >= rat);

    REQUIRE(!(rat > floor+1));
    REQUIRE(!(floor+1 < rat));
    REQUIRE(rat < floor+1);
    REQUIRE(floor+1 > rat);
}

TEST_CASE( "NativeRational vs. size_t comparisons" ) {
    compareHelper(NativeRational(3,2), 1, false);
    compareHelper(NativeRational(10,5), 2, true);
    compareHelper(NativeRational(MAX, MAX-1), 1, false);
    compareHelper(NativeRational(MAX-1, MAX-1), 1, true);
}

TEST_CASE( "NativeRational vs. NativeRational comparisons" ) {
    REQUIRE(!(NativeRational(1,3) == NativeRational(1,5)));
    REQUIRE(NativeRational(1,3) != NativeRational(1,5));
    REQUIRE(NativeRational(1,3) > NativeRational(1,5));
    REQUIRE(NativeRational(1,3) >= NativeRational(1,5));
    REQUIRE(NativeRational(1,5) < NativeRational(1,3));
    REQUIRE(NativeRational(1,5) <= NativeRational(1,3));

    REQUIRE(NativeRational(7,3) == NativeRational(14,6));
    REQUIRE(!(NativeRational(7,3) != NativeRational(14,6)));
    REQUIRE(!(NativeRational(7,3) > NativeRational(14,6)));
    REQUIRE(NativeRational(7,3) >= NativeRational(14,6));
    REQUIRE(!(NativeRational(7,3) < NativeRational(14,6)));
    REQUIRE(NativeRational(7,3) <= NativeRational(14,6));

    REQUIRE(!(NativeRational(2, MAX-2) == NativeRational(2, MAX)));
    REQUIRE(NativeRational(2, MAX-2) != NativeRational(2, MAX));
    REQUIRE(NativeRational(2, MAX-2) >= NativeRational(2, MAX));
    REQUIRE(NativeRational(2, MAX-2) > NativeRational(2, MAX));
    REQUIRE(NativeRational(2, MAX-2) <= NativeRational(2, MAX-2));
    REQUIRE(NativeRational(2, MAX) < NativeRational(2, MAX-2));

    REQUIRE(NativeRational(6, MAX-1) == NativeRational(3, (MAX-1)/2));
    REQUIRE(!(NativeRational(6, MAX-1) != NativeRational(3, (MAX-1)/2)));
    REQUIRE(NativeRational(6, MAX-1) >= NativeRational(3, (MAX-1)/2));
    REQUIRE(!(NativeRational(6, MAX-1) > NativeRational(3, (MAX-1)/2)));
    REQUIRE(NativeRational(6, MAX-1) <= NativeRational(3, (MAX-1)/2));
    REQUIRE(!(NativeRational(6, MAX-1) < NativeRational(3, (MAX-1)/2)));
}

TEST_CASE( "NativeRational::reduceInPlace" ) {
    NativeRational one_half(2, 4);
    one_half.reduceInPlace();
    REQUIRE(one_half.num == 1);
    REQUIRE(one_half.den == 2);

    NativeRational one(MAX, MAX);
    one.reduceInPlace();
    REQUIRE(one.num == 1);
    REQUIRE(one.den == 1);

    NativeRational zero(0, 10);
    zero.reduceInPlace();
    REQUIRE(zero.num == 0);
    REQUIRE(zero.den == 1);

    NativeRational irreducible(MAX-1, MAX);
    irreducible.reduceInPlace();
    REQUIRE(irreducible.num == MAX-1);
    REQUIRE(irreducible.den == MAX);
}

TEST_CASE( "ckd_mul (NativeRational * size_t)" ) {
    NativeRational result;

    REQUIRE_FALSE(ckd_mul(&result, NativeRational(3, 5), 2));
    REQUIRE(result.num == 6);
    REQUIRE(result.den == 5);

    REQUIRE_FALSE(ckd_mul(&result, NativeRational(2, MAX), MAX));
    REQUIRE(result.num == 2);
    REQUIRE(result.den == 1);

    REQUIRE_FALSE(ckd_mul(&result, NativeRational(MAX, MAX), 2));
    REQUIRE(result.num == 2);
    REQUIRE(result.den == 1);

    REQUIRE(true == ckd_mul(&result, NativeRational(MAX, MAX-2), 2));
}

TEST_CASE( "ckd_mul (NativeRational * NativeRational)" ) {
    NativeRational result;

    REQUIRE_FALSE(ckd_mul(&result, NativeRational(3, 5), NativeRational(4, 7)));
    REQUIRE(result.num == 12);
    REQUIRE(result.den == 35);

    constexpr size_t MAX = std::numeric_limits<size_t>::max();

    // Want NUM not dividing MAX to avoid reductions making final non-canonical values unclear
    constexpr size_t NUM = 54321;
    static_assert(MAX % NUM != 0);

    REQUIRE_FALSE(ckd_mul(&result, NativeRational(2, MAX), NativeRational(MAX, NUM)));
    REQUIRE(result.num == 2);
    REQUIRE(result.den == NUM);

    REQUIRE_FALSE(ckd_mul(&result, NativeRational(MAX, NUM), NativeRational(2, MAX)));
    REQUIRE(result.num == 2);
    REQUIRE(result.den == NUM);

    REQUIRE_FALSE(ckd_mul(&result, NativeRational(2, NUM), NativeRational(MAX, MAX)));
    REQUIRE(result.num == 2);
    REQUIRE(result.den == NUM);

    REQUIRE_FALSE(ckd_mul(&result, NativeRational(MAX, MAX), NativeRational(2, NUM)));
    REQUIRE(result.num == 2);
    REQUIRE(result.den == NUM);
}

TEST_CASE( "ckd_add (NativeRational + size_t)" ) {
    NativeRational result;

    REQUIRE_FALSE(ckd_add(&result, NativeRational(1,2), 1));
    REQUIRE(result.num == 3);
    REQUIRE(result.den == 2);

    static_assert(MAX % 3 == 0);
    REQUIRE_FALSE(ckd_add(&result, NativeRational(MAX/3, MAX), 2));
    REQUIRE(result.num == 7);
    REQUIRE(result.den == 3);

    REQUIRE(true == ckd_add(&result, NativeRational(MAX, 2), 2));
}

TEST_CASE( "ckd_add (NativeRational + NativeRational)" ) {
    NativeRational result;

    REQUIRE_FALSE(ckd_add(&result, NativeRational(1,3), NativeRational(2,5)));
    REQUIRE(result.num == 11);
    REQUIRE(result.den == 15);

    REQUIRE_FALSE(ckd_add(&result, NativeRational(1, MAX), NativeRational(1, MAX)));
    REQUIRE(result.num == 2);
    REQUIRE(result.den == MAX);

    REQUIRE_FALSE(ckd_add(&result, NativeRational(1, 2), NativeRational(MAX, MAX)));
    REQUIRE(result.num == 3);
    REQUIRE(result.den == 2);

    REQUIRE_FALSE(ckd_add(&result, NativeRational(MAX, MAX), NativeRational(1, 2)));
    REQUIRE(result.num == 3);
    REQUIRE(result.den == 2);

    REQUIRE(true == ckd_add(&result, NativeRational(1, MAX), NativeRational(1, 2)));
}

TEST_CASE( "sub (NativeRational - size_t)" ) {
    NativeRational result;

    result = sub(NativeRational(17, 5), 2);
    REQUIRE(result.num == 7);
    REQUIRE(result.den == 5);

    result = sub(NativeRational(121, 2), 50);
    REQUIRE(result.num == 21);
    REQUIRE(result.den == 2);
}

TEST_CASE( "ckd_sub (size_t - NativeRational)" ) {
    NativeRational result;

    REQUIRE_FALSE(ckd_sub(&result, 2, NativeRational(1, 2)));
    REQUIRE(result.num == 3);
    REQUIRE(result.den == 2);

    REQUIRE_FALSE(ckd_sub(&result, 2, NativeRational((MAX-1)/2, MAX-1)));
    REQUIRE(result.num == 3);
    REQUIRE(result.den == 2);

    REQUIRE(true == ckd_sub(&result, 2, NativeRational(1, MAX)));
}

TEST_CASE( "ckd_sub (NativeRational - NativeRational)" ) {
    NativeRational result;

    REQUIRE_FALSE(ckd_sub(&result, NativeRational(2,5), NativeRational(1,3)));
    REQUIRE(result.num == 1);
    REQUIRE(result.den == 15);

    REQUIRE_FALSE(ckd_sub(&result, NativeRational(2, MAX), NativeRational(2, MAX)));
    REQUIRE(result.num == 0);
    REQUIRE(result.den == MAX);

    REQUIRE_FALSE(ckd_sub(&result, NativeRational(2, 3), NativeRational((MAX-1)/2, MAX-1)));
    REQUIRE(result.num == 1);
    REQUIRE(result.den == 6);

    REQUIRE(true == ckd_sub(&result, NativeRational(1, MAX-1), NativeRational(1, MAX)));
}
