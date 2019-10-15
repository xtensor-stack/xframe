/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XFRAME_TRACE_HPP
#define XFRAME_XFRAME_TRACE_HPP

#include <string>

#include "xframe_config.hpp"

namespace xf
{

#if XFRAME_ENABLE_TRACE

#define XFRAME_TRACE(expr) XFRAME_OUT << expr << std::endl;

#define XFRAME_TRACE_BROADCAST_DIMENSIONS(lhs, rhs)             \
    XFRAME_TRACE("BROADCASTING DIMENSIONS");                    \
    XFRAME_TRACE("lhs = " << lhs);                              \
    XFRAME_TRACE("rhs = " << rhs);

#define XFRAME_TRACE_DIMENSIONS_RESULT(res, trivial)            \
    XFRAME_TRACE("res = " << res);                              \
    XFRAME_TRACE("trivial broadcasting = " << trivial);

#define XFRAME_TRACE_BROADCAST_COORDINATES(lhs, rhs)            \
    XFRAME_TRACE("BROADCASTING COORDINATES");                   \
    XFRAME_TRACE("lhs:")                                        \
    XFRAME_OUT << lhs;                                          \
    XFRAME_TRACE("rhs:")                                        \
    XFRAME_OUT << rhs;

#define XFRAME_TRACE_COORDINATES_RESULT(res, trivial)           \
    XFRAME_TRACE("res:")                                        \
    XFRAME_OUT << res;                                          \
    XFRAME_TRACE("trivial broadcasting = (" << trivial.m_same_dimensions << ", " << trivial.m_same_labels << ")");
#else
#define XFRAME_TRACE(expr)
#define XFRAME_TRACE_BROADCAST_DIMENSIONS(lhs, rhs)
#define XFRAME_TRACE_DIMENSIONS_RESULT(res, trivial)
#define XFRAME_TRACE_BROADCAST_COORDINATES(lhs, rhs)
#define XFRAME_TRACE_COORDINATES_RESULT(res, trivial)
#endif

}

#endif
