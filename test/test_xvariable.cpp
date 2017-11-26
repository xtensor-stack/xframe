/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"
#include "test_fixture.hpp"

namespace xf
{

    TEST(xvariable, constructor)
    {
        auto v1 = variable_type(make_test_data(), make_test_coordinate(), dimension_type({"abscissa", "ordinate"}));
        
        variable_type::coordinate_map m;
        m["abscissa"] = make_test_saxis();
        m["ordinate"] = make_test_iaxis();

        data_type d = make_test_data();
        dimension_type dim_map = {"abscissa", "ordinate"};
        auto v2 = variable_type(d, m, dim_map);
        auto v3 = variable_type(d, std::move(m), std::move(dim_map));

        EXPECT_EQ(v1, v2);
        EXPECT_EQ(v1, v3);
        EXPECT_FALSE(v1 != v2);

        auto v4 = variable_type(std::move(d), {{"abscissa", make_test_saxis()}, {"ordinate", make_test_iaxis()}});
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
        saxis_type s = { "abscissa", "ordinate" };

        EXPECT_EQ(v.dimension_labels(), s.labels());
    }

    TEST(xvariable, reshape)
    {
        auto v1 = make_test_variable();
        auto shape1 = v1.data().shape();
        decltype(shape1) res1 = { 3, 3 };
        EXPECT_EQ(shape1, res1);

        saxis_type a = { "a", "c" };
        dimension_type dim = { "abscissa" };
        auto c = coordinate(std::make_pair(fstring("abscissa"), a));
        v1.reshape(c, dim);
        auto shape2 = v1.data().shape();
        decltype(shape2) res2 = { 2 };
        EXPECT_EQ(shape2, res2);
    }

    TEST(xvariable, access)
    {
        auto v = make_test_variable();
        EXPECT_EQ(v(0, 0), 1.0);
        EXPECT_EQ(v(0, 1), 2.0);
        EXPECT_EQ(v(0, 2), xtl::missing<double>());
        EXPECT_EQ(v(1, 0), xtl::missing<double>());
        EXPECT_EQ(v(1, 1), 5.0);
        EXPECT_EQ(v(1, 2), 6.0);
        EXPECT_EQ(v(2, 0), 7.0);
        EXPECT_EQ(v(2, 1), 8.0);
        EXPECT_EQ(v(2, 2), 9.0);
    }

    TEST(xvariable, select)
    {
        auto v = make_test_variable();
        auto t00 = v.select({{"abscissa", "a"}, {"ordinate", 1}});
        auto t01 = v.select({{"abscissa", "a"}, {"ordinate", 2}});
        auto t02 = v.select({{"abscissa", "a"}, {"ordinate", 4}});
        auto t10 = v.select({{"abscissa", "c"}, {"ordinate", 1}});
        auto t11 = v.select({{"abscissa", "c"}, {"ordinate", 2}});
        auto t12 = v.select({{"abscissa", "c"}, {"ordinate", 4}});
        auto t20 = v.select({{"abscissa", "d"}, {"ordinate", 1}});
        auto t21 = v.select({{"abscissa", "d"}, {"ordinate", 2}});
        auto t22 = v.select({{"abscissa", "d"}, {"ordinate", 4}});

        EXPECT_EQ(t00, v(0, 0));
        EXPECT_EQ(t01, v(0, 1));
        EXPECT_EQ(t02, v(0, 2));
        EXPECT_EQ(t10, v(1, 0));
        EXPECT_EQ(t11, v(1, 1));
        EXPECT_EQ(t12, v(1, 2));
        EXPECT_EQ(t20, v(2, 0));
        EXPECT_EQ(t21, v(2, 1));
        EXPECT_EQ(t22, v(2, 2));
    }

    TEST(xvariable, iselect)
    {
        auto v = make_test_variable();
        auto t00 = v.iselect({{"abscissa", 0}, {"ordinate", 0}});
        auto t01 = v.iselect({{"abscissa", 0}, {"ordinate", 1}});
        auto t02 = v.iselect({{"abscissa", 0}, {"ordinate", 2}});
        auto t10 = v.iselect({{"abscissa", 1}, {"ordinate", 0}});
        auto t11 = v.iselect({{"abscissa", 1}, {"ordinate", 1}});
        auto t12 = v.iselect({{"abscissa", 1}, {"ordinate", 2}});
        auto t20 = v.iselect({{"abscissa", 2}, {"ordinate", 0}});
        auto t21 = v.iselect({{"abscissa", 2}, {"ordinate", 1}});
        auto t22 = v.iselect({{"abscissa", 2}, {"ordinate", 2}});

        EXPECT_EQ(t00, v(0, 0));
        EXPECT_EQ(t01, v(0, 1));
        EXPECT_EQ(t02, v(0, 2));
        EXPECT_EQ(t10, v(1, 0));
        EXPECT_EQ(t11, v(1, 1));
        EXPECT_EQ(t12, v(1, 2));
        EXPECT_EQ(t20, v(2, 0));
        EXPECT_EQ(t21, v(2, 1));
        EXPECT_EQ(t22, v(2, 2));
    }

    TEST(xvariable, locate)
    {
        auto v = make_test_variable();
        auto t00 = v.locate({{0, "a"}, {1, 1}});
        auto t01 = v.locate({{0, "a"}, {1, 2}});
        auto t02 = v.locate({{0, "a"}, {1, 4}});
        auto t10 = v.locate({{0, "c"}, {1, 1}});
        auto t11 = v.locate({{0, "c"}, {1, 2}});
        auto t12 = v.locate({{0, "c"}, {1, 4}});
        auto t20 = v.locate({{0, "d"}, {1, 1}});
        auto t21 = v.locate({{0, "d"}, {1, 2}});
        auto t22 = v.locate({{0, "d"}, {1, 4}});

        EXPECT_EQ(t00, v(0, 0));
        EXPECT_EQ(t01, v(0, 1));
        EXPECT_EQ(t02, v(0, 2));
        EXPECT_EQ(t10, v(1, 0));
        EXPECT_EQ(t11, v(1, 1));
        EXPECT_EQ(t12, v(1, 2));
        EXPECT_EQ(t20, v(2, 0));
        EXPECT_EQ(t21, v(2, 1));
        EXPECT_EQ(t22, v(2, 2));
    }
}
