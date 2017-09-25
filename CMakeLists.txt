############################################################################
# Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

cmake_minimum_required(VERSION 3.1)
project(xdataframe)

set(XDATAFRAME_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Versionning
# ===========

file(STRINGS "${XDATAFRAME_INCLUDE_DIR}/xdataframe/xdataframe_config.hpp" xdataframe_version_defines
    REGEX "#define XDATAFRAME_VERSION_(MAJOR|MINOR|PATCH)")
foreach(ver ${xdataframe_version_defines})
    if(ver MATCHES "#define XDATAFRAME_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(XDATAFRAME_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif()
endforeach()
set(${PROJECT_NAME}_VERSION 
    ${XDATAFRAME_VERSION_MAJOR}.${XDATAFRAME_VERSION_MINOR}.${XDATAFRAME_VERSION_PATCH})
message(STATUS "xdataframe v${${PROJECT_NAME}_VERSION}")

# Dependencies
# ============

find_package(xtensor REQUIRED)
include_directories(${xtensor_INCLUDE_DIRS})

find_package(xtl REQUIRED)
include_directories(${xtl_INCLUDE_DIRS})

# Build
# =====

set(XDATAFRAME_HEADERS
    ${XDATAFRAME_INCLUDE_DIR}/xdataframe/xaxis.hpp
    ${XDATAFRAME_INCLUDE_DIR}/xdataframe/xdataframe_config.hpp
)

OPTION(BUILD_TESTS "xdataframe test suite" OFF)
OPTION(DOWNLOAD_GTEST "build gtest from downloaded sources" OFF)

if(DOWNLOAD_GTEST OR GTEST_SRC_DIR)
    set(BUILD_TESTS ON)
endif()

if(BUILD_TESTS)
    add_subdirectory(test)
endif()

# Installation
# ============

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(FILES ${XDATAFRAME_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xdataframe)

    set(XDATAFRAME_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE
    STRING "install path for xdataframeConfig.cmake")

configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${XDATAFRAME_CMAKECONFIG_INSTALL_DIR})

# xdataframe is header-only and does not depend on the architecture.
# Remove CMAKE_SIZEOF_VOID_P from xdataframeConfigVersion.cmake so that an xdataframeConfig.cmake
# generated for a 64 bit target can be used for 32 bit targets and vice versa.
set(_XDATAFRAME_CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P})
unset(CMAKE_SIZEOF_VOID_P)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
                                 VERSION ${${PROJECT_NAME}_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
set(CMAKE_SIZEOF_VOID_P ${_XDATAFRAME_CMAKE_SIZEOF_VOID_P})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${XDATAFRAME_CMAKECONFIG_INSTALL_DIR})
