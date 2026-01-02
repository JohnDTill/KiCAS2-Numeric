cmake_minimum_required(VERSION 3.5)

project(ki_cas_numeric VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FLINT_ROOT "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
include_directories("${FLINT_ROOT}/include")
link_directories("${FLINT_ROOT}/lib")

find_package(PkgConfig REQUIRED)
pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp mpfr)
find_package(Threads REQUIRED)

set(SRC_FILES
    src/ki_cas_big_num_wrapper.cpp
    src/ki_cas_big_num_wrapper.h
    src/ki_cas_native_float.cpp
    src/ki_cas_native_float.h
    src/ki_cas_native_integer.cpp
    src/ki_cas_native_integer.h
    src/ki_cas_native_rational.h
    src/ki_cas_native_rational.cpp
    src/ki_cas_str_conversions.cpp
    src/ki_cas_str_conversions.h
    src/ki_cas_typesetting_flags.h
)

add_library(ki_cas_numeric_lib SHARED ${SRC_FILES})

foreach(X Flint::flint gmp mpfr)
    add_library(${X} SHARED IMPORTED)
    get_target_property(is_shared ${X} TYPE)
    if(NOT is_shared STREQUAL "SHARED_LIBRARY")
        message(FATAL_ERROR "Must dynamically link ${X} for LGPL compliance with permissive licenses")
    endif()
endforeach()
target_link_libraries(ki_cas_numeric_lib PkgConfig::gmp flint)
target_link_libraries(ki_cas_numeric_lib Threads::Threads)

set_target_properties(ki_cas_numeric_lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(ki_cas_numeric_lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
set_target_properties(ki_cas_numeric_lib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Testing setup
find_package(Catch2 CONFIG REQUIRED)

enable_testing()
add_executable(Tests
    test/test_big_num_wrapper.cpp
    test/test_native_float.cpp
    test/test_native_integer.cpp
    test/test_native_rational.cpp
    test/test_str_conversions.cpp)
target_include_directories(Tests PUBLIC src)
target_link_libraries(Tests PRIVATE ki_cas_numeric_lib Catch2::Catch2WithMain)
add_test(NAME Tests COMMAND Tests)

# Benchmark setup
add_executable(Benchmarks
    benchmark/benchmark_big_num_wrapper.cpp
    benchmark/benchmark_native_rational.cpp)
set_property(TARGET Benchmarks PROPERTY INTERPROCEDURAL_OPTIMIZATION OFF)
target_include_directories(Benchmarks PUBLIC src)
target_link_libraries(Benchmarks PRIVATE ki_cas_numeric_lib Catch2::Catch2WithMain)
add_test(NAME Benchmarks COMMAND Benchmarks)
