#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include "ki_cas_big_num_wrapper.h"

using namespace KiCAS2;

TEST_CASE("fmpz_init_set_strview (tiny)") {
    const std::string_view str = "1337";

    BENCHMARK_ADVANCED( "fmpz_init_set_strview" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init_set_strview(big_int, str);});
        fmpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "fmpz_set_str" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init(big_int); std::string copy(str); fmpz_set_str(big_int, copy.c_str(), 10);});
        fmpz_clear(big_int);
    };
};

TEST_CASE("fmpz_init_set_strview (Flint word size)") {
    const std::string src = std::to_string(COEFF_MAX);
    const std::string_view str(src);

    BENCHMARK_ADVANCED( "fmpz_init_set_strview" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init_set_strview(big_int, str);});
        fmpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "fmpz_set_str" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init(big_int); std::string copy(str); fmpz_set_str(big_int, copy.c_str(), 10);});
        fmpz_clear(big_int);
    };
};

TEST_CASE("fmpz_init_set_strview (Flint word size x2)") {
    const std::string src = std::to_string(std::numeric_limits<size_t>::max()) + "00";
    const std::string_view str(src);

    BENCHMARK_ADVANCED( "fmpz_init_set_strview" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init_set_strview(big_int, str);});
        fmpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "fmpz_set_str" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init(big_int); std::string copy(str); fmpz_set_str(big_int, copy.c_str(), 10);});
        fmpz_clear(big_int);
    };
};

TEST_CASE("fmpz_init_set_strview (Flint word size x3)") {
    const std::string src = std::to_string(std::numeric_limits<size_t>::max())
                          + std::to_string(std::numeric_limits<size_t>::max())
                          + "0000";
    const std::string_view str(src);

    BENCHMARK_ADVANCED( "fmpz_init_set_strview" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init_set_strview(big_int, str);});
        fmpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "fmpz_set_str" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init(big_int); std::string copy(str); fmpz_set_str(big_int, copy.c_str(), 10);});
        fmpz_clear(big_int);
    };
};

TEST_CASE("mpz_init_set_strview (tiny)") {
    const std::string_view str = "1337";

    BENCHMARK_ADVANCED( "mpz_init_set_strview" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init_set_strview(big_int, str);});
        mpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "mpz_set_str" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init(big_int); std::string copy(str); mpz_set_str(big_int, copy.c_str(), 10);});
        mpz_clear(big_int);
    };
};

TEST_CASE("mpz_init_set_strview (word size)") {
    const std::string src = std::to_string(COEFF_MAX);
    const std::string_view str(src);

    BENCHMARK_ADVANCED( "mpz_init_set_strview" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init_set_strview(big_int, str);});
        mpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "mpz_set_str" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init(big_int); std::string copy(str); mpz_set_str(big_int, copy.c_str(), 10);});
        mpz_clear(big_int);
    };
};

TEST_CASE("mpz_init_set_strview (Flint word size x2)") {
    const std::string src = std::to_string(std::numeric_limits<size_t>::max()) + "00";
    const std::string_view str(src);

    BENCHMARK_ADVANCED( "mpz_init_set_strview" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init_set_strview(big_int, str);});
        mpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "mpz_set_str" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init(big_int); std::string copy(str); mpz_set_str(big_int, copy.c_str(), 10);});
        mpz_clear(big_int);
    };
};

TEST_CASE("mpz_init_set_strview (Flint word size x3)") {
    const std::string src = std::to_string(std::numeric_limits<size_t>::max())
                          + std::to_string(std::numeric_limits<size_t>::max())
                          + "0000";
    const std::string_view str(src);

    BENCHMARK_ADVANCED( "mpz_init_set_strview" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init_set_strview(big_int, str);});
        mpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "mpz_set_str" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init(big_int); std::string copy(str); mpz_set_str(big_int, copy.c_str(), 10);});
        mpz_clear(big_int);
    };
};

static fmpq naiveDecimalParse(std::string_view str){
    const size_t decimal_index = str.find('.');
    if(decimal_index == std::string::npos) return {fmpz_from_strview(str), *FMPZ_ONE};

    fmpz lead = fmpz_from_strview(str.substr(0, decimal_index));

    fmpz num = fmpz_from_strview(str.substr(decimal_index+1));
    fmpz den = 0;
    fmpz_10_pow_ui(&den, str.size()-(decimal_index+1));
    fmpq_t tail {{num, den}};
    fmpq_canonicalise(tail);

    fmpq_add_fmpz(tail, tail, &lead);
    fmpz_clear(&lead);

    return *tail;
}

static fmpq naiveScientificParse(std::string_view str){
    const size_t e_index = str.find('e');
    if(e_index == std::string::npos) return naiveDecimalParse(str);
    size_t exp_start = e_index + 1;

    const bool hasNegativePrefix = (str[exp_start] == '-');
    const bool hasPositivePrefix = (str[exp_start] == '+');

    fmpq val = naiveDecimalParse(str.substr(0, e_index));
    const auto op = hasNegativePrefix ? &fmpq_div_fmpz : &fmpq_mul_fmpz;
    exp_start += (hasNegativePrefix || hasPositivePrefix);

    fmpz exponent = fmpz_from_strview(str.substr(exp_start));
    fmpz_pow_fmpz(&exponent, FMPZ_TEN, &exponent);
    (*op)(&val, &val, &exponent);
    fmpz_clear(&exponent);

    return val;
}

TEST_CASE("fmpq_from_decimal_str (factors of 5)") {
    const std::string str = "2.125";

    BENCHMARK_ADVANCED( "fmpq_from_decimal_str" )(Catch::Benchmark::Chronometer meter) {
        fmpq big_rat;
        meter.measure([&](){big_rat = fmpq_from_decimal_str(str);});

        REQUIRE(big_rat.num == 17);
        REQUIRE(big_rat.den == 8);

        fmpq_clear(&big_rat);
    };

    BENCHMARK_ADVANCED( "naiveDecimalParse" )(Catch::Benchmark::Chronometer meter) {
        fmpq big_rat;
        meter.measure([&](){big_rat = naiveDecimalParse(str);});

        REQUIRE(big_rat.num == 17);
        REQUIRE(big_rat.den == 8);

        fmpq_clear(&big_rat);
    };
}

TEST_CASE("fmpq_from_decimal_str (factors of 2)") {
    const std::string str = "3.008";

    BENCHMARK_ADVANCED( "fmpq_from_decimal_str" )(Catch::Benchmark::Chronometer meter) {
        fmpq big_rat;
        meter.measure([&](){big_rat = fmpq_from_decimal_str(str);});

        REQUIRE(big_rat.num == 376);
        REQUIRE(big_rat.den == 125);

        fmpq_clear(&big_rat);
    };

    BENCHMARK_ADVANCED( "naiveDecimalParse" )(Catch::Benchmark::Chronometer meter) {
        fmpq big_rat;
        meter.measure([&](){big_rat = naiveDecimalParse(str);});

        REQUIRE(big_rat.num == 376);
        REQUIRE(big_rat.den == 125);

        fmpq_clear(&big_rat);
    };
}

TEST_CASE("fmpq_from_scientific_str (int result)") {
    const std::string str = "2.998e8";

    BENCHMARK_ADVANCED( "fmpq_from_scientific_str" )(Catch::Benchmark::Chronometer meter) {
        fmpq big_rat;
        meter.measure([&](){big_rat = fmpq_from_scientific_str(str);});

        REQUIRE(big_rat.num == 299800000);
        REQUIRE(big_rat.den == 1);

        fmpq_clear(&big_rat);
    };

    BENCHMARK_ADVANCED( "naiveScientificParse" )(Catch::Benchmark::Chronometer meter) {
        fmpq big_rat;
        meter.measure([&](){big_rat = naiveScientificParse(str);});

        REQUIRE(big_rat.num == 299800000);
        REQUIRE(big_rat.den == 1);

        fmpq_clear(&big_rat);
    };
}

TEST_CASE("fmpq_from_scientific_str (rational result)") {
    const std::string str = "0.0625e-2";

    BENCHMARK_ADVANCED( "fmpq_from_scientific_str" )(Catch::Benchmark::Chronometer meter) {
        fmpq big_rat;
        meter.measure([&](){big_rat = fmpq_from_scientific_str(str);});

        REQUIRE(big_rat.num == 1);
        REQUIRE(big_rat.den == 1600);

        fmpq_clear(&big_rat);
    };

    BENCHMARK_ADVANCED( "naiveScientificParse" )(Catch::Benchmark::Chronometer meter) {
        fmpq big_rat;
        meter.measure([&](){big_rat = naiveScientificParse(str);});

        REQUIRE(big_rat.num == 1);
        REQUIRE(big_rat.den == 1600);

        fmpq_clear(&big_rat);
    };
}
