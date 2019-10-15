############################################################################
# Copyright (c) Johan Mabille and Sylvain Corlay                           #
# Copyright (c) QuantStack                                                 #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

cmake_minimum_required(VERSION 3.1)
project(xframe)

set(XFRAME_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Versionning
# ===========

file(STRINGS "${XFRAME_INCLUDE_DIR}/xframe/xframe_config.hpp" xframe_version_defines
    REGEX "#define XFRAME_VERSION_(MAJOR|MINOR|PATCH)")
foreach(ver ${xframe_version_defines})
    if(ver MATCHES "#define XFRAME_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(XFRAME_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif()
endforeach()
set(${PROJECT_NAME}_VERSION
    ${XFRAME_VERSION_MAJOR}.${XFRAME_VERSION_MINOR}.${XFRAME_VERSION_PATCH})
message(STATUS "xframe v${${PROJECT_NAME}_VERSION}")

# Dependencies
# ============

find_package(xtensor REQUIRED)
include_directories(${xtensor_INCLUDE_DIRS})

find_package(xtl REQUIRED)
include_directories(${xtl_INCLUDE_DIRS})

# Build
# =====

set(XFRAME_HEADERS
    ${XFRAME_INCLUDE_DIR}/xframe/xaxis.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xaxis_base.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xaxis_default.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xaxis_expression_leaf.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xaxis_function.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xaxis_index_slice.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xaxis_label_slice.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xaxis_math.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xaxis_meta.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xaxis_scalar.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xaxis_variant.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xaxis_view.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xcoordinate.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xcoordinate_base.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xcoordinate_chain.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xcoordinate_expanded.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xcoordinate_system.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xcoordinate_view.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xdimension.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xdynamic_variable_impl.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xdynamic_variable.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xexpand_dims_view.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xframe_config.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xframe_expression.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xframe_trace.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xframe_utils.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xio.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xnamed_axis.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xreindex_view.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xreindex_data.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xselecting.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xsequence_view.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xvariable.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xvariable_assign.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xvariable_base.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xvariable_function.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xvariable_masked_view.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xvariable_math.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xvariable_meta.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xvariable_scalar.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xvariable_view.hpp
    ${XFRAME_INCLUDE_DIR}/xframe/xvector_variant.hpp
)

OPTION(BUILD_TESTS "xframe test suite" OFF)
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

install(FILES ${XFRAME_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xframe)

set(XFRAME_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE
    STRING "install path for xframeConfig.cmake")

configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${XFRAME_CMAKECONFIG_INSTALL_DIR})

# xframe is header-only and does not depend on the architecture.
# Remove CMAKE_SIZEOF_VOID_P from xframeConfigVersion.cmake so that an xframeConfig.cmake
# generated for a 64 bit target can be used for 32 bit targets and vice versa.
set(_XFRAME_CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P})
unset(CMAKE_SIZEOF_VOID_P)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
                                 VERSION ${${PROJECT_NAME}_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
set(CMAKE_SIZEOF_VOID_P ${_XFRAME_CMAKE_SIZEOF_VOID_P})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${XFRAME_CMAKECONFIG_INSTALL_DIR})


configure_file(${PROJECT_NAME}.pc.in
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
        @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig/")
