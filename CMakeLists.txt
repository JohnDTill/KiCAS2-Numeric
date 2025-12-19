cmake_minimum_required(VERSION 3.5)

project(ki_cas_number VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FLINT_ROOT "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
include_directories("${FLINT_ROOT}/include")
link_directories("${FLINT_ROOT}/lib")

set(SRC_FILES
    src/ki_cas_integer_math.h
    src/ki_cas_integer_math.cpp
    src/ki_cas_number.cpp
    src/ki_cas_number.h
)

add_library(ki_cas_number SHARED ${SRC_FILES})

foreach(X Flint::flint gmp mpfr)
    add_library(${X} SHARED IMPORTED)
    get_target_property(is_shared ${X} TYPE)
    if(NOT is_shared STREQUAL "SHARED_LIBRARY")
        message(FATAL_ERROR "Must dynamically link ${X} for LGPL compliance with Forscape's license")
    endif()
endforeach()

set_target_properties(ki_cas_number PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(ki_cas_number PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
set_target_properties(ki_cas_number PROPERTIES PUBLIC_HEADER src/ki_cas_number.h)

install(TARGETS ki_cas_number
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Testing setup
find_package(Catch2 CONFIG REQUIRED)

enable_testing()
add_executable(IntegrationTests ${SRC_FILES}
    test/test_integer_math.cpp)
target_include_directories(IntegrationTests PUBLIC src)
get_target_property(LIBS ki_cas_number LINK_LIBRARIES)
target_link_libraries(IntegrationTests PRIVATE ${LINK_LIBRARIES})
target_link_libraries(IntegrationTests PRIVATE Catch2::Catch2WithMain)
add_test(NAME IntegrationTests COMMAND IntegrationTests)
