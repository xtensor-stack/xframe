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

#include "test_fixture_view.hpp"

#include "xframe/xvariable_masked_view.hpp"

namespace xf
{
    TEST(xvariable_masked_view, iselect)
    {
        variable_type var = make_test_view_variable();
        auto masked_var = where(var, var.axis<int>("ordinate") < 6);

        auto expected = make_masked_data();
        for (std::size_t a = 0; a < expected.shape()[0]; ++a)
        {
            for (std::size_t o = 0; o < expected.shape()[1]; ++o)
            {
                EXPECT_EQ(masked_var.iselect({{"abscissa", a}, {"ordinate", o}}), expected(a, o));
            }
        }

        auto masked_var2 = where(var, not_equal(var.axis<fstring>("abscissa"), fstring("m")) && not_equal(var.axis<int>("ordinate"), 1));
        auto expected2 = make_masked_data2();

        using const_masked_var_type = const std::decay_t<decltype(masked_var2)>;
        const_masked_var_type const_masked_var = where(var,
            not_equal(var.axis<fstring>("abscissa"), fstring("m")) &&
            not_equal(var.axis<int>("ordinate"), 1)
        );

        for (std::size_t a = 0; a < expected2.shape()[0]; ++a)
        {
            for (std::size_t o = 0; o < expected2.shape()[1]; ++o)
            {
                auto val = const_masked_var.iselect<2>({{{"abscissa", a}, {"ordinate", o}}});
                EXPECT_EQ(val, expected2(a, o));
            }
        }
    }

    TEST(xvariable_masked_view, select_inner)
    {
        variable_type var = make_test_view_variable();
        auto masked_var = where(var,
            not_equal(var.axis<fstring>("abscissa"), fstring("m")) &&
            not_equal(var.axis<int>("ordinate"), 1)
        );

        using const_masked_var_type = const std::decay_t<decltype(masked_var)>;
        const_masked_var_type const_masked_var = where(var,
            not_equal(var.axis<fstring>("abscissa"), fstring("m")) &&
            not_equal(var.axis<int>("ordinate"), 1)
        );

        auto masked_value = xtl::masked<xtl::xoptional<double, bool>>();

        auto expected = masked_var.select<join::inner, 2>({{{"abscissa", "a"}, {"ordinate", 12}}});
        EXPECT_EQ(expected, 6);
        EXPECT_EQ(masked_var.select({{"abscissa", "g"}, {"ordinate", 5}}), 35);

        EXPECT_EQ(masked_var.select({{"abscissa", "d"}, {"ordinate", 1}}), masked_value);
        EXPECT_EQ(masked_var.select({{"abscissa", "m"}, {"ordinate", 5}}), masked_value);

        var.select({{"abscissa", "g"}, {"ordinate", 5}}) = 123;
        EXPECT_EQ(masked_var.select({{"abscissa", "g"}, {"ordinate", 5}}), 123);
        masked_var.select({{"abscissa", "g"}, {"ordinate", 5}}) = 124;
        EXPECT_EQ(var.select({{"abscissa", "g"}, {"ordinate", 5}}), 124);

        EXPECT_ANY_THROW(masked_var.select({{"abscissa", "e"}, {"ordinate", 4}}));
        EXPECT_ANY_THROW(masked_var.select({{"abscissa", "a"}, {"ordinate", 3}}));
    }

    TEST(xvariable_masked_view, select_outer)
    {
        variable_type var = make_test_view_variable();
        auto masked_var = where(var,
            not_equal(var.axis<fstring>("abscissa"), fstring("m")) &&
            not_equal(var.axis<int>("ordinate"), 1)
        );

        auto masked_value = xtl::masked<xtl::xoptional<double, bool>>();

        auto expected = masked_var.select<join::outer, 2>({{{"abscissa", "a"}, {"ordinate", 12}}});
        EXPECT_EQ(expected, 6);
        EXPECT_EQ(masked_var.select<join::outer>({{"abscissa", "g"}, {"ordinate", 5}}), 35);
        EXPECT_EQ(masked_var.select<join::outer>({{"abscissa", "m"}, {"ordinate", 5}}), masked_value);
        EXPECT_EQ(masked_var.select<join::outer>({{"abscissa", "d"}, {"ordinate", 1}}), masked_value);

        EXPECT_EQ(masked_var.select<join::outer>({{"abscissa", "e"}, {"ordinate", 4}}), xtl::missing<double>());
        EXPECT_EQ(masked_var.select<join::outer>({{"abscissa", "a"}, {"ordinate", 3}}), xtl::missing<double>());
    }

    TEST(xvariable_masked_view, shape)
    {
        variable_type var = make_test_view_variable();
        auto masked_var = where(var, var.axis<int>("ordinate") < 6);

        EXPECT_EQ(masked_var.shape(), var.shape());
    }

    TEST(xvariable_masked_view, access)
    {
        variable_type var = make_test_view_variable();
        auto masked_var = where(var,
            not_equal(var.axis<fstring>("abscissa"), fstring("m")) &&
            not_equal(var.axis<int>("ordinate"), 1)
        );

        using const_masked_var_type = const std::decay_t<decltype(masked_var)>;
        const_masked_var_type const_masked_var = where(var,
            not_equal(var.axis<fstring>("abscissa"), fstring("m")) &&
            not_equal(var.axis<int>("ordinate"), 1)
        );

        auto masked_value = xtl::masked<xtl::xoptional<double, bool>>();

        EXPECT_EQ(masked_var(0, 6), 6);
        masked_var(0, 6) = 7;
        EXPECT_EQ(masked_var(0, 6), 7);

        EXPECT_EQ(masked_var(4, 3), 35);
        EXPECT_EQ(const_masked_var(4, 3), 35);
        EXPECT_EQ(masked_var(6, 3), masked_value);
        EXPECT_EQ(masked_var(2, 0), masked_value);
    }

    TEST(xvariable_masked_view, element)
    {
        variable_type var = make_test_view_variable();
        auto masked_var = where(var,
            not_equal(var.axis<fstring>("abscissa"), fstring("m")) &&
            not_equal(var.axis<int>("ordinate"), 1)
        );

        using const_masked_var_type = const std::decay_t<decltype(masked_var)>;
        const_masked_var_type const_masked_var = where(var,
            not_equal(var.axis<fstring>("abscissa"), fstring("m")) &&
            not_equal(var.axis<int>("ordinate"), 1)
        );

        auto masked_value = xtl::masked<xtl::xoptional<double, bool>>();
        EXPECT_EQ(masked_var.element<2>({0, 6}), 6);
        EXPECT_EQ(const_masked_var.element({4, 3}), 35);
        EXPECT_EQ(masked_var.element({6, 3}), masked_value);
        EXPECT_EQ(masked_var.element({2, 0}), masked_value);
    }

    TEST(xvariable_masked_view, locate)
    {
        variable_type var = make_test_view_variable();
        auto masked_var = where(var,
            not_equal(var.axis<fstring>("abscissa"), fstring("m")) &&
            not_equal(var.axis<int>("ordinate"), 1)
        );

        using const_masked_var_type = const std::decay_t<decltype(masked_var)>;
        const_masked_var_type const_masked_var = where(var,
            not_equal(var.axis<fstring>("abscissa"), fstring("m")) &&
            not_equal(var.axis<int>("ordinate"), 1)
        );

        auto masked_value = xtl::masked<xtl::xoptional<double, bool>>();
        EXPECT_EQ(masked_var.locate("a", 12), 6);
        EXPECT_EQ(const_masked_var.locate("g", 5), 35);
        EXPECT_EQ(masked_var.locate("m", 5), masked_value);
        EXPECT_EQ(masked_var.locate("d", 1), masked_value);
        EXPECT_ANY_THROW(masked_var.locate("e", 4));
    }

    TEST(xvariable_masked_view, locate_element)
    {
        variable_type var = make_test_view_variable();
        auto masked_var = where(var,
            not_equal(var.axis<fstring>("abscissa"), fstring("m")) &&
            not_equal(var.axis<int>("ordinate"), 1)
        );

        using const_masked_var_type = const std::decay_t<decltype(masked_var)>;
        const_masked_var_type const_masked_var = where(var,
            not_equal(var.axis<fstring>("abscissa"), fstring("m")) &&
            not_equal(var.axis<int>("ordinate"), 1)
        );

        auto masked_value = xtl::masked<xtl::xoptional<double, bool>>();
        EXPECT_EQ(masked_var.locate_element({"a", 12}), 6);
        EXPECT_EQ(const_masked_var.locate_element({"g", 5}), 35);
        EXPECT_EQ(masked_var.locate_element({"m", 5}), masked_value);
        EXPECT_EQ(masked_var.locate_element({"d", 1}), masked_value);
        EXPECT_ANY_THROW(masked_var.locate_element({"e", 4}));
    }

    TEST(xvariable_masked_view, scalar_assign)
    {
        variable_type var = make_test_view_variable();
        variable_type test_var = make_test_view_variable();
        auto expected = make_masked_data2();
        auto expected2 = make_masked_data3();

        auto masked_var = where(
            var,
            not_equal(var.axis<fstring>("abscissa"), fstring("m")) &&
            not_equal(var.axis<int>("ordinate"), 1)
        );

        // Check that the data didn't change
        ASSERT_EQ(var.data(), test_var.data());

        ASSERT_NE(var.data(), masked_var.data());

        ASSERT_EQ(expected, masked_var.data());

        masked_var = 5.2;

        ASSERT_EQ(expected2, masked_var.data());
    }

    TEST(xvariable_masked_view, variable_assign)
    {
        variable_type var = make_test_view_variable();
        variable_type test_var = make_test_view_variable();

        auto masked_var = where(
            var,
            not_equal(var.axis<fstring>("abscissa"), fstring("m")) &&
            not_equal(var.axis<int>("ordinate"), 1)
        );

        masked_var = test_var + 2;

        ASSERT_NE(masked_var.data(), test_var.data());
        ASSERT_NE(var.data(), test_var.data());
    }
}
