cmake_minimum_required(VERSION 3.5)

project(ki_cas_number VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FLINT_ROOT "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
include_directories("${FLINT_ROOT}/include")
link_directories("${FLINT_ROOT}/lib")

find_package(PkgConfig REQUIRED)
pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)

set(SRC_FILES
    src/ki_cas_big_num_wrapper.cpp
    src/ki_cas_big_num_wrapper.h
    src/ki_cas_float.h
    src/ki_cas_integer_math.cpp
    src/ki_cas_integer_math.h
    src/ki_cas_native_rational.h
    src/ki_cas_native_rational.cpp
    src/ki_cas_number.cpp
    src/ki_cas_number.h
    src/ki_cas_str_conversions.cpp
    src/ki_cas_str_conversions.h
)

add_library(ki_cas_number SHARED ${SRC_FILES})

foreach(X Flint::flint gmp mpfr)
    add_library(${X} SHARED IMPORTED)
    get_target_property(is_shared ${X} TYPE)
    if(NOT is_shared STREQUAL "SHARED_LIBRARY")
        message(FATAL_ERROR "Must dynamically link ${X} for LGPL compliance with Forscape's license")
    endif()
endforeach()
target_link_libraries(ki_cas_number PkgConfig::gmp flint)

set_target_properties(ki_cas_number PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(ki_cas_number PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
set_target_properties(ki_cas_number PROPERTIES PUBLIC_HEADER src/ki_cas_number.h)

set_target_properties(ki_cas_number PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Testing setup
find_package(Catch2 CONFIG REQUIRED)

enable_testing()
add_executable(IntegrationTests
    test/test_big_num_wrapper.cpp
    test/test_integer_math.cpp
    test/test_native_rational.cpp
    test/test_number.cpp
    test/test_str_conversions.cpp)
target_include_directories(IntegrationTests PUBLIC src)
target_link_libraries(IntegrationTests PRIVATE ki_cas_number Catch2::Catch2WithMain)
add_test(NAME IntegrationTests COMMAND IntegrationTests)
