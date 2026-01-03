#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include "../src/ki_cas_big_num_wrapper.h"

using namespace KiCAS2;

TEST_CASE("fmpz_init_set_strview (tiny)") {
    std::string str = "1337";

    BENCHMARK_ADVANCED( "fmpz_init_set_strview" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init_set_strview(big_int, str);});
        fmpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "fmpz_init_set_mutable_str" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init_set_mutable_str(big_int, str, 0, 4);});
        fmpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "fmpz_set_str" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init(big_int); fmpz_set_str(big_int, str.data(), 10);});
        fmpz_clear(big_int);
    };
};

TEST_CASE("fmpz_init_set_strview (Flint word size)") {
    std::string str = std::to_string(COEFF_MAX);

    BENCHMARK_ADVANCED( "fmpz_init_set_strview" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init_set_strview(big_int, str);});
        fmpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "fmpz_init_set_strview<ALLOW_STRING_MODIFICATION>" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init_set_mutable_str(big_int, str, 0, str.size());});
        fmpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "fmpz_set_str" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init(big_int); fmpz_set_str(big_int, str.data(), 10);});
        fmpz_clear(big_int);
    };
};

TEST_CASE("fmpz_init_set_strview (Flint word size x2)") {
    std::string str = std::to_string(std::numeric_limits<size_t>::max()) + "00";

    BENCHMARK_ADVANCED( "fmpz_init_set_strview" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init_set_strview(big_int, str);});
        fmpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "fmpz_init_set_strview<ALLOW_STRING_MODIFICATION>" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init_set_mutable_str(big_int, str, 0, str.size());});
        fmpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "fmpz_set_str" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init(big_int); fmpz_set_str(big_int, str.data(), 10);});
        fmpz_clear(big_int);
    };
};

TEST_CASE("fmpz_init_set_strview (Flint word size x3)") {
    std::string str = std::to_string(std::numeric_limits<size_t>::max())
                    + std::to_string(std::numeric_limits<size_t>::max())
                    + "0000";

    BENCHMARK_ADVANCED( "fmpz_init_set_strview" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init_set_strview(big_int, str);});
        fmpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "fmpz_init_set_strview<ALLOW_STRING_MODIFICATION>" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init_set_mutable_str(big_int, str, 0, str.size());});
        fmpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "fmpz_set_str" )(Catch::Benchmark::Chronometer meter) {
        fmpz_t big_int;
        meter.measure([&](){fmpz_init(big_int); fmpz_set_str(big_int, str.data(), 10);});
        fmpz_clear(big_int);
    };
};

TEST_CASE("mpz_init_set_strview (tiny)") {
    std::string str = "1337";

    BENCHMARK_ADVANCED( "mpz_init_set_strview" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init_set_strview(big_int, str);});
        mpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "mpz_init_set_strview<ALLOW_STRING_MODIFICATION>" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init_set_mutable_str(big_int, str, 0, 4);});
        mpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "mpz_set_str" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init(big_int); mpz_set_str(big_int, str.data(), 10);});
        mpz_clear(big_int);
    };
};

TEST_CASE("mpz_init_set_strview (word size)") {
    std::string str = std::to_string(COEFF_MAX);
    constexpr size_t max = std::numeric_limits<size_t>::max();

    BENCHMARK_ADVANCED( "mpz_init_set_strview" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init_set_strview(big_int, str);});
        mpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "mpz_init_set_strview<ALLOW_STRING_MODIFICATION>" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init_set_mutable_str(big_int, str, 0, str.size());});
        mpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "mpz_set_str" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init(big_int); mpz_set_str(big_int, str.data(), 10);});
        mpz_clear(big_int);
    };
};

TEST_CASE("mpz_init_set_strview (Flint word size x2)") {
    std::string str = std::to_string(std::numeric_limits<size_t>::max()) + "00";

    BENCHMARK_ADVANCED( "mpz_init_set_strview" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init_set_strview(big_int, str);});
        mpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "mpz_init_set_strview<ALLOW_STRING_MODIFICATION>" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init_set_mutable_str(big_int, str, 0, str.size());});
        mpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "mpz_set_str" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init(big_int); mpz_set_str(big_int, str.data(), 10);});
        mpz_clear(big_int);
    };
};

TEST_CASE("mpz_init_set_strview (Flint word size x3)") {
    std::string str = std::to_string(std::numeric_limits<size_t>::max())
                    + std::to_string(std::numeric_limits<size_t>::max())
                    + "0000";

    BENCHMARK_ADVANCED( "mpz_init_set_strview" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init_set_strview(big_int, str);});
        mpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "mpz_init_set_strview<ALLOW_STRING_MODIFICATION>" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init_set_mutable_str(big_int, str, 0, str.size());});
        mpz_clear(big_int);
    };

    BENCHMARK_ADVANCED( "mpz_set_str" )(Catch::Benchmark::Chronometer meter) {
        mpz_t big_int;
        meter.measure([&](){mpz_init(big_int); mpz_set_str(big_int, str.data(), 10);});
        mpz_clear(big_int);
    };
};
