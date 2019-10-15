/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_META_HPP
#define XFRAME_XAXIS_META_HPP

#include "xtl/xbasic_fixed_string.hpp"
#include "xtl/xvariant.hpp"
#include "xtl/xmeta_utils.hpp"

#include "xframe_config.hpp"
#include "xaxis_scalar.hpp"

namespace xf
{
    template <class K, class T, class MT, class L, class LT>
    class xnamed_axis;

    template <class CTA>
    class xaxis_expression_leaf;

    namespace detail
    {

        template <class... Args>
        struct common_name_type;

        template <class T, class... Args>
        struct common_name_type<T, Args...>
        {
            using tmp_type = typename common_name_type<Args...>::type;
            using type = typename common_name_type<T, tmp_type>::type;
        };

        template <class T1, class T2>
        struct common_name_type<T1, xaxis_scalar<T2>>
        {
            using type = typename T1::name_type;
        };

        template <class T1, class T2>
        struct common_name_type<xaxis_scalar<T1>, T2>
        {
            using type = typename T2::name_type;
        };

        template <class T1, class T2>
        struct common_name_type<xaxis_scalar<T1>, xaxis_scalar<T2>>
        {
            using type = xaxis_scalar_name;
        };

        template <class T1, class T2>
        struct common_name_type<T1, T2>
        {
            using type = std::common_type_t<typename T1::name_type, typename T2::name_type>;
        };

        template <class... Args>
        using common_name_type_t = typename common_name_type<Args...>::type;

        template <class CTA, bool val>
        struct axis_closure_getter
        {
            static auto get_axis_closure(CTA&& axis)
            {
                return xaxis_expression_leaf<CTA>(std::forward<CTA>(axis));
            }
        };

        template <class CTA>
        struct axis_closure_getter<CTA, false>
        {
            static auto get_axis_closure(CTA&& axis)
            {
                return std::forward<CTA>(axis);
            }
        };

        template <class T>
        struct is_named_axis : std::false_type
        {
        };

        template <class K, class T, class MT, class L, class LT>
        struct is_named_axis<xnamed_axis<K, T, MT, L, LT>> : std::true_type
        {
        };

        template <class CTA>
        auto get_axis_closure(CTA&& axis)
        {
            return axis_closure_getter<CTA, is_named_axis<std::decay_t<CTA>>::value>::get_axis_closure(std::forward<CTA>(axis));
        }
    }

    /*****************
     * xaxis_closure *
     *****************/

    template <class CT>
    struct xaxis_closure
    {
        using type = CT;
    };

    template <class CT>
    using xaxis_closure_t = typename xaxis_closure<CT>::type;

    /****************************
     * xaxis_expression_closure *
     ****************************/

    template <class CT, bool val>
    struct axis_expression_closure_getter
    {
        using type = xaxis_expression_leaf<CT>;
    };

    template <class CT>
    struct axis_expression_closure_getter<CT, false>
    {
        using type = CT;
    };

    template <class CT>
    struct axis_expression_closure_getter<xt::xscalar<CT>, false>
    {
        using type = xaxis_scalar<CT>;
    };

    template <class CT>
    struct xaxis_expression_closure
    {
        using type = typename axis_expression_closure_getter<CT, detail::is_named_axis<std::decay_t<CT>>::value>::type;
    };

    template <class CT>
    using xaxis_expression_closure_t = typename xaxis_expression_closure<CT>::type;
}

#endif
