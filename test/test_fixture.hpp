/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <cstddef>
#include "xtl/xbasic_fixed_string.hpp"
#include "xframe/xvariable.hpp"

namespace xf
{
    using fstring = xtl::xfixed_string<55>;
    using saxis_type = xaxis<fstring, std::size_t>; 
    using iaxis_type = xaxis<int, std::size_t>;
    using data_type = xt::xoptional_assembly<xt::xarray<double>, xt::xarray<bool>>;
    using coordinate_type = xcoordinate<fstring, data_type::size_type>;
    using variable_type = xvariable<fstring, data_type::value_expression, data_type::flag_expression>;

    /********
     * axes *
     ********/

    // { "a", "c", "d" }
    inline saxis_type make_test_saxis()
    {
        return saxis_type({"a", "c", "d"});
    }

    // { "a", "d", "e" }
    inline saxis_type make_test_saxis2()
    {
        return saxis_type({"a", "d", "e"});
    }

    // { 1, 2, 4 }
    inline iaxis_type make_test_iaxis()
    {
        return iaxis_type({1, 2, 4});
    }

    // { 1, 4, 5 }
    inline iaxis_type make_test_iaxis2()
    {
        return iaxis_type({1, 4, 5});
    }

    /***************
     * coordinates *
     ***************/

    // abscissa: { "a", "c", "d" }
    // ordinate: { 1, 2, 4 }
    inline coordinate_type make_test_coordinate()
    {
        fstring s1 = "abscissa";
        fstring s2 = "ordinate";
        return coordinate(std::make_pair(s1, make_test_saxis()), std::make_pair(s2, make_test_iaxis()));
    }

    // abscissa: { "a", "d", "e" }
    // ordinate: { 1, 4, 5 }
    inline coordinate_type make_test_coordinate2()
    {
        fstring s1 = "abscissa";
        fstring s2 = "ordinate";
        return coordinate(std::make_pair(s1, make_test_saxis2()), std::make_pair(s2, make_test_iaxis2()));
    }

    // abscissa: { "a", "d", "e" }
    // ordinate: { 1, 4, 5 }
    // altitude: { 1, 2, 4 }
    inline coordinate_type make_test_coordinate3()
    {
        fstring s1 = "abscissa";
        fstring s2 = "ordinate";
        fstring s3 = "altitude";
        return coordinate(std::make_pair(s1, make_test_saxis2()),
                          std::make_pair(s2, make_test_iaxis2()),
                          std::make_pair(s3, make_test_iaxis()));
    }

    // abscissa: { "a", "c", "d", "e" }
    // ordinate: { 1, 2, 4, 5 }
    // altitude: { 1, 2, 4 }
    inline coordinate_type make_merge_coordinate()
    {
        saxis_type sres = { "a", "c", "d", "e" };
        iaxis_type ires = { 1, 2, 4, 5 };
        return coordinate(std::make_pair(fstring("abscissa"), std::move(sres)),
                          std::make_pair(fstring("ordinate"), std::move(ires)),
                          std::make_pair(fstring("altitude"), make_test_iaxis()));
    }

    inline coordinate_type make_intersect_coordinate()
    {
        saxis_type sres = { "a", "d" };
        iaxis_type ires = { 1, 4 };
        return coordinate(std::make_pair(fstring("abscissa"), std::move(sres)),
                          std::make_pair(fstring("ordinate"), std::move(ires)),
                          std::make_pair(fstring("altitude"), make_test_iaxis()));
    }

    /*************
     * variables *
     *************/

    // data = {{ 1. ,  2., N/A },
    //         { N/A,  5.,  6. },
    //         { 7. ,  8.,  9. }}
    inline data_type make_test_data()
    {
        data_type d = {{ 1., 2., 3.},
                       { 4., 5., 6.},
                       { 7., 8., 9.}};
        d(0, 2).has_value() = false;
        d(1, 0).has_value() = false;
        return d;
    }

    // abscissa: { "a", "d", "e" }
    // ordinate: { 1, 2, 4 }
    // dims: {{ "abscissa", 0 }, { "ordinate", 1 }}
    // data = {{ 1. ,  2., N/A },
    //         { N/A,  5.,  6. },
    //         { 7. ,  8.,  9. }}
    inline variable_type make_test_variable()
    {
        return variable_type(make_test_data(), make_test_coordinate(), saxis_type({"abscissa", "ordinate"}));
    }
}

