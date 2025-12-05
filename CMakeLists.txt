cmake_minimum_required(VERSION 3.5)

# TODO: make workflows use dynamic lib
# string(FIND "${VCPKG_TARGET_TRIPLET}" "dynamic" DYNAMIC_INDEX)
# if(DYNAMIC_INDEX EQUAL -1)
#     message(FATAL_ERROR "VCPKG_TARGET_TRIPLET='${VCPKG_TARGET_TRIPLET}'; must dynamically link for LGPL compliance")
# endif()

project(ki_cas_number VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FLINT_ROOT "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
include_directories("${FLINT_ROOT}/include")
link_directories("${FLINT_ROOT}/lib")

set(SRC_FILES
    src/ki_cas_number.cpp
    src/ki_cas_number.h
)

add_library(ki_cas_number SHARED ${SRC_FILES})

target_link_libraries(ki_cas_number PRIVATE flint gmp mpfr)

set_target_properties(ki_cas_number PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(ki_cas_number PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
set_target_properties(ki_cas_number PROPERTIES PUBLIC_HEADER src/ki_cas_number.h)

install(TARGETS ki_cas_number
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Testing setup
enable_testing()
add_executable(IntegrationTests test/test_driver.cpp ${SRC_FILES})
target_include_directories(IntegrationTests PUBLIC src)
get_target_property(LIBS ki_cas_number LINK_LIBRARIES)
target_link_libraries(IntegrationTests PRIVATE ${LINK_LIBRARIES})
add_test(NAME IntegrationTests COMMAND IntegrationTests)
