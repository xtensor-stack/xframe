/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVARIABLE_MATH_HPP
#define XFRAME_XVARIABLE_MATH_HPP

#include "xtensor/xconcepts.hpp"
#include "xtensor/xmath.hpp"
#include "xframe_expression.hpp"
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

namespace xf
{
    using xt::operator+;
    using xt::operator-;
    using xt::operator*;
    using xt::operator/;
    using xt::operator%;
    
    using xt::operator||;
    using xt::operator&&;
    using xt::operator!;

    using xt::operator&;
    using xt::operator|;
    using xt::operator^;
    using xt::operator~;

    using xt::operator!=;
    using xt::operator<;
    using xt::operator<=;
    using xt::operator>;
    using xt::operator>=;

    using xt::equal;
    using xt::not_equal;

    using xt::abs;
    using xt::fabs;
    using xt::fmod;
    using xt::remainder;
    // Needs a bug fix in xtl
    //using xt::fma;
    using xt::fmax;
    using xt::fmin;
    using xt::fdim;
    // Needs a bug fix in xtl
    //using xt::maximum;
    //using xt::minimum;
    // Needs a bug fix in xtensor
    //using xt::clip;
    using xt::sign;

    using xt::exp;
    using xt::exp2;
    using xt::expm1;
    using xt::log;
    using xt::log10;
    using xt::log2;
    using xt::log1p;

    using xt::pow;
    using xt::sqrt;
    using xt::cbrt;
    using xt::hypot;

    using xt::sin;
    using xt::cos;
    using xt::tan;
    using xt::asin;
    using xt::acos;
    using xt::atan;
    using xt::atan2;

    using xt::sinh;
    using xt::cosh;
    using xt::tanh;
    using xt::asinh;
    using xt::acosh;
    using xt::atanh;

    using xt::erf;
    using xt::erfc;
    using xt::tgamma;
    using xt::lgamma;
}

#endif

