/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVARIABLE_VARIANT_HPP
#define XFRAME_XVARIABLE_VARIANT_HPP

#include "xvariable.hpp"

namespace xf
{


    template <class... V>
    class xvariable_variant
    {
    };

    // vtype1 = variable<double, coord_type1>;
    // vtype2 variable<int, coord_type2>;
    // vtype1 v1;
    // vtype2 v2;

    // auto fr = ("n1", v1) | ("n2", v2);
    // ftype = xframe<L, vtype1, vtype2>;
    // ftype::vtype = variant<variable<double, coord_type1>&, variable<int, coord_type2>&>
    // ftype::vtype::value_type = variant<vtype1::value_type, vtype1::value_type>;
    // ftype::vtype::coordinate_type = variant<vtype1::coordinate_type, vtype2::coordinate_type>;
    // ftype::vtype::dimension_type = variant<vtype1::dimension_type, vtype2::dimension_type>;
    // ftype::vtype::data_type = variant<vtype1::data_type, vtype2::deta_type>
    // ftype::ctype = xcommon_coordinate_type<coord_type1, coord_type2>;
    // ftype::dtype = xcommon_dimension_type<typename vtype1::dimension_type, typename vtype2::dimension_type>;
    //
    // xframe not aligned:
    // std::map<L, ftype::vtype>;
    // ftype::ctype m_coord;
    // ftype::dtype m_dims;
    // f["n1"] returns ??
    //
    // xframe aligned
    // std::map<L, ftype::vtype::data_type>;
    // ftype::ctype m_coord;
    // ftype::dtype m_dims;
    // f["n1"]
}

#endif

