/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"
#include "xframe/xvariable.hpp"

namespace xf
{
    using fstring = xtl::xfixed_string<55>;
    using slabel_type = std::vector<fstring>;
    using saxis_type = xaxis<fstring, std::size_t>;
    using ilabel_type = std::vector<int>;
    using iaxis_type = xaxis<int, std::size_t>;
    using data_type = xt::xoptional_assembly<xt::xarray<double>, xt::xarray<bool>>;
    using coordinate_type = xcoordinate<fstring, data_type::size_type>;
    using variable_type = xvariable<fstring, data_type::value_expression, data_type::flag_expression>;
    
    namespace
    {
        auto make_test_saxis()
        {
            return saxis_type({ "a", "c", "d" });
        }

        auto make_test_iaxis()
        {
            return iaxis_type({ 1, 2, 4 });
        }

        auto make_test_coordinate()
        {
            fstring n1 = "temperature";
            fstring n2 = "pressure";

            return coordinate(std::make_pair(n1, make_test_saxis()), std::make_pair(n2, make_test_iaxis()));
        }
    
        auto make_test_data()
        {
            data_type d = {{ 1., 2., 3.},
                           { 4., 5., 6.},
                           { 7., 8., 9.}};
            d(0, 2).has_value() = false;
            d(1, 0).has_value() = false;
            return d;
        }

        auto make_test_variable()
        {
            return variable_type(make_test_data(), make_test_coordinate(), saxis_type({"temperature", "pressure"}));
        }
    }

    TEST(xvariable, constructor)
    {
        auto v1 = variable_type(make_test_data(), make_test_coordinate(), saxis_type({"temperature", "pressure"}));
        
        variable_type::coordinate_map m;
        m["temperature"] = make_test_saxis();
        m["pressure"] = make_test_iaxis();

        data_type d = make_test_data();
        saxis_type dim_map = {"temperature", "pressure"};
        auto v2 = variable_type(d, m, dim_map);
        auto v3 = variable_type(d, std::move(m), std::move(dim_map));

        EXPECT_EQ(v1, v2);
        EXPECT_EQ(v1, v3);
        EXPECT_FALSE(v1 != v2);

        auto v4 = variable_type(std::move(d), {{"temperature", make_test_saxis()}, {"pressure", make_test_iaxis()}});
        EXPECT_EQ(v1, v4);
    }

    TEST(xvariable, size)
    {
        auto v1 = make_test_variable();
        EXPECT_EQ(9u, v1.size());

        auto v2 = variable_type();
        EXPECT_EQ(1u, v2.size());
    }

    TEST(xvariable, dimension)
    {
        auto v1 = make_test_variable();
        EXPECT_EQ(2u, v1.dimension());

        auto v2 = variable_type();
        EXPECT_EQ(0u, v2.dimension());
    }

    TEST(xvariable, dimension_labels)
    {
        auto v = make_test_variable();
        saxis_type s = { "temperature", "pressure" };

        EXPECT_EQ(v.dimension_labels(), s.labels());
    }

    TEST(xvariable, access)
    {
        auto v = make_test_variable();
        EXPECT_EQ(v(0, 0), 1.0);
        EXPECT_EQ(v(0, 1), 2.0);
        EXPECT_EQ(v(0, 2), xt::missing<double>());
        EXPECT_EQ(v(1, 0), xt::missing<double>());
        EXPECT_EQ(v(1, 1), 5.0);
        EXPECT_EQ(v(1, 2), 6.0);
        EXPECT_EQ(v(2, 0), 7.0);
        EXPECT_EQ(v(2, 1), 8.0);
        EXPECT_EQ(v(2, 2), 9.0);
    }

    TEST(xvariable, select)
    {
        auto v = make_test_variable();
        auto t1 = decltype(v)::selection().set("temperature", "c").set("pressure", 2).apply_to(v);
        auto t2 = v.select(v.selection().set("temperature", "c").set("pressure", 2));

        EXPECT_EQ(t1, v(1, 1));
        EXPECT_EQ(t2, v(1, 1));
    }

}
