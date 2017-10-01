/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_MATH_HPP
#define XFRAME_MATH_HPP

#include "xtensor/xmath.hpp"
#include "xseries_function.hpp"

namespace xt
{
    namespace detail
    {
        template <template <class...> class F, class... E>
        struct build_functor_type<xf::xframe_expression_tag, F, E...>
        {
            using type = F<common_value_type_t<typename std::decay_t<E>::expression_type...>>;
            //using type = F<double>;
        };

        template <class F, class... E>
        struct select_xfunction_expression<xf::xframe_expression_tag, F, E...>
        {
            using type = xf::xseries_function<F, typename F::result_type, E...>;
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
