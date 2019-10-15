/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_MATH_HPP
#define XFRAME_XAXIS_MATH_HPP

#include "xtensor/xmath.hpp"

#include "xframe_expression.hpp"
#include "xaxis_function.hpp"

namespace xt
{
    namespace detail
    {
        template <class F, class... E>
        struct select_xfunction_expression<xf::xaxis_expression_tag, F, E...>
        {
            using result_type = decltype(std::declval<F>()(std::declval<xvalue_type_t<std::decay_t<E>>>()...));
            using type = xf::xaxis_function<F, result_type, E...>;
        };
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
    using xt::fma;
    using xt::fmax;
    using xt::fmin;
    using xt::fdim;
    using xt::maximum;
    using xt::minimum;
    using xt::clip;
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

    using xt::ceil;
    using xt::floor;
    using xt::trunc;
    using xt::round;
    using xt::nearbyint;
    using xt::rint;

    using xt::isfinite;
    using xt::isinf;
    using xt::isnan;
    // Needs a fix in xtensor
    /*using xt::isclose;
    using xt::allclose;*/

    using xt::where;
}

#endif
