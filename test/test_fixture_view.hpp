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
#include "xframe/xcoordinate_view.hpp"

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
}

#endif
