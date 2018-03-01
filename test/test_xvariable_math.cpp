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
    TEST(xvariable_math, plus)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(+(a.select(sel)), (+a).select(sel));
    }

    TEST(xvariable_math, minus)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(-(a.select(sel)), (-a).select(sel));
    }

    TEST(xvariable_math, add)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) + a.select(sel), (a + a).select(sel));

        double sb = 1.2;
        EXPECT_EQ(a.select(sel) + sb, (a + sb).select(sel));

        double sa = 1.2;
        EXPECT_EQ(sa + a.select(sel), (sa + a).select(sel));
    }

    TEST(xvariable_math, substract)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) - a.select(sel), (a - a).select(sel));

        double sb = 1.2;
        EXPECT_EQ(a.select(sel) - sb, (a - sb).select(sel));

        double sa = 1.2;
        EXPECT_EQ(sa - a.select(sel), (sa - a).select(sel));
    }

    TEST(xvariable_math, multiply)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) * a.select(sel), (a * a).select(sel));

        double sb = 1.2;
        EXPECT_EQ(a.select(sel) * sb, (a * sb).select(sel));

        double sa = 1.2;
        EXPECT_EQ(sa * a.select(sel), (sa * a).select(sel));
    }

    TEST(xvariable_math, divide)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) / a.select(sel), (a / a).select(sel));

        double sb = 1.2;
        EXPECT_EQ(a.select(sel) / sb, (a / sb).select(sel));

        double sa = 1.2;
        EXPECT_EQ(sa / a.select(sel), (sa / a).select(sel));
    }

    TEST(xvariable_math, modulus)
    {
        int_variable_type a = make_test_int_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) % a.select(sel), (a % a).select(sel));

        int sb = 2;
        EXPECT_EQ(a.select(sel) % sb, (a % sb).select(sel));

        int sa = 4;
        EXPECT_EQ(sa % a.select(sel), (sa % a).select(sel));
    }

    TEST(xvariable_math, logical_and)
    {
        bool_variable_type a = make_test_bool_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) && a.select(sel), (a && a).select(sel));

        bool sb = false;
        EXPECT_EQ(a.select(sel) && sb, (a && sb).select(sel));

        bool sa = false;
        EXPECT_EQ(sa && a.select(sel), (sa && a).select(sel));
    }

    TEST(xvariable_math, logical_or)
    {
        bool_variable_type a = make_test_bool_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) || a.select(sel), (a || a).select(sel));

        bool sb = false;
        EXPECT_EQ(a.select(sel) || sb, (a || sb).select(sel));

        bool sa = false;
        EXPECT_EQ(sa || a.select(sel), (sa || a).select(sel));
    }

    TEST(xvariable_math, logical_not)
    {
        bool_variable_type a = make_test_bool_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(!(a.select(sel)), (!a).select(sel));
    }

    TEST(xvariable_math, bitwise_and)
    {
        int_variable_type a = make_test_int_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) & a.select(sel), (a & a).select(sel));

        int sb = 4;
        EXPECT_EQ(a.select(sel) & sb, (a & sb).select(sel));

        int sa = 9;
        EXPECT_EQ(sa & a.select(sel), (sa & a).select(sel));
    }

    TEST(xvariable_math, bitwise_or)
    {
        int_variable_type a = make_test_int_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) | a.select(sel), (a | a).select(sel));

        int sb = 4;
        EXPECT_EQ(a.select(sel) | sb, (a | sb).select(sel));

        int sa = 9;
        EXPECT_EQ(sa | a.select(sel), (sa | a).select(sel));
    }

    TEST(xvariable_math, bitwise_xor)
    {
        int_variable_type a = make_test_int_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) ^ a.select(sel), (a ^ a).select(sel));

        int sb = 4;
        EXPECT_EQ(a.select(sel) ^ sb, (a ^ sb).select(sel));

        int sa = 9;
        EXPECT_EQ(sa ^ a.select(sel), (sa ^ a).select(sel));
    }

    TEST(xvariable_math, bitwise_not)
    {
        int_variable_type a = make_test_int_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(~(a.select(sel)), (~a).select(sel));
    }

    TEST(xvariable_math, equal)
    {
        variable_type a = make_test_variable();
        EXPECT_TRUE(a == a);
    }

    TEST(xvariable_math, not_equal)
    {
        variable_type a = make_test_variable();
        EXPECT_FALSE(a != a);
    }

    TEST(xvariable_math, less)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(xtl::xoptional<bool>(false), (a < a).select(sel));
    }

    TEST(xvariable_math, less_equal)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(xtl::xoptional<bool>(true), (a <= a).select(sel));
    }

    TEST(xvariable_math, greater)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(xtl::xoptional<bool>(false), (a > a).select(sel));
    }

    TEST(xvariable_math, greater_equal)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(xtl::xoptional<bool>(true), (a >= a).select(sel));
    }

    TEST(xvariable_math, element_wise_equal)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(xtl::xoptional<bool>(true), equal(a, a).select(sel));
    }

    TEST(xvariable_math, element_wise_not_equal)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(xtl::xoptional<bool>(false), not_equal(a, a).select(sel));
    }
}