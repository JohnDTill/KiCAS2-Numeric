#include <catch2/catch_test_macros.hpp>

#include "../src/ki_cas_native_rational.h"

#include "../src/ki_cas_native_integer.h"

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

    REQUIRE(true == ckd_mul(&result, NativeRational(1, 2), NativeRational(1, MAX)));
}

TEST_CASE( "ckd_div (NativeRational / size_t)" ) {
    NativeRational result;

    REQUIRE_FALSE(ckd_div(&result, NativeRational(3, 5), 2));
    REQUIRE(result.num == 3);
    REQUIRE(result.den == 10);

    REQUIRE_FALSE(ckd_div(&result, NativeRational(MAX, 2), MAX));
    REQUIRE(result.num == 1);
    REQUIRE(result.den == 2);

    REQUIRE_FALSE(ckd_div(&result, NativeRational(MAX, MAX), 2));
    REQUIRE(result.num == 1);
    REQUIRE(result.den == 2);

    REQUIRE(true == ckd_div(&result, NativeRational(MAX, MAX-2), 2));
}

TEST_CASE( "ckd_div (NativeRational / NativeRational)" ) {
    NativeRational result;

    REQUIRE_FALSE(ckd_div(&result, NativeRational(3, 5), NativeRational(4, 7)));
    REQUIRE(result.num == 21);
    REQUIRE(result.den == 20);

    // Want NUM not dividing MAX to avoid reductions making final non-canonical values unclear
    constexpr size_t NUM = 54321;
    static_assert(MAX % NUM != 0);

    REQUIRE_FALSE(ckd_div(&result, NativeRational(2, MAX), NativeRational(NUM, MAX)));
    REQUIRE(result.num == 2);
    REQUIRE(result.den == NUM);

    REQUIRE_FALSE(ckd_div(&result, NativeRational(MAX, NUM), NativeRational(MAX, 2)));
    REQUIRE(result.num == 2);
    REQUIRE(result.den == NUM);

    REQUIRE_FALSE(ckd_div(&result, NativeRational(2, NUM), NativeRational(MAX, MAX)));
    REQUIRE(result.num == 2);
    REQUIRE(result.den == NUM);

    REQUIRE_FALSE(ckd_div(&result, NativeRational(MAX, MAX), NativeRational(NUM, 2)));
    REQUIRE(result.num == 2);
    REQUIRE(result.den == NUM);

    REQUIRE(true == ckd_mul(&result, NativeRational(2, MAX-2), NativeRational(1, MAX)));
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

TEST_CASE( "write_native_rational" ) {
    std::string str = "x + ";
    NativeRational num(3,2);

    SECTION("plaintext"){
        write_native_rational<PLAINTEXT_OUTPUT>(str, num);
        REQUIRE(str == "x + 3/2");
    }

    SECTION("typeset"){
        write_native_rational<TYPESET_OUTPUT>(str, num);
        REQUIRE(str == "x + ⁜f⏴3⏵⏴2⏵");
    }
}

TEST_CASE( "ckd_strdecimaltail2rat" ) {
    NativeRational result;

    SECTION("No reduction"){
        REQUIRE_FALSE(ckd_strdecimaltail2rat(&result, ".7"));
        REQUIRE(result.num == 7);
        REQUIRE(result.den == 10);

        REQUIRE_FALSE(ckd_strdecimaltail2rat(&result, ".017"));
        REQUIRE(result.num == 17);
        REQUIRE(result.den == 1000);

        constexpr size_t large_fit_num = MAX / 10;
        std::string large_fit = '.' + std::to_string(large_fit_num);
        large_fit.back() = '1';
        REQUIRE_FALSE(ckd_strdecimaltail2rat(&result, large_fit));
        REQUIRE(result.num == large_fit_num + 1 - large_fit_num % 10);
        REQUIRE(result.den == knownfit_pow(10, large_fit.size()-1));

        std::string overflow_fit = '.' + std::to_string(MAX);
        overflow_fit.back() = '1';
        REQUIRE(true == ckd_strdecimaltail2rat(&result, overflow_fit));
    }

    SECTION("Trailing zero"){
        REQUIRE_FALSE(ckd_strdecimaltail2rat(&result, ".000000"));
        REQUIRE(result.num == 0);
        REQUIRE(result.den == 1);

        REQUIRE_FALSE(ckd_strdecimaltail2rat(&result, ".70000000000000000000000000000000000000000000000"));
        REQUIRE(result.num == 7);
        REQUIRE(result.den == 10);
    }

    SECTION("Factor of 2 reduction"){
        REQUIRE_FALSE(ckd_strdecimaltail2rat(&result, ".2"));
        REQUIRE(result.num == 1);
        REQUIRE(result.den == 5);

        REQUIRE_FALSE(ckd_strdecimaltail2rat(&result, ".04"));
        REQUIRE(result.num == 1);
        REQUIRE(result.den == 25);

        REQUIRE_FALSE(ckd_strdecimaltail2rat(&result, ".64"));
        REQUIRE(result.num == 16);
        REQUIRE(result.den == 25);

        constexpr size_t large_fit_num = (MAX / 16) * 16;  // Reduces to fit, despite 10^len(digits) too big
        const std::string large_fit = '.' + std::to_string(large_fit_num);
        REQUIRE_FALSE(ckd_strdecimaltail2rat(&result, large_fit));
        REQUIRE(large_fit_num % result.num == 0);

        constexpr size_t nonfitting_num = (MAX / 2) * 2;  // Reduces, but still doesn't fit
        const std::string nonfit = '.' + std::to_string(nonfitting_num);
        REQUIRE(true == ckd_strdecimaltail2rat(&result, nonfit));
    }

    SECTION("Factor of 5 reduction"){
        REQUIRE_FALSE(ckd_strdecimaltail2rat(&result, ".5"));
        REQUIRE(result.num == 1);
        REQUIRE(result.den == 2);

        REQUIRE_FALSE(ckd_strdecimaltail2rat(&result, ".25"));
        REQUIRE(result.num == 1);
        REQUIRE(result.den == 4);

        constexpr size_t large_fit_num = (MAX / 25) * 25;  // Reduces to fit, despite 10^len(digits) too big
        const std::string large_fit = '.' + std::to_string(large_fit_num);
        REQUIRE_FALSE(ckd_strdecimaltail2rat(&result, large_fit));
        REQUIRE(large_fit_num % result.num == 0);

        if(sizeof(size_t) == 8){
            constexpr size_t nonfitting_num = (MAX / 5) * 5;  // Reduces, but still doesn't fit
            const std::string nonfit = '.' + std::to_string(nonfitting_num);
            REQUIRE(true == ckd_strdecimaltail2rat(&result, nonfit));
        }
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

        REQUIRE_FALSE(ckd_strdecimal2rat(&result, "123.4"));
        REQUIRE(result.num == 617);
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

        REQUIRE_FALSE(ckd_strdecimal2rat(&result, "3."));
        REQUIRE(result.num == 3);
        REQUIRE(result.den == 1);
    }

    SECTION("Leading zeros"){
        REQUIRE_FALSE(ckd_strdecimal2rat(&result, ".3"));
        REQUIRE(result.num == 3);
        REQUIRE(result.den == 10);

        REQUIRE_FALSE(ckd_strdecimal2rat(&result, "0.3"));
        REQUIRE(result.num == 3);
        REQUIRE(result.den == 10);

        REQUIRE_FALSE(ckd_strdecimal2rat(&result, "000.3"));
        REQUIRE(result.num == 3);
        REQUIRE(result.den == 10);

        REQUIRE_FALSE(ckd_strdecimal2rat(&result, "001.3"));
        REQUIRE(result.num == 13);
        REQUIRE(result.den == 10);
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
        test_str = "0." + std::string(std::numeric_limits<size_t>::digits10+1, '9');
        REQUIRE(true == ckd_strdecimal2rat(&result, test_str));

        test_str = "0." + std::string(std::numeric_limits<size_t>::digits10, '9');
        REQUIRE(false == ckd_strdecimal2rat(&result, test_str));
        REQUIRE(std::to_string(result.num) == std::string(std::numeric_limits<size_t>::digits10, '9'));
        REQUIRE(std::to_string(result.den) == '1' + std::string(std::numeric_limits<size_t>::digits10, '0'));

        // Test a combination of leading and trailing values which does not fit
        test_str = "999." + std::string(std::numeric_limits<size_t>::digits10, '9');
        REQUIRE(true == ckd_strdecimal2rat(&result, test_str));

        test_str = "0." + std::string(std::numeric_limits<size_t>::digits10, '9');
        REQUIRE(false == ckd_strdecimal2rat(&result, test_str));

        // Test a leading value which does not fit
        REQUIRE(true == ckd_strdecimal2rat(&result, "265252859812191058636308480000000.1"));
    }
}

TEST_CASE( "ckd_strscientific2rat" ) {
    NativeRational result;

    SECTION("Integer Positive Exponent"){
        REQUIRE_FALSE(ckd_strscientific2rat(&result, "1e2"));
        REQUIRE(result.num == 100);
        REQUIRE(result.den == 1);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "0e2"));
        REQUIRE(result.num == 0);
        REQUIRE(result.den == 1);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "23e1"));
        REQUIRE(result.num == 230);
        REQUIRE(result.den == 1);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "0000000000000000000000000000000000000000000000001234e5"));
        REQUIRE(result.num == 123400000);
        REQUIRE(result.den == 1);

        REQUIRE(true == ckd_strscientific2rat(&result, "1e100"));
    }

    SECTION("Integer Negative Exponent"){
        REQUIRE_FALSE(ckd_strscientific2rat(&result, "1e-2"));
        REQUIRE(result.num == 1);
        REQUIRE(result.den == 100);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "0e-2"));
        REQUIRE(result.num == 0);
        REQUIRE(result.den == 1);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "23e-1"));
        REQUIRE(result.num == 23);
        REQUIRE(result.den == 10);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "0000000000000000000000000000000000000000000000004321e-5"));
        REQUIRE(result.num == 4321);
        REQUIRE(result.den == 100000);

        REQUIRE(true == ckd_strscientific2rat(&result, "1e-100"));
    }

    SECTION("Decimal Positive Exponent To Integer"){
        REQUIRE_FALSE(ckd_strscientific2rat(&result, "1.2e3"));
        REQUIRE(result.num == 1200);
        REQUIRE(result.den == 1);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "0.2e3"));
        REQUIRE(result.num == 200);
        REQUIRE(result.den == 1);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "000000000000000000000000000000000000000000000000.2e3"));
        REQUIRE(result.num == 200);
        REQUIRE(result.den == 1);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "0.200000000000000000000000000000000000000000000000e3"));
        REQUIRE(result.num == 200);
        REQUIRE(result.den == 1);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "0.0e3"));
        REQUIRE(result.num == 0);
        REQUIRE(result.den == 1);

        REQUIRE(true == ckd_strscientific2rat(&result, "1.2e100"));
    }

    SECTION("Decimal Positive Exponent To Rational"){
        REQUIRE_FALSE(ckd_strscientific2rat(&result, "1.23e1"));
        REQUIRE(result.num == 123);
        REQUIRE(result.den == 10);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "0.002e2"));
        REQUIRE(result.num == 1);
        REQUIRE(result.den == 5);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "0000000000000000000000000000000000000000000000000.005e2"));
        REQUIRE(result.num == 1);
        REQUIRE(result.den == 2);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, ".0050000000000000000000000000000000000000000000000000e2"));
        REQUIRE(result.num == 1);
        REQUIRE(result.den == 2);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "0.00000e1"));
        REQUIRE(result.num == 0);
        REQUIRE(result.den == 1);

        REQUIRE(true == ckd_strscientific2rat(&result, "1.2345678901234567890123456789e5"));
    }

    SECTION("Decimal Negative Exponent Of Greater Magnitude"){
        REQUIRE_FALSE(ckd_strscientific2rat(&result, "1.23e-1"));
        REQUIRE(result.num == 123);
        REQUIRE(result.den == 1000);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "0.1e-2"));
        REQUIRE(result.num == 1);
        REQUIRE(result.den == 1000);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "0000000000000000000000000000000000000000000000000.5e-1"));
        REQUIRE(result.num == 1);
        REQUIRE(result.den == 20);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, ".50000000000000000000000000000000000000000000000000e-1"));
        REQUIRE(result.num == 1);
        REQUIRE(result.den == 20);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "0.00000e-2"));
        REQUIRE(result.num == 0);
        REQUIRE(result.den == 1);

        REQUIRE(true == ckd_strscientific2rat(&result, "1.2345678901234567890123456789e-30"));
    }

    SECTION("Decimal Negative Exponent Of Lesser Magnitude"){
        REQUIRE_FALSE(ckd_strscientific2rat(&result, "12.3e-1"));
        REQUIRE(result.num == 123);
        REQUIRE(result.den == 100);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "123.4e-2"));
        REQUIRE(result.num == 617);
        REQUIRE(result.den == 500);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "0000000000000000000000000000000000000000000000012.3e-1"));
        REQUIRE(result.num == 123);
        REQUIRE(result.den == 100);

        REQUIRE_FALSE(ckd_strscientific2rat(&result, "12.300000000000000000000000000000000000000000000000e-1"));
        REQUIRE(result.num == 123);
        REQUIRE(result.den == 100);

        REQUIRE(true == ckd_strscientific2rat(&result, "123456789012345.67890123456789e-3"));
    }
}
