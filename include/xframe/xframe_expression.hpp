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
}

// TODO: remove that when xtensor is fixed
namespace xt
{
    namespace detail
    {
        template <>
        struct expression_tag_and<xtensor_expression_tag, xf::xvariable_expression_tag>
        {
            using type = xf::xvariable_expression_tag;
        };

        template <>
        struct expression_tag_and<xf::xvariable_expression_tag, xtensor_expression_tag>
            : expression_tag_and<xtensor_expression_tag, xf::xvariable_expression_tag>
        {
        };
    }
}

#endif
