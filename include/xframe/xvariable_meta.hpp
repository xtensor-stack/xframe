/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVARIABLE_META_HPP
#define XFRAME_XVARIABLE_META_HPP

#include "xcoordinate.hpp"
#include "xdimension.hpp"
#include "xvariable_scalar.hpp"

namespace xf
{

    /*********************
     * xvariable_closure *
     *********************/

    template <class CT>
    struct xvariable_closure
    {
        using type = CT;
    };

    template <class CT>
    struct xvariable_closure<xt::xscalar<CT>>
    {
        using type = xvariable_scalar<CT>;
    };

    template <class CT>
    using xvariable_closure_t = typename xvariable_closure<CT>::type;

    /***************************
     * xdecay_variable_closure *
     ***************************/

    template <class CT>
    struct xdecay_variable_closure
    {
        using type = std::decay_t<xvariable_closure_t<CT>>;
    };

    template <class CT>
    using xdecay_variable_closure_t = typename xdecay_variable_closure<CT>::type;

    /***************************
     * xcommon_coordinate_type *
     ***************************/

    namespace detail
    {
        template <class... C>
        struct xcommon_coordinate_type_impl;

        template <class C1, class C2>
        struct xcommon_coordinate_type_impl<C1, C2>
        {
            using key_type = std::common_type_t<typename C1::key_type, typename C2::key_type>;
            using label_list = xtl::mpl::merge_set_t<typename C1::label_list, typename C2::label_list>;
            using index_type = std::common_type_t<typename C1::index_type, typename C2::index_type>;
            using type = xcoordinate<key_type, label_list, index_type>;
        };

        template <class C>
        struct xcommon_coordinate_type_impl<C, xfull_coordinate>
        {
            using key_type = typename C::key_type;
            using label_list = typename C::label_list;
            using index_type = typename C::index_type;
            using type = xcoordinate<key_type, label_list, index_type>;
        };

        template <class C>
        struct xcommon_coordinate_type_impl<xfull_coordinate, C>
            : xcommon_coordinate_type_impl<C, xfull_coordinate>
        {
        };

        template <>
        struct xcommon_coordinate_type_impl<xfull_coordinate, xfull_coordinate>
        {
            using type = xfull_coordinate;
        };

        template <class C>
        struct xcommon_coordinate_type_impl<C>
            : xcommon_coordinate_type_impl<C, xfull_coordinate>
        {
        };

        template <class C1, class C2, class... C>
        struct xcommon_coordinate_type_impl<C1, C2, C...>
            : xcommon_coordinate_type_impl<C1, typename xcommon_coordinate_type_impl<C2, C...>::type>
        {
        };
    }

    template <class... CT>
    struct xcommon_coordinate_type
    {
        using type = typename detail::xcommon_coordinate_type_impl<typename xdecay_variable_closure_t<CT>::coordinate_type...>::type;
    };

    template <class... CT>
    using xcommon_coordinate_type_t = typename xcommon_coordinate_type<CT...>::type;

    /**************************
     * xcommon_dimension_type *
     **************************/

    namespace detail
    {
        template <class... D>
        struct xcommon_dimension_type_impl;

        template <class D1, class D2>
        struct xcommon_dimension_type_impl<D1, D2>
        {
            using key_type = std::common_type_t<typename D1::key_type, typename D2::key_type>;
            using mapped_type = std::common_type_t<typename D1::mapped_type, typename D2::mapped_type>;
            using type = xdimension<key_type, mapped_type>;
        };

        template <class D>
        struct xcommon_dimension_type_impl<D, xfull_coordinate>
        {
            using key_type = typename D::key_type;
            using mapped_type = typename D::mapped_type;
            using type = xdimension<key_type, mapped_type>;
        };

        template <class D>
        struct xcommon_dimension_type_impl<xfull_coordinate, D>
            : xcommon_dimension_type_impl<D, xfull_coordinate>
        {
        };

        template <>
        struct xcommon_dimension_type_impl<xfull_coordinate, xfull_coordinate>
        {
            using type = xfull_coordinate;
        };

        template <class D>
        struct xcommon_dimension_type_impl<D>
            : xcommon_dimension_type_impl<D, xfull_coordinate>
        {
        };

        template <class D1, class D2, class... D>
        struct xcommon_dimension_type_impl<D1, D2, D...>
            : xcommon_dimension_type_impl<D1, typename xcommon_dimension_type_impl<D2, D...>::type>
        {
        };
    }

    template <class... CT>
    struct xcommon_dimension_type
    {
        using type = typename detail::xcommon_dimension_type_impl<typename xdecay_variable_closure_t<CT>::dimension_type...>::type;
    };

    template <class... CT>
    using xcommon_dimension_type_t = typename xcommon_dimension_type<CT...>::type;
}

#endif
