/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XFRAME_EXPRESSION_HPP
#define XFRAME_XFRAME_EXPRESSION_HPP

#include "xtensor/xexpression.hpp"
#include "xtl/xtype_traits.hpp"

namespace xf
{
    struct xvariable_expression_tag {};
    struct xframe_expression_tag {};

    template <class E>
    struct is_xvariable_expression : std::is_same<xt::xexpression_tag_t<E>, xvariable_expression_tag>
    {
    };

    template <class... E>
    struct xvariable_comparable : xtl::conjunction<is_xvariable_expression<E>...>
    {
    };

    struct xaxis_expression_tag {};

    template <class E>
    struct is_xaxis_expression : std::is_same<xt::xexpression_tag_t<E>, xaxis_expression_tag>
    {
    };

    template <class... E>
    struct xaxis_comparable : xtl::conjunction<is_xaxis_expression<E>...>
    {
    };
}

#endif
