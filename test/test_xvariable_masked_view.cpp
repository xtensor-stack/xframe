/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay, Wolf Vollprecht and   *
* Martin Renou                                                             *
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

        // expected =                   ordinate
        //                 1,   2,   4,   5,   6,   8,  12,  13
        //           a {{  0,   1,   2,   3, N/A, N/A, N/A, N/A},
        //           c  {  8,   9,  10,  11, N/A, N/A, N/A, N/A},
        //           d  { 16,  17,  18, N/A, N/A, N/A, N/A, N/A},
        // abscissa  f  { 24,  25,  26,  27, N/A, N/A, N/A, N/A},
        //           g  { 32,  33,  34,  35, N/A, N/A, N/A, N/A},
        //           h  { 40,  41,  42,  43, N/A, N/A, N/A, N/A},
        //           m  { 48,  49,  50,  51, N/A, N/A, N/A, N/A},
        //           n  { 56,  57,  58,  59, N/A, N/A, N/A, N/A}
        variable_type expected = make_test_view_variable();
        for (std::size_t a = 0; a < expected.shape()[0]; ++a)
        {
            for (std::size_t o = 4; o < expected.shape()[1]; ++o)
            {
                expected(a, o) = xtl::missing<double>();
            }
        }

        for (std::size_t a = 0; a < expected.shape()[0]; ++a)
        {
            for (std::size_t o = 0; o < expected.shape()[1]; ++o)
            {
                EXPECT_EQ(masked_var.iselect({{"abscissa", a}, {"ordinate", o}}), expected(a, o));
            }
        }

        auto masked_var2 = where(var, not_equal(var.axis<fstring>("abscissa"), fstring("m")) && not_equal(var.axis<int>("ordinate"), 1));

        // expected2 =                  ordinate
        //                 1,   2,   4,   5,   6,   8,  12,  13
        //           a {{N/A,   1,   2,   3,   4,   5,   6,   7},
        //           c  {N/A,   9,  10,  11,  12,  13,  14,  15},
        //           d  {N/A,  17,  18, N/A, N/A,  21,  22,  23},
        // abscissa  f  {N/A,  25,  26,  27,  28,  29,  30,  31},
        //           g  {N/A,  33,  34,  35,  36,  37,  38,  39},
        //           h  {N/A,  41,  42,  43,  44,  45,  46,  47},
        //           m  {N/A, N/A, N/A, N/A, N/A, N/A, N/A, N/A},
        //           n  {N/A,  57,  58,  59,  60,  61,  62,  63}
        variable_type expected2 = make_test_view_variable();
        for (std::size_t o = 0; o < expected2.shape()[1]; ++o)
        {
            expected2(6, o) = xtl::missing<double>();
        }
        for (std::size_t a = 0; a < expected2.shape()[0]; ++a)
        {
            expected2(a, 0) = xtl::missing<double>();
        }

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

        auto expected = masked_var.select<join::inner, 2>({{{"abscissa", "a"}, {"ordinate", 12}}});
        EXPECT_EQ(expected, 6);
        EXPECT_EQ(masked_var.select({{"abscissa", "g"}, {"ordinate", 5}}), 35);

        EXPECT_EQ(masked_var.select({{"abscissa", "d"}, {"ordinate", 1}}), masked_var.missing());
        EXPECT_EQ(masked_var.select({{"abscissa", "m"}, {"ordinate", 5}}), masked_var.missing());

        auto val = std::is_same<typename decltype(var)::reference, decltype(masked_var.select({{"abscissa", "g"}, {"ordinate", 5}}))>::value;
        EXPECT_TRUE(val);
        val = std::is_same<typename decltype(var)::const_reference, decltype(const_masked_var.select({{"abscissa", "g"}, {"ordinate", 5}}))>::value;
        EXPECT_TRUE(val);

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

        auto expected = masked_var.select<join::outer, 2>({{{"abscissa", "a"}, {"ordinate", 12}}});
        EXPECT_EQ(expected, 6);
        EXPECT_EQ(masked_var.select<join::outer>({{"abscissa", "g"}, {"ordinate", 5}}), 35);
        EXPECT_EQ(masked_var.select<join::outer>({{"abscissa", "m"}, {"ordinate", 5}}), masked_var.missing());
        EXPECT_EQ(masked_var.select<join::outer>({{"abscissa", "d"}, {"ordinate", 1}}), masked_var.missing());

        EXPECT_EQ(masked_var.select<join::outer>({{"abscissa", "e"}, {"ordinate", 4}}), masked_var.missing());
        EXPECT_EQ(masked_var.select<join::outer>({{"abscissa", "a"}, {"ordinate", 3}}), masked_var.missing());
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

        auto val = std::is_same<typename decltype(var)::reference, decltype(masked_var(0, 6))>::value;
        EXPECT_TRUE(val);
        val = std::is_same<typename decltype(var)::const_reference, decltype(const_masked_var(0, 6))>::value;
        EXPECT_TRUE(val);

        EXPECT_EQ(masked_var(0, 6), 6);
        masked_var(0, 6) = 7;
        EXPECT_EQ(masked_var(0, 6), 7);

        EXPECT_EQ(masked_var(4, 3), 35);
        EXPECT_EQ(const_masked_var(4, 3), 35);
        EXPECT_EQ(masked_var(6, 3), masked_var.missing());
        EXPECT_EQ(masked_var(2, 0), masked_var.missing());
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

        auto val = std::is_same<typename decltype(var)::reference, decltype(masked_var.element({6, 3}))>::value;
        EXPECT_TRUE(val);
        val = std::is_same<typename decltype(var)::const_reference, decltype(const_masked_var.element({6, 3}))>::value;
        EXPECT_TRUE(val);

        EXPECT_EQ(masked_var.element<2>({0, 6}), 6);
        EXPECT_EQ(const_masked_var.element({4, 3}), 35);
        EXPECT_EQ(masked_var.element({6, 3}), masked_var.missing());
        EXPECT_EQ(masked_var.element({2, 0}), masked_var.missing());
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

        EXPECT_EQ(masked_var.locate("a", 12), 6);
        EXPECT_EQ(const_masked_var.locate("g", 5), 35);
        EXPECT_EQ(masked_var.locate("m", 5), masked_var.missing());
        EXPECT_EQ(masked_var.locate("d", 1), masked_var.missing());
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

        EXPECT_EQ(masked_var.locate_element({"a", 12}), 6);
        EXPECT_EQ(const_masked_var.locate_element({"g", 5}), 35);
        EXPECT_EQ(masked_var.locate_element({"m", 5}), masked_var.missing());
        EXPECT_EQ(masked_var.locate_element({"d", 1}), masked_var.missing());
        EXPECT_ANY_THROW(masked_var.locate_element({"e", 4}));
    }
}
