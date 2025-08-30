cmake_minimum_required(VERSION 3.25)
project(infinite)

set(CMAKE_CXX_STANDARD 20)

add_library(infinite
    inc/infinite_state.h
    src/infinite_state.c
    inc/infinite_state_machine.h
    src/infinite_state_machine.c
    inc/infinite_state_machine.hpp
    src/infinite_state_machine.cpp
)

# Set the include directories for the library.
target_include_directories(infinite
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:include>
)

# Add a CTest executable for running all tests.
# This will be used to run the tests defined in the test sources.
# The test sources will be compiled into a test executable.
include(CTest)
create_test_sourcelist(test_sources
    test_runner.c
    test/abc.cpp
    test/def.c
)

# Add a test executable that links against the library.
# The test executable will be built from the test sources.
# It will include the library and any necessary test files.
add_executable(test_runner
    ${test_sources}
)
target_link_libraries(test_runner PRIVATE infinite)

add_test(NAME abc COMMAND test_runner test/abc)
add_test(NAME def COMMAND test_runner test/def)

# CPack configuration for packaging.
install(TARGETS infinite ARCHIVE DESTINATION lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/inc/infinite_state_machine.hpp
        DESTINATION include)
set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_NAME "infinite")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION "Infinite State Machine Library")
set(CPACK_PACKAGE_VENDOR "Roy Ratcliffe")
set(CPACK_PACKAGE_CONTACT "Roy Ratcliffe <roy@ratcliffe.me>")
set(CPACK_INSTALL_PREFIX "/usr/local")
include(CPack)

# https://cmake.org/cmake/help/latest/module/FindDoxygen.html
# https://www.mcternan.me.uk/mscgen/
find_package(Doxygen OPTIONAL_COMPONENTS dot mscgen dia)
if(DOXYGEN_FOUND)
    set(OPTIMIZE_OUTPUT_FOR_CXX YES)
    set(DOXYGEN_SOURCE_BROWSER YES)
    doxygen_add_docs(doxy ${PROJECT_SOURCE_DIR}/inc ${PROJECT_SOURCE_DIR}/src)
endif()
