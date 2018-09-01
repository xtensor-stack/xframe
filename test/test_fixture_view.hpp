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

#include "xtensor/xmasked_value.hpp"

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
    using masked_data_type = xt::xarray<xt::xmasked_value<xtl::xoptional<double, bool>>>;
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
        return coordinate<fstring>({
            {s1, make_test_view_saxis()},
            {s2, make_test_view_iaxis()}
        });
    }

    //                              ordinate
    //                 1,   2,   4,   5,   6,   8,  12,  13
    //           a {{  0,   1,   2,   3,   4,   5,   6,   7},
    //           c  {  8,   9,  10,  11,  12,  13,  14,  15},
    //           d  { 16,  17,  18, N/A, N/A,  21,  22,  23},
    // abscissa  f  { 24,  25,  26,  27,  28,  29,  30,  31},
    //           g  { 32,  33,  34,  35,  36,  37,  38,  39},
    //           h  { 40,  41,  42,  43,  44,  45,  46,  47},
    //           m  { 48,  49,  50,  51,  52,  53,  54,  55},
    //           n  { 56,  57,  58,  59,  60,  61,  62,  63}}
    inline variable_type make_test_view_variable()
    {
        coordinate_type c = make_test_view_coordinate();
        data_type d = data_type::from_shape({ c["abscissa"].size(), c["ordinate"].size() });
        std::iota(d.begin(), d.end(), 0.);
        d(2, 3).has_value() = false;
        d(2, 4).has_value() = false;
        return variable_type(std::move(d), std::move(c), dimension_type({ "abscissa", "ordinate" }));
    }

    //                              ordinate
    //                 1,   2,   4,   5,      6,      8,     12,     13
    //           a {{  0,   1,   2,   3, masked, masked, masked, masked},
    //           c  {  8,   9,  10,  11, masked, masked, masked, masked},
    //           d  { 16,  17,  18, N/A, masked, masked, masked, masked},
    // abscissa  f  { 24,  25,  26,  27, masked, masked, masked, masked},
    //           g  { 32,  33,  34,  35, masked, masked, masked, masked},
    //           h  { 40,  41,  42,  43, masked, masked, masked, masked},
    //           m  { 48,  49,  50,  51, masked, masked, masked, masked},
    //           n  { 56,  57,  58,  59, masked, masked, masked, masked}
    inline auto make_masked_data()
    {
        masked_data_type val = masked_data_type::from_shape({ 8, 8 });
        std::iota(val.begin(), val.end(), 0.);
        val(2, 3).value().has_value() = false;
        val(2, 4).value().has_value() = false;

        for (std::size_t a = 0; a < val.shape()[0]; ++a)
        {
            for (std::size_t o = 4; o < val.shape()[1]; ++o)
            {
                val(a, o) = xt::masked<xtl::xoptional<double, bool>>();
            }
        }
        return val;
    }

    //                              ordinate
    //                 1,   2,   4,   5,   6,   8,  12,  13
    //           a {{N/A,   1,   2,   3,   4,   5,   6,   7},
    //           c  {N/A,   9,  10,  11,  12,  13,  14,  15},
    //           d  {N/A,  17,  18, N/A, N/A,  21,  22,  23},
    // abscissa  f  {N/A,  25,  26,  27,  28,  29,  30,  31},
    //           g  {N/A,  33,  34,  35,  36,  37,  38,  39},
    //           h  {N/A,  41,  42,  43,  44,  45,  46,  47},
    //           m  {N/A, N/A, N/A, N/A, N/A, N/A, N/A, N/A},
    //           n  {N/A,  57,  58,  59,  60,  61,  62,  63}
    inline auto make_masked_data2()
    {
        masked_data_type val = masked_data_type::from_shape({ 8, 8 });
        std::iota(val.begin(), val.end(), 0.);
        val(2, 3).value().has_value() = false;
        val(2, 4).value().has_value() = false;

        for (std::size_t o = 0; o < val.shape()[1]; ++o)
        {
            val(6, o) = xt::masked<xtl::xoptional<double, bool>>();
        }
        for (std::size_t a = 0; a < val.shape()[0]; ++a)
        {
            val(a, 0) = xt::masked<xtl::xoptional<double, bool>>();
        }
        return val;
    }

    //                              ordinate
    //                 1,   2,   4,   5,   6,   8,  12,  13
    //           a {{N/A, 5.2, 5.2, 5.2, 5.2, 5.2, 5.2, 5.2},
    //           c  {N/A, 5.2, 5.2, 5.2, 5.2, 5.2, 5.2, 5.2},
    //           d  {N/A, 5.2, 5.2, 5.2, 5.2, 5.2, 5.2, 5.2},
    // abscissa  f  {N/A, 5.2, 5.2, 5.2, 5.2, 5.2, 5.2, 5.2},
    //           g  {N/A, 5.2, 5.2, 5.2, 5.2, 5.2, 5.2, 5.2},
    //           h  {N/A, 5.2, 5.2, 5.2, 5.2, 5.2, 5.2, 5.2},
    //           m  {N/A, N/A, N/A, N/A, N/A, N/A, N/A, N/A},
    //           n  {N/A, 5.2, 5.2, 5.2, 5.2, 5.2, 5.2, 5.2}
    inline auto make_masked_data3()
    {
        masked_data_type val = masked_data_type::from_shape({ 8, 8 });
        std::iota(val.begin(), val.end(), 0.);
        val(2, 3).value().has_value() = false;
        val(2, 4).value().has_value() = false;

        val.fill(5.2);
        for (std::size_t o = 0; o < val.shape()[1]; ++o)
        {
            val(6, o) = xt::masked<xtl::xoptional<double, bool>>();
        }
        for (std::size_t a = 0; a < val.shape()[0]; ++a)
        {
            val(a, 0) = xt::masked<xtl::xoptional<double, bool>>();
        }
        return val;
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

        nmap.emplace(std::make_pair("abscissa", axis_view_type(abscissa, r.build_index_slice(abscissa))));
        nmap.emplace(std::make_pair("ordinate", axis_view_type(ordinate, sr.build_index_slice(ordinate))));

        return coordinate_view(std::move(nmap));
    }

    // abscissa: { "f", "g", "h", "m", "n" }
    // ordinate: { 1, 4, 6 }
    // dims: {{ "abscissa", 0 }, { "ordinate", 1 }}
    inline variable_view_type build_view(variable_type& v)
    {
        const coordinate_type& c = v.coordinates();
        dimension_type dim = v.dimension_mapping();
        return variable_view_type(v, build_coordinate_view(c), std::move(dim), {});
    }
}

#endif
