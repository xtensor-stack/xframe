/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
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
        template <class... D>
        struct xcommon_coordinate_dimension;

        template <class D1, class D2>
        struct xcommon_coordinate_dimension<D1, D2>
        {
            using key_type = std::common_type_t<typename D1::key_type, typename D2::key_type>;
            using mapped_type = std::common_type_t<typename D1::mapped_type, typename D2::mapped_type>;
            using coordinate_type = xcoordinate<key_type, mapped_type>;
            using dimension_type = xdimension<key_type, mapped_type>;
        };

        template <class D>
        struct xcommon_coordinate_dimension<D, xfull_coordinate>
        {
            using key_type = typename D::key_type;
            using mapped_type = typename D::mapped_type;
            using coordinate_type = xcoordinate<key_type, mapped_type>;
            using dimension_type = xdimension<key_type, mapped_type>;
        };

        template <class D>
        struct xcommon_coordinate_dimension<xfull_coordinate, D>
            : xcommon_coordinate_dimension<D, xfull_coordinate>
        {
        };

        template <>
        struct xcommon_coordinate_dimension<xfull_coordinate, xfull_coordinate>
        {
            using coordinate_type = xfull_coordinate;
            using dimension_type = xfull_coordinate;
        };

        template <class D>
        struct xcommon_coordinate_dimension<D>
            : xcommon_coordinate_dimension<D, xfull_coordinate>
        {
        };

        template <class D1, class D2, class... D>
        struct xcommon_coordinate_dimension<D1, D2, D...>
            : xcommon_coordinate_dimension<D1, typename xcommon_coordinate_dimension<D2, D...>::dimension_type>
        {
        };
    }

    template <class... CT>
    struct xcommon_coordinate_type
    {
        using type = typename detail::xcommon_coordinate_dimension<typename xdecay_variable_closure_t<CT>::dimension_type...>::coordinate_type;
    };

    template <class... CT>
    using xcommon_coordinate_type_t = typename xcommon_coordinate_type<CT...>::type;

    template <class... CT>
    struct xcommon_dimension_type
    {
        using type = typename detail::xcommon_coordinate_dimension<typename xdecay_variable_closure_t<CT>::dimension_type...>::dimension_type;
    };

    template <class... CT>
    using xcommon_dimension_type_t = typename xcommon_dimension_type<CT...>::type;
}

#endif
