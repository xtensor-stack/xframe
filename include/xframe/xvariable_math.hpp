/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVARIABLE_MATH_HPP
#define XFRAME_XVARIABLE_MATH_HPP

#include "xtensor/xmath.hpp"
#include "xvariable_function.hpp"

namespace xt
{
    namespace detail
    {
        template <template <class...> class F, class... E>
        struct build_functor_type<xf::xvariable_expression_tag, F, E...>
        {
            using type = F<common_value_type_t<typename std::decay_t<E>...>>;
        };

        template <class F, class... E>
        struct select_xfunction_expression<xf::xvariable_expression_tag, F, E...>
        {
            using type = xf::xvariable_function<F, typename F::result_type, E...>;
        };
    }
}

namespace xf
{
    using xt::operator+;
    using xt::operator-;
    using xt::operator*;
    using xt::operator/;
}

#endif

