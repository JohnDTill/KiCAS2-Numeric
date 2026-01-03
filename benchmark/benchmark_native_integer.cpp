#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include "../src/ki_cas_native_integer.h"
#include "../src/ki_cas_big_num_wrapper.h"

using namespace KiCAS2;

TEST_CASE("ckd_sqrt") {
    constexpr size_t root = 1013;
    constexpr size_t square = root * root;

    BENCHMARK_ADVANCED( "ckd_sqrt" )(Catch::Benchmark::Chronometer meter) {
        size_t ans;
        meter.measure([&](){return ckd_sqrt(&ans, square);});
        REQUIRE(ans == root);
    };

    BENCHMARK_ADVANCED( "n_sqrt" )(Catch::Benchmark::Chronometer meter) {
        size_t ans;
        meter.measure([&](){ans = n_sqrt(square); return ans*ans*ans != square;});
        REQUIRE(ans == root);
    };
};

TEST_CASE("ckd_cbrt") {
    constexpr size_t root = 1013;
    constexpr size_t cube = root * root * root;

    BENCHMARK_ADVANCED( "ckd_cbrt" )(Catch::Benchmark::Chronometer meter) {
        size_t ans;
        meter.measure([&](){return ckd_cbrt(&ans, cube);});
        REQUIRE(ans == root);
    };

    BENCHMARK_ADVANCED( "n_cbrt" )(Catch::Benchmark::Chronometer meter) {
        size_t ans;
        meter.measure([&](){ans = n_cbrt(cube); return ans*ans*ans != cube;});
        REQUIRE(ans == root);
    };

    BENCHMARK_ADVANCED( "n_cbrt_binary_search" )(Catch::Benchmark::Chronometer meter) {
        size_t ans;
        meter.measure([&](){ans = n_cbrt_binary_search(cube); return ans*ans*ans != cube;});
        REQUIRE(ans == root);
    };
};

TEST_CASE("ckd_nrt") {
    constexpr size_t root = 7;
    constexpr size_t power = 11;
    constexpr size_t raised = 1977326743uLL;

    BENCHMARK_ADVANCED( "ckd_nrt" )(Catch::Benchmark::Chronometer meter) {
        size_t ans;
        meter.measure([&](){return ckd_nrt(&ans, raised, power);});
        REQUIRE(ans == root);
    };
};

TEST_CASE("knownfit_pow") {
    BENCHMARK_ADVANCED( "ckd_pow_helper" )(Catch::Benchmark::Chronometer meter) {
        size_t ans;
        bool overflowed;
        meter.measure([&](){overflowed = ckd_pow(&ans, 7, 11);});
        REQUIRE(overflowed == false);
        REQUIRE(ans == 1977326743uLL);
    };

    BENCHMARK_ADVANCED( "knownfit_pow" )(Catch::Benchmark::Chronometer meter) {
        size_t ans;
        meter.measure([&](){ans = knownfit_pow(7, 11);});
        REQUIRE(ans == 1977326743uLL);
    };

    BENCHMARK_ADVANCED( "n_pow" )(Catch::Benchmark::Chronometer meter) {
        size_t ans;
        meter.measure([&](){ans = n_pow(7, 11);});
        REQUIRE(ans == 1977326743uLL);
    };
};
