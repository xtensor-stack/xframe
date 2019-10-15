############################################################################
# Copyright (c) Johan Mabille and Sylvain Corlay                           #
# Copyright (c) QuantStack                                                 #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

cmake_minimum_required(VERSION 3.1)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(xframe-test)

    find_package(xframe REQUIRED CONFIG)
    set(XFRAME_INCLUDE_DIR ${xframe_INCLUDE_DIRS})
endif ()

message(STATUS "Forcing tests build type to Release")
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

include(CheckCXXCompilerFlag)

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wunused-parameter -Wextra -Wreorder -Wconversion -Wsign-conversion")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wunused-parameter -Wextra -Wreorder -Wconversion")
    CHECK_CXX_COMPILER_FLAG("-std=c++14" HAS_CPP14_FLAG)

    if (HAS_CPP14_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    else()
        message(FATAL_ERROR "Unsupported compiler -- xframe requires C++14 support!")
    endif()
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /MP /bigobj")
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

if(DOWNLOAD_GTEST OR GTEST_SRC_DIR)
    if(DOWNLOAD_GTEST)
        # Download and unpack googletest at configure time
        configure_file(downloadGTest.cmake.in googletest-download/CMakeLists.txt)
    else()
        # Copy local source of googletest at configure time
        configure_file(copyGTest.cmake.in googletest-download/CMakeLists.txt)
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif()

    # Add googletest directly to our build. This defines
    # the gtest and gtest_main targets.
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                     ${CMAKE_CURRENT_BINARY_DIR}/googletest-build EXCLUDE_FROM_ALL)

    set(GTEST_INCLUDE_DIRS "${gtest_SOURCE_DIR}/include")
    set(GTEST_BOTH_LIBRARIES  gtest_main gtest)
else()
    find_package(GTest REQUIRED)
endif()

find_package(Threads)

include_directories(${XFRAME_INCLUDE_DIR})
include_directories(${GTEST_INCLUDE_DIRS})

set(XFRAME_TESTS
    main.cpp
    test_fixture.hpp
    test_fixture_view.hpp
    test_xaxis.cpp
    test_xaxis_default.cpp
    test_xaxis_function.cpp
    test_xaxis_variant.cpp
    test_xaxis_view.cpp
    test_xcoordinate.cpp
    test_xcoordinate_chain.cpp
    test_xcoordinate_expanded.cpp
    test_xcoordinate_view.cpp
    test_xdimension.cpp
    test_xdynamic_variable.cpp
    test_xexpand_dims_view.cpp
    test_xframe_utils.cpp
    test_xnamed_axis.cpp
    test_xreindex_view.cpp
    test_xsequence_view.cpp
    test_xvariable.cpp
    test_xvariable_assign.cpp
    test_xvariable_function.cpp
    test_xvariable_masked_view.cpp
    test_xvariable_math.cpp
    test_xvariable_noalias.cpp
    test_xvariable_scalar.cpp
    test_xvariable_view.cpp
    test_xvariable_view_assign.cpp
    test_xvector_variant.cpp
)

add_executable(test_xframe ${XFRAME_TESTS} ${XFRAME_HEADERS})
if(DOWNLOAD_GTEST OR GTEST_SRC_DIR)
    add_dependencies(test_xframe gtest_main)
endif()
target_link_libraries(test_xframe ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(test_xframe PRIVATE ${XFRAME_INCLUDE_DIR})

add_custom_target(xtest COMMAND test_xframe DEPENDS test_xframe)
