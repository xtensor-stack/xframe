/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <array>
#include <cstddef>
#include "gtest/gtest.h"
#include "test_fixture.hpp"
#include "xframe/xnamed_axis.hpp"

namespace xf
{
    TEST(xvariable, empty_data_constructor)
    {
        variable_type::coordinate_map m;
        m["abscissa"] = make_test_saxis();
        m["ordinate"] = make_test_iaxis();
        variable_type::coordinate_map m2(m);

        dimension_type::label_list dim_map = { "abscissa", "ordinate" };
        dimension_type::label_list dim_map2(dim_map);

        auto v1 = variable_type(m, dim_map);
        auto v2 = variable_type(std::move(m2), std::move(dim_map2));
        auto v3 = variable_type({ {"abscissa", make_test_saxis()}, {"ordinate", make_test_iaxis()} });

        using shape_type = std::decay_t<decltype(v1.data().shape())>;
        shape_type shape = { 3, 3 };
        EXPECT_EQ(v1.data().shape(), shape);
        EXPECT_EQ(v2.data().shape(), shape);
        EXPECT_EQ(v3.data().shape(), shape);
        EXPECT_EQ(v3.dimension_labels()[0], "abscissa");
        EXPECT_EQ(v3.dimension_labels()[1], "ordinate");
    }

    TEST(xvariable, constructor)
    {
        auto v1 = variable_type(make_test_data(), make_test_coordinate(), dimension_type({"abscissa", "ordinate"}));

        variable_type::coordinate_map m;
        m["abscissa"] = make_test_saxis();
        m["ordinate"] = make_test_iaxis();
        variable_type::coordinate_map m2(m);

        data_type d = make_test_data();
        dimension_type::label_list dim_map = {"abscissa", "ordinate"};
        dimension_type::label_list dim_map2(dim_map);
        auto v2 = variable_type(d, m, dim_map);
        auto v3 = variable_type(d, std::move(m), std::move(dim_map));

        EXPECT_EQ(v1, v2);
        EXPECT_EQ(v1, v3);
        EXPECT_FALSE(v1 != v2);

        auto v4 = variable_type(std::move(d), {{"abscissa", make_test_saxis()}, {"ordinate", make_test_iaxis()}});
        EXPECT_EQ(v1, v4);

        auto v5 = variable_type(make_test_coordinate(), dimension_type({ "abscissa", "ordinate" }));
        EXPECT_EQ(v5.data().shape(), data_type::shape_type({ 3, 3 }));

        auto v6 = variable_type(m2, dim_map2);
        EXPECT_EQ(v6.data().shape(), data_type::shape_type({ 3, 3 }));

        auto v7 = variable_type(std::move(m2), std::move(dim_map2));
        EXPECT_EQ(v7.data().shape(), data_type::shape_type({ 3, 3 }));

        auto v8 = variable_type(make_test_data(), make_test_coordinate(), dimension({"abscissa", "ordinate"}));
        EXPECT_EQ(v1, v8);
    }

    TEST(xvariable, size)
    {
        auto v1 = make_test_variable();
        EXPECT_EQ(9u, v1.size());

        auto v2 = variable_type();
        EXPECT_EQ(1u, v2.size());
    }

    TEST(xvariable, axis_accessor)
    {
        auto v1 = make_test_variable();
        auto a = v1["ordinate"].axis();
        EXPECT_EQ(a.size(), 3u);
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

    TEST(xvariable, shape)
    {
        auto v = make_test_variable();
        auto shape = v.shape();
        std::decay_t<decltype(v.shape())> expected = { 3, 3 };
        EXPECT_EQ(shape, expected);
    }

    TEST(xvariable, resize)
    {
        auto v1 = make_test_variable();
        auto shape1 = v1.data().shape();
        decltype(shape1) res1 = { 3, 3 };
        EXPECT_EQ(shape1, res1);

        saxis_type a = { "a", "c" };
        dimension_type dim = { "abscissa" };
        auto c = coordinate<fstring>({{fstring("abscissa"), a}});
        v1.resize(c, dim);
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

    TEST(xvariable, element)
    {
        auto v = make_test_variable();
        EXPECT_EQ(v.element({ 0, 0 }), 1.0);
        EXPECT_EQ(v.element({ 0, 1 }), 2.0);
        EXPECT_EQ(v.element({ 0, 2 }), xtl::missing<double>());
        EXPECT_EQ(v.element({ 1, 0 }), xtl::missing<double>());
        EXPECT_EQ(v.element({ 1, 1 }), 5.0);
        EXPECT_EQ(v.element({ 1, 2 }), 6.0);
        EXPECT_EQ(v.element({ 2, 0 }), 7.0);
        EXPECT_EQ(v.element({ 2, 1 }), 8.0);
        EXPECT_EQ(v.element({ 2, 2 }), 9.0);
    }

    TEST(xvariable, select_inner)
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

        auto t100 = v.select({{"abscissa", "a"}, {"ordinate", 1}, {"altitude", 1}});
        EXPECT_EQ(t100, t00);

        EXPECT_ANY_THROW(v.select({{"abscissa", "e"}, {"ordinate", 1}}));
    }

    TEST(xvariable, select_outer)
    {
        const auto v = make_test_variable();
        auto t00 = v.select<join::outer>({{"abscissa", "a"}, {"ordinate", 1}});
        auto t01 = v.select<join::outer>({{"abscissa", "a"}, {"ordinate", 2}});
        auto t02 = v.select<join::outer>({{"abscissa", "a"}, {"ordinate", 4}});
        auto t10 = v.select<join::outer>({{"abscissa", "c"}, {"ordinate", 1}});
        auto t11 = v.select<join::outer>({{"abscissa", "c"}, {"ordinate", 2}});
        auto t12 = v.select<join::outer>({{"abscissa", "c"}, {"ordinate", 4}});
        auto t20 = v.select<join::outer>({{"abscissa", "d"}, {"ordinate", 1}});
        auto t21 = v.select<join::outer>({{"abscissa", "d"}, {"ordinate", 2}});
        auto t22 = v.select<join::outer>({{"abscissa", "d"}, {"ordinate", 4}});

        EXPECT_EQ(t00, v(0, 0));
        EXPECT_EQ(t01, v(0, 1));
        EXPECT_EQ(t02, v(0, 2));
        EXPECT_EQ(t10, v(1, 0));
        EXPECT_EQ(t11, v(1, 1));
        EXPECT_EQ(t12, v(1, 2));
        EXPECT_EQ(t20, v(2, 0));
        EXPECT_EQ(t21, v(2, 1));
        EXPECT_EQ(t22, v(2, 2));

        auto t100 = v.select<join::outer>({{"abscissa", "a"}, {"ordinate", 1}, {"altitude", 1}});
        EXPECT_EQ(t100, t00);

        auto mis = v.select<join::outer>({{"abscissa", "e"}, {"ordinate", 1}});
        EXPECT_EQ(mis, v.missing());
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
        auto t221 = v.iselect({{"abscissa", 2}, {"ordinate", 2}, {"missing_dimension", 1}});

        EXPECT_EQ(t00, v(0, 0));
        EXPECT_EQ(t01, v(0, 1));
        EXPECT_EQ(t02, v(0, 2));
        EXPECT_EQ(t10, v(1, 0));
        EXPECT_EQ(t11, v(1, 1));
        EXPECT_EQ(t12, v(1, 2));
        EXPECT_EQ(t20, v(2, 0));
        EXPECT_EQ(t21, v(2, 1));
        EXPECT_EQ(t22, v(2, 2));
        EXPECT_EQ(t221, v(2, 2));
    }

    TEST(xvariable, locate)
    {
        auto v = make_test_variable();
        auto t00 = v.locate("a", 1);
        auto t01 = v.locate("a", 2);
        auto t02 = v.locate("a", 4);
        auto t10 = v.locate("c", 1);
        auto t11 = v.locate("c", 2);
        auto t12 = v.locate("c", 4);
        auto t20 = v.locate("d", 1);
        auto t21 = v.locate("d", 2);
        auto t22 = v.locate("d", 4);

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

    TEST(xvariable, locate_element)
    {
        auto v = make_test_variable();

        auto t00 = v.locate_element({ "a", 1 });
        auto t01 = v.locate_element({ "a", 2 });
        auto t02 = v.locate_element({ "a", 4 });
        auto t10 = v.locate_element({ "c", 1 });
        auto t11 = v.locate_element({ "c", 2 });
        auto t12 = v.locate_element({ "c", 4 });
        auto t20 = v.locate_element({ "d", 1 });
        auto t21 = v.locate_element({ "d", 2 });
        auto t22 = v.locate_element({ "d", 4 });

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

    TEST(xvariable, generator)
    {
        dimension_type dim({ "abscissa", "ordinate" });
        coordinate_type coord1 = make_test_coordinate();
        data_type data1 = make_test_data();
        using shape_type = std::decay_t<decltype(data1.shape())>;
        shape_type shape = { 3, 3 };

        auto var1 = variable(data1, coord1, dim);
        using var1_type = decltype(var1);
        bool coord1_res = std::is_same<var1_type::coordinate_closure_type, var1_type::coordinate_type&>::value;
        bool data1_res = std::is_same<var1_type::data_closure_type, var1_type::data_type&>::value;
        EXPECT_TRUE(coord1_res);
        EXPECT_TRUE(data1_res);
        EXPECT_EQ(var1(0, 0), 1.0);
        EXPECT_EQ(var1.shape(), shape);

        const coordinate_type coord2 = make_test_coordinate();
        const data_type data2 = make_test_data();
        auto var2 = variable(data2, coord2, dim);
        using var2_type = decltype(var2);
        bool coord2_res = std::is_same<var2_type::coordinate_closure_type, const var2_type::coordinate_type&>::value;
        bool data2_res = std::is_same<var2_type::data_closure_type, const var2_type::data_type&>::value;
        EXPECT_TRUE(coord2_res);
        EXPECT_TRUE(data2_res);
        EXPECT_EQ(var2(0, 0), 1.0);
        EXPECT_EQ(var2.shape(), shape);

        auto var3 = variable(make_test_data(), make_test_coordinate(), dim);
        using var3_type = decltype(var3);
        bool coord3_res = std::is_same<var3_type::coordinate_closure_type, var3_type::coordinate_type>::value;
        bool data3_res = std::is_same<var3_type::data_closure_type, var3_type::data_type>::value;
        EXPECT_TRUE(coord3_res);
        EXPECT_TRUE(data3_res);
        EXPECT_EQ(var3(0, 0), 1.0);
        EXPECT_EQ(var3.shape(), shape);

        auto var4 = variable<double>(make_test_coordinate(), dim);
        using var4_type = decltype(var4);
        bool coord4_res = std::is_same<var4_type::coordinate_closure_type, var4_type::coordinate_type>::value;
        bool data4_res = std::is_same<var4_type::data_closure_type, var4_type::data_type>::value;
        EXPECT_TRUE(coord4_res);
        EXPECT_TRUE(data4_res);
        EXPECT_EQ(var4.shape(), shape);
    }

    TEST(xvariable, map_generator)
    {
        dimension_type::label_list dim({ "abscissa", "ordinate" });
        coordinate_type coord1 = make_test_coordinate();
        data_type data1 = make_test_data();
        coordinate_type::map_type coord_map = coord1.data();
        using shape_type = std::decay_t<decltype(data1.shape())>;
        shape_type shape = { 3, 3 };

        auto var1 = variable(data1, coord_map, dim);
        using var1_type = decltype(var1);
        bool coord1_res = std::is_same<var1_type::coordinate_closure_type, var1_type::coordinate_type>::value;
        bool data1_res = std::is_same<var1_type::data_closure_type, var1_type::data_type&>::value;
        EXPECT_TRUE(coord1_res);
        EXPECT_TRUE(data1_res);
        EXPECT_EQ(var1(0, 0), 1.0);
        EXPECT_EQ(var1.shape(), shape);

        auto var2 = variable<double>(coord_map, dim);
        using var2_type = decltype(var2);
        bool coord2_res = std::is_same<var2_type::coordinate_closure_type, var2_type::coordinate_type>::value;
        bool data2_res = std::is_same<var2_type::data_closure_type, var2_type::data_type>::value;
        EXPECT_TRUE(coord2_res);
        EXPECT_TRUE(data2_res);
        EXPECT_EQ(var2.shape(), shape);
    }

    TEST(xvariable, print)
    {
        auto var = make_test_variable();
        std::string expected =
R"variable({{  1,   2, N/A},
 {N/A,   5,   6},
 {  7,   8,   9}}
Coordinates:
abscissa: (a, c, d, )
ordinate: (1, 2, 4, )
)variable";

        std::ostringstream oss;
        oss << var;
        std::string res = oss.str();
        EXPECT_EQ(res, expected);
    }
}
