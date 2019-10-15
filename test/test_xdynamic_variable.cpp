/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"
#include "test_fixture.hpp"
#include "xframe/xdynamic_variable.hpp"

namespace xf
{
    inline xtl::xoptional<double&, bool&> opt_cast(xtl::any arg)
    {
        return xtl::any_cast<xtl::xoptional<double&, bool&>>(arg);
    }

    inline xtl::xoptional<const double&, const bool&> const_opt_cast(xtl::any arg)
    {
        return xtl::any_cast<xtl::xoptional<const double&, const bool&>>(arg);
    }

    TEST(xdynamic_variable, size)
    {
        auto v = make_test_variable();
        auto dv = make_dynamic(v);
        EXPECT_EQ(dv.size(), v.size());
    }

    TEST(xdynamic_variable, dimension)
    {
        auto v = make_test_variable();
        auto dv = make_dynamic(v);
        EXPECT_EQ(dv.dimension(), v.dimension());
    }

    TEST(xdynamic_variable, dimension_labels)
    {
        auto v = make_test_variable();
        auto dv = make_dynamic(v);
        EXPECT_EQ(dv.dimension_labels(), v.dimension_labels());
    }

    TEST(xdynamic_variable, coordinates)
    {
        auto v = make_test_variable();
        auto dv = make_dynamic(v);
        EXPECT_EQ(dv.coordinates(), v.coordinates());
    }

    TEST(xdynamic_variable, dimension_mapping)
    {
        auto v = make_test_variable();
        auto dv = make_dynamic(v);
        EXPECT_EQ(dv.dimension_mapping(), v.dimension_mapping());
    }

    TEST(xdynamic_variable, broadcast_coordinates)
    {
        auto coord_res = make_merge_coordinate();
        auto v = make_test_variable2();
        auto dv = make_dynamic(v);

        auto c = make_test_coordinate();
        auto res = dv.template broadcast_coordinates<join::outer>(c);

        EXPECT_FALSE(res.m_same_dimensions);
        EXPECT_FALSE(res.m_same_labels);
        EXPECT_EQ(c, coord_res);

        auto coord_res2 = make_intersect_coordinate();
        auto c2 = make_test_coordinate();
        auto res2 = dv.template broadcast_coordinates<join::inner>(c2);

        EXPECT_FALSE(res2.m_same_dimensions);
        EXPECT_FALSE(res2.m_same_labels);
        EXPECT_EQ(c2, coord_res2);
    }

    TEST(xdynamic_variable, broadcast_dimensions)
    {
        auto v = make_test_variable2();
        auto dv = make_dynamic(v);

        auto c = dimension_type({ "abscissa", "ordinate" });
        bool res = dv.broadcast_dimensions(c);
        EXPECT_EQ(c, dv.dimension_mapping());
    }

    TEST(xdynamic_variable, shape)
    {
        auto v = make_test_variable();
        auto dv = make_dynamic<double>(v);

        auto shape = dv.shape();
        std::decay_t<decltype(dv.shape())> expected = { 3, 3 };
        EXPECT_EQ(shape, expected);
    }

    TEST(xdynamic_variable, access)
    {
        auto v = make_test_variable();
        auto dv = make_dynamic(v);

        auto t00 = dv(0, 0);
        auto t01 = dv(0, 1);
        auto t02 = dv(0, 2);
        auto t10 = dv(1, 0);
        auto t11 = dv(1, 1);
        auto t12 = dv(1, 2);
        auto t20 = dv(2, 0);
        auto t21 = dv(2, 1);
        auto t22 = dv(2, 2);

        EXPECT_EQ(opt_cast(t00), v(0, 0));
        EXPECT_EQ(opt_cast(t01), v(0, 1));
        EXPECT_EQ(opt_cast(t02), v(0, 2));
        EXPECT_EQ(opt_cast(t10), v(1, 0));
        EXPECT_EQ(opt_cast(t11), v(1, 1));
        EXPECT_EQ(opt_cast(t12), v(1, 2));
        EXPECT_EQ(opt_cast(t20), v(2, 0));
        EXPECT_EQ(opt_cast(t21), v(2, 1));
        EXPECT_EQ(opt_cast(t22), v(2, 2));
    }

    TEST(xdynamic_variable, element)
    {
        auto v = make_test_variable();
        auto dv = make_dynamic(v);

        auto t00 = dv.element({ 0, 0 });
        auto t01 = dv.element({ 0, 1 });
        auto t02 = dv.element({ 0, 2 });
        auto t10 = dv.element({ 1, 0 });
        auto t11 = dv.element({ 1, 1 });
        auto t12 = dv.element({ 1, 2 });
        auto t20 = dv.element({ 2, 0 });
        auto t21 = dv.element({ 2, 1 });
        auto t22 = dv.element({ 2, 2 });

        EXPECT_EQ(opt_cast(t00), v(0, 0));
        EXPECT_EQ(opt_cast(t01), v(0, 1));
        EXPECT_EQ(opt_cast(t02), v(0, 2));
        EXPECT_EQ(opt_cast(t10), v(1, 0));
        EXPECT_EQ(opt_cast(t11), v(1, 1));
        EXPECT_EQ(opt_cast(t12), v(1, 2));
        EXPECT_EQ(opt_cast(t20), v(2, 0));
        EXPECT_EQ(opt_cast(t21), v(2, 1));
        EXPECT_EQ(opt_cast(t22), v(2, 2));
    }

    TEST(xdynamic_variable, select_inner)
    {
        auto v = make_test_variable();
        auto dv = make_dynamic(v);

        auto t00 = dv.select({ { "abscissa", "a" },{ "ordinate", 1 } });
        auto t01 = dv.select({ { "abscissa", "a" },{ "ordinate", 2 } });
        auto t02 = dv.select({ { "abscissa", "a" },{ "ordinate", 4 } });
        auto t10 = dv.select({ { "abscissa", "c" },{ "ordinate", 1 } });
        auto t11 = dv.select({ { "abscissa", "c" },{ "ordinate", 2 } });
        auto t12 = dv.select({ { "abscissa", "c" },{ "ordinate", 4 } });
        auto t20 = dv.select({ { "abscissa", "d" },{ "ordinate", 1 } });
        auto t21 = dv.select({ { "abscissa", "d" },{ "ordinate", 2 } });
        auto t22 = dv.select({ { "abscissa", "d" },{ "ordinate", 4 } });

        EXPECT_EQ(opt_cast(t00), v(0, 0));
        EXPECT_EQ(opt_cast(t01), v(0, 1));
        EXPECT_EQ(opt_cast(t02), v(0, 2));
        EXPECT_EQ(opt_cast(t10), v(1, 0));
        EXPECT_EQ(opt_cast(t11), v(1, 1));
        EXPECT_EQ(opt_cast(t12), v(1, 2));
        EXPECT_EQ(opt_cast(t20), v(2, 0));
        EXPECT_EQ(opt_cast(t21), v(2, 1));
        EXPECT_EQ(opt_cast(t22), v(2, 2));
    }

    TEST(xdynamic_variable, select_outer)
    {
        const auto v = make_test_variable();
        auto dv = make_dynamic(v);

        auto t00 = dv.select<join::outer>({ { "abscissa", "a" },{ "ordinate", 1 } });
        auto t01 = dv.select<join::outer>({ { "abscissa", "a" },{ "ordinate", 2 } });
        auto t02 = dv.select<join::outer>({ { "abscissa", "a" },{ "ordinate", 4 } });
        auto t10 = dv.select<join::outer>({ { "abscissa", "c" },{ "ordinate", 1 } });
        auto t11 = dv.select<join::outer>({ { "abscissa", "c" },{ "ordinate", 2 } });
        auto t12 = dv.select<join::outer>({ { "abscissa", "c" },{ "ordinate", 4 } });
        auto t20 = dv.select<join::outer>({ { "abscissa", "d" },{ "ordinate", 1 } });
        auto t21 = dv.select<join::outer>({ { "abscissa", "d" },{ "ordinate", 2 } });
        auto t22 = dv.select<join::outer>({ { "abscissa", "d" },{ "ordinate", 4 } });

        EXPECT_EQ(const_opt_cast(t00), v(0, 0));
        EXPECT_EQ(const_opt_cast(t01), v(0, 1));
        EXPECT_EQ(const_opt_cast(t02), v(0, 2));
        EXPECT_EQ(const_opt_cast(t10), v(1, 0));
        EXPECT_EQ(const_opt_cast(t11), v(1, 1));
        EXPECT_EQ(const_opt_cast(t12), v(1, 2));
        EXPECT_EQ(const_opt_cast(t20), v(2, 0));
        EXPECT_EQ(const_opt_cast(t21), v(2, 1));
        EXPECT_EQ(const_opt_cast(t22), v(2, 2));

        auto t100 = dv.select<join::outer>({ { "abscissa", "a" },{ "ordinate", 1 },{ "altitude", 1 } });
        EXPECT_EQ(const_opt_cast(t100), const_opt_cast(t00));

        auto mis = dv.select<join::outer>({ { "abscissa", "e" },{ "ordinate", 1 } });
        EXPECT_EQ(const_opt_cast(mis), v.missing());
    }

    TEST(xdynamic_variable, iselect)
    {
        auto v = make_test_variable();
        auto dv = make_dynamic(v);

        auto t00 = dv.iselect({ { "abscissa", 0 },{ "ordinate", 0 } });
        auto t01 = dv.iselect({ { "abscissa", 0 },{ "ordinate", 1 } });
        auto t02 = dv.iselect({ { "abscissa", 0 },{ "ordinate", 2 } });
        auto t10 = dv.iselect({ { "abscissa", 1 },{ "ordinate", 0 } });
        auto t11 = dv.iselect({ { "abscissa", 1 },{ "ordinate", 1 } });
        auto t12 = dv.iselect({ { "abscissa", 1 },{ "ordinate", 2 } });
        auto t20 = dv.iselect({ { "abscissa", 2 },{ "ordinate", 0 } });
        auto t21 = dv.iselect({ { "abscissa", 2 },{ "ordinate", 1 } });
        auto t22 = dv.iselect({ { "abscissa", 2 },{ "ordinate", 2 } });

        EXPECT_EQ(opt_cast(t00), v(0, 0));
        EXPECT_EQ(opt_cast(t01), v(0, 1));
        EXPECT_EQ(opt_cast(t02), v(0, 2));
        EXPECT_EQ(opt_cast(t10), v(1, 0));
        EXPECT_EQ(opt_cast(t11), v(1, 1));
        EXPECT_EQ(opt_cast(t12), v(1, 2));
        EXPECT_EQ(opt_cast(t20), v(2, 0));
        EXPECT_EQ(opt_cast(t21), v(2, 1));
        EXPECT_EQ(opt_cast(t22), v(2, 2));
    }

    TEST(xdynamic_variable, locate)
    {
        auto v = make_test_variable();
        auto dv = make_dynamic(v);

        auto t00 = dv.locate("a", 1);
        auto t01 = dv.locate("a", 2);
        auto t02 = dv.locate("a", 4);
        auto t10 = dv.locate("c", 1);
        auto t11 = dv.locate("c", 2);
        auto t12 = dv.locate("c", 4);
        auto t20 = dv.locate("d", 1);
        auto t21 = dv.locate("d", 2);
        auto t22 = dv.locate("d", 4);

        EXPECT_EQ(opt_cast(t00), v(0, 0));
        EXPECT_EQ(opt_cast(t01), v(0, 1));
        EXPECT_EQ(opt_cast(t02), v(0, 2));
        EXPECT_EQ(opt_cast(t10), v(1, 0));
        EXPECT_EQ(opt_cast(t11), v(1, 1));
        EXPECT_EQ(opt_cast(t12), v(1, 2));
        EXPECT_EQ(opt_cast(t20), v(2, 0));
        EXPECT_EQ(opt_cast(t21), v(2, 1));
        EXPECT_EQ(opt_cast(t22), v(2, 2));
    }

    TEST(xdynamic_variable, locate_element)
    {
        auto v = make_test_variable();
        auto dv = make_dynamic(v);

        auto t00 = dv.locate_element({ "a", 1 });
        auto t01 = dv.locate_element({ "a", 2 });
        auto t02 = dv.locate_element({ "a", 4 });
        auto t10 = dv.locate_element({ "c", 1 });
        auto t11 = dv.locate_element({ "c", 2 });
        auto t12 = dv.locate_element({ "c", 4 });
        auto t20 = dv.locate_element({ "d", 1 });
        auto t21 = dv.locate_element({ "d", 2 });
        auto t22 = dv.locate_element({ "d", 4 });

        EXPECT_EQ(opt_cast(t00), v(0, 0));
        EXPECT_EQ(opt_cast(t01), v(0, 1));
        EXPECT_EQ(opt_cast(t02), v(0, 2));
        EXPECT_EQ(opt_cast(t10), v(1, 0));
        EXPECT_EQ(opt_cast(t11), v(1, 1));
        EXPECT_EQ(opt_cast(t12), v(1, 2));
        EXPECT_EQ(opt_cast(t20), v(2, 0));
        EXPECT_EQ(opt_cast(t21), v(2, 1));
        EXPECT_EQ(opt_cast(t22), v(2, 2));
    }

    template <class V>
    inline xtl::xoptional<double&, bool&> variant_get(V& v)
    {
        return xtl::get<xtl::xoptional<double&, bool&>>(v);
    }

    template <class V>
    inline xtl::xoptional<const double&, const bool&> const_variant_get(V& v)
    {
        return xtl::get<xtl::xoptional<const double&, const bool&>>(v);
    }

    TEST(xdynamic_variable, make_variant)
    {
        auto v = make_test_variable();
        auto dv = make_dynamic<xtl::variant<double, int>>(v);
        
        auto ta00 = dv(0, 0);
        EXPECT_EQ(variant_get(ta00), v(0, 0));

        auto t00 = dv.element({ 0, 0 });
        EXPECT_EQ(variant_get(t00), v(0, 0));

        auto ts00 = dv.select({ { "abscissa", "a" },{ "ordinate", 1 } });
        EXPECT_EQ(variant_get(ts00), v(0, 0));

        auto tsc00 = dv.select<join::outer>({ { "abscissa", "a" },{ "ordinate", 1 } });
        EXPECT_EQ(const_variant_get(tsc00), v(0, 0));

        auto tis00 = dv.iselect({ { "abscissa", 0 },{ "ordinate", 0 } });
        EXPECT_EQ(variant_get(tis00), v(0, 0));

        auto tl00 = dv.locate("a", 1);
        EXPECT_EQ(variant_get(tl00), v(0, 0));

        auto tle00 = dv.locate_element({ "a", 1 });
        EXPECT_EQ(variant_get(tle00), v(0, 0));
    }

    TEST(xdynamic_variable, make_simple)
    {
        auto v = make_test_variable();
        auto dv = make_dynamic<double>(v);

        auto ta00 = dv(0, 0);
        EXPECT_EQ(ta00, v(0, 0));

        auto t00 = dv.element({ 0, 0 });
        EXPECT_EQ(t00, v(0, 0));

        auto ts00 = dv.select({ { "abscissa", "a" },{ "ordinate", 1 } });
        EXPECT_EQ(ts00, v(0, 0));

        auto tsc00 = dv.select<join::outer>({ { "abscissa", "a" },{ "ordinate", 1 } });
        EXPECT_EQ(tsc00, v(0, 0));

        auto tis00 = dv.iselect({ { "abscissa", 0 },{ "ordinate", 0 } });
        EXPECT_EQ(tis00, v(0, 0));

        auto tl00 = dv.locate("a", 1);
        EXPECT_EQ(tl00, v(0, 0));

        auto tle00 = dv.locate_element({ "a", 1 });
        EXPECT_EQ(tle00, v(0, 0));
    }

    TEST(xdynamic_variable, print)
    {
        auto v = make_test_variable();
        auto dv = make_dynamic<double>(v);

        std::string expected =
R"variable({{  1,   2, N/A},
 {N/A,   5,   6},
 {  7,   8,   9}}
Coordinates:
abscissa: (a, c, d, )
ordinate: (1, 2, 4, )
)variable";

        std::ostringstream oss;
        oss << dv;
        std::string res = oss.str();
        EXPECT_EQ(res, expected);
    }
}
