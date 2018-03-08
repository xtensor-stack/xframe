/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
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
    inline xtl::xoptional<const double&, const bool&> opt_cast(xtl::any arg)
    {
        return xtl::any_cast<xtl::xoptional<const double&, const bool&>>(arg);
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

        EXPECT_EQ(opt_cast(t00), v(0, 0));
        EXPECT_EQ(opt_cast(t01), v(0, 1));
        EXPECT_EQ(opt_cast(t02), v(0, 2));
        EXPECT_EQ(opt_cast(t10), v(1, 0));
        EXPECT_EQ(opt_cast(t11), v(1, 1));
        EXPECT_EQ(opt_cast(t12), v(1, 2));
        EXPECT_EQ(opt_cast(t20), v(2, 0));
        EXPECT_EQ(opt_cast(t21), v(2, 1));
        EXPECT_EQ(opt_cast(t22), v(2, 2));

        auto t100 = dv.select<join::outer>({ { "abscissa", "a" },{ "ordinate", 1 },{ "altitude", 1 } });
        EXPECT_EQ(opt_cast(t100), opt_cast(t00));

        auto mis = dv.select<join::outer>({ { "abscissa", "e" },{ "ordinate", 1 } });
        EXPECT_EQ(opt_cast(mis), v.missing());
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

        auto t00 = dv.locate({ { 0, "a" },{ 1, 1 } });
        auto t01 = dv.locate({ { 0, "a" },{ 1, 2 } });
        auto t02 = dv.locate({ { 0, "a" },{ 1, 4 } });
        auto t10 = dv.locate({ { 0, "c" },{ 1, 1 } });
        auto t11 = dv.locate({ { 0, "c" },{ 1, 2 } });
        auto t12 = dv.locate({ { 0, "c" },{ 1, 4 } });
        auto t20 = dv.locate({ { 0, "d" },{ 1, 1 } });
        auto t21 = dv.locate({ { 0, "d" },{ 1, 2 } });
        auto t22 = dv.locate({ { 0, "d" },{ 1, 4 } });

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
}
