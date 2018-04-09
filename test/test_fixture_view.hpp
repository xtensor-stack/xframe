/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_TEST_FIXTURE_VIEW_HPP
#define XFRAME_TEST_FIXTURE_VIEW_HPP

#include <cstddef>
#include "xtl/xbasic_fixed_string.hpp"
#include "xframe/xvariable_view.hpp"

namespace xf
{
    using fstring = xtl::xfixed_string<55>;
    using saxis_type = xaxis<fstring, std::size_t>;
    using iaxis_type = xaxis<int, std::size_t>;
    using size_type = std::size_t;
    using axis_variant = xaxis_variant<DEFAULT_LABEL_LIST, size_type>;
    using axis_view_type = xaxis_view<DEFAULT_LABEL_LIST, size_type, typename axis_variant::map_container_tag>;
    using coordinate_type = xcoordinate<fstring, size_type>;
    using coordinate_view_type = xcoordinate_view<fstring, size_type>;
    using dimension_type = xdimension<fstring, std::size_t>;
    using data_type = xt::xoptional_assembly<xt::xarray<double>, xt::xarray<bool>>;
    using variable_type = xvariable<coordinate_type, data_type>;
    using variable_view_type = xvariable_view<variable_type&>;

    // { "a", "c", "d", "f", "g", "h", "m", "n" }
    inline saxis_type make_test_view_saxis()
    {
        return saxis_type({ "a", "c", "d", "f", "g", "h", "m", "n" });
    }

    inline axis_variant make_variant_view_saxis()
    {
        return axis_variant(make_test_view_saxis());
    }

    // { 1, 2, 4, 5, 6, 8, 12, 13 }
    inline iaxis_type make_test_view_iaxis()
    {
        return iaxis_type({ 1, 2, 4, 5, 6, 8, 12, 13 });
    }

    // abscissa: { "a", "c", "d", "f", "g", "h", "m", "n" }
    // ordinate: { 1, 2, 4, 5, 6, 8, 12, 13 }
    inline coordinate_type make_test_view_coordinate()
    {
        fstring s1 = "abscissa";
        fstring s2 = "ordinate";
        return coordinate(std::make_pair(s1, make_test_view_saxis()), std::make_pair(s2, make_test_view_iaxis()));
    }

    // abscissa: { "a", "c", "d", "f", "g", "h", "m", "n" }
    // ordinate: { 1, 2, 4, 5, 6, 8, 12, 13 }
    // dims: {{ "abscissa", 0 }, { "ordinate", 1 }}
    inline variable_type make_test_view_variable()
    {
        coordinate_type c = make_test_view_coordinate();
        data_type d = data_type::from_shape({ c["abscissa"].size(), c["ordinate"].size() });
        std::iota(d.begin(), d.end(), 0.);
        d(2, 3).has_value() = false;
        d(2, 4).has_value() = false;
        return variable_type(std::move(d), std::move(c), dimension_type({ "abscissa", "ordinate" }));
    }

    // abscissa: { "f", "g", "h", "m", "n" }
    // ordinate: { 1, 4, 6 }
    inline coordinate_view_type build_coordinate_view(const coordinate_type& c)
    {
        using map_type = typename coordinate_view_type::map_type;
        map_type nmap;

        auto r = range("f", "n");
        auto sr = range(1, 6, 2);

        const auto& abscissa = c["abscissa"];
        const auto& ordinate = c["ordinate"];

        nmap.emplace(std::make_pair("abscissa", axis_view_type(abscissa, r.build_islice(abscissa))));
        nmap.emplace(std::make_pair("ordinate", axis_view_type(ordinate, sr.build_islice(ordinate))));

        return coordinate_view(std::move(nmap));
    }

    // abscissa: { "f", "g", "h", "m", "n" }
    // ordinate: { 1, 4, 6 }
    // dims: {{ "abscissa", 0 }, { "ordinate", 1 }}
    inline variable_view_type build_view(variable_type& v)
    {
        const coordinate_type& c = v.coordinates();
        dimension_type dim = v.dimension_mapping();
        return variable_view_type(v, build_coordinate_view(c), std::move(dim));
    }
}

#endif
