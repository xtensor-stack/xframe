/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVARIABLE_MATH_HPP
#define XFRAME_XVARIABLE_MATH_HPP

#include "xframe_expression.hpp"
#include "xaxis_math.hpp"
#include "xvariable_function.hpp"

namespace xt
{
    namespace detail
    {
        template <class F, class... E>
        struct select_xfunction_expression<xf::xvariable_expression_tag, F, E...>
        {
            using result_type = decltype(std::declval<F>()(std::declval<xvalue_type_t<std::decay_t<E>>>()...));
            using type = xf::xvariable_function<F, result_type, E...>;
        };
    }

    template <class E1, class E2>
    inline std::enable_if_t<xf::xvariable_comparable<E1, E2>::value, bool>
    operator==(const xexpression<E1>& e1, const xexpression<E2>& e2)
    {
        const E1& de1 = e1.derived_cast();
        const E2& de2 = e2.derived_cast();

        return de1.data() == de2.data() &&
            de1.coordinates() == de2.coordinates() &&
            de1.dimension_mapping() == de2.dimension_mapping();
    }
}

#endif
