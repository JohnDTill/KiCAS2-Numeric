#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include "../src/ki_cas_native_rational.h"
#include "../src/ki_cas_big_num_wrapper.h"

using namespace KiCAS2;

TEST_CASE("ckd_strdecimal2rat (tiny)") {
    const std::string str = "42.25";

    BENCHMARK_ADVANCED( "ckd_strdecimal2rat" )(Catch::Benchmark::Chronometer meter) {
        NativeRational result;
        meter.measure([&](){ckd_strdecimal2rat(&result, str);});
    };
}

TEST_CASE("ckd_strscientific2rat") {
    SECTION("Integer result"){
        const std::string str = "2.998e8";
        NativeRational result;
        REQUIRE_FALSE(ckd_strscientific2rat(&result, str));
        REQUIRE(result.num == 299800000);
        REQUIRE(result.den == 1);

        BENCHMARK_ADVANCED( "ckd_strscientific2rat (integer)" )(Catch::Benchmark::Chronometer meter) {
            meter.measure([&](){ ckd_strscientific2rat(&result, str); });
        };
    }

    SECTION("Rational result"){
        const std::string str = "25.4e-3";
        NativeRational result;
        REQUIRE_FALSE(ckd_strscientific2rat(&result, str));
        REQUIRE(result.num == 127);
        REQUIRE(result.den == 5000);

        BENCHMARK_ADVANCED( "ckd_strscientific2rat (rational)" )(Catch::Benchmark::Chronometer meter) {
            meter.measure([&](){ ckd_strscientific2rat(&result, str); });
        };
    }
}

TEST_CASE("delayed normalisation") {
    const std::pair<size_t, size_t> fibonacci_and_prime_sequences[] = {
    //    Fib.|Prime
        {   1,   1 },
        {   2,   2 },
        {   3,   3 },
        {   5,   5 },
        {   8,   7 },
        {  13,  11 },
        {  21,  13 },
        {  34,  17 },
        {  55,  19 },
        {  89,  23 },
        { 144,  29 },
        { 233,  31 },
    };

    BENCHMARK_ADVANCED( "native" )(Catch::Benchmark::Chronometer meter) {
        bool overflow = false;

        meter.measure([&](){
            NativeRational result(1, 1);

            for(const auto entry : fibonacci_and_prime_sequences)
                overflow |= ckd_mul(&result, result, NativeRational(entry.first, entry.second));
            result.reduceInPlace();
        });

        REQUIRE_FALSE(overflow);
    };

    BENCHMARK_ADVANCED( "fmpq_t" )(Catch::Benchmark::Chronometer meter) {
        meter.measure([&](){
            fmpq_t result;
            fmpq_init(result);
            fmpq_set_ui(result, 1, 1);

            for(const auto entry : fibonacci_and_prime_sequences){
                fmpq_t other;
                fmpq_init(other);
                fmpq_set_ui(other, entry.first, entry.second);
                fmpq_mul(result, result, other);
                fmpq_clear(other);
            }

            fmpq_clear(result);
        });
    };
}
