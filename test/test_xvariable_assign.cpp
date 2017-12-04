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

    TEST(xvariable_assign, a_plus_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a + a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, +)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a + b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, +)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = c + d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, +)
        }
    }

    TEST(xvariable_assign, a_minus_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a - a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, -)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a - b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, -)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = c - d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, -)
        }
    }

    TEST(xvariable_assign, a_times_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a * a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, *)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a * b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, *)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = c * d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, *)
        }
    }

    TEST(xvariable_assign, a_divided_by_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a / a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, /)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a / b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, /)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = c / d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, /)
        }
    }

    TEST(xvariable_assign, a_plus_equal_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a;
            res += a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, +)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a;
            res += b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, +)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = c;
            res += d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, +)
        }
    }

    TEST(xvariable_assign, a_minus_equal_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a;
            res -= a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, -)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a;
            res -= b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, -)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = c;
            res -= d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, -)
        }
    }

    TEST(xvariable_assign, a_times_equal_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a;
            res *= a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, *)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a;
            res *= b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, *)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = c;
            res *= d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, *)
        }
    }

    TEST(xvariable_assign, a_divided_by_equal_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a;
            res /= a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, /)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a;
            res /= b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, /)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = c;
            res /= d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, /)
        }
    }

    TEST(xvariable_assign, assign_a_plus_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a;
            EXPECT_EQ(res(0, 0), a(0, 0));
            res = a + a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, +)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a;
            EXPECT_EQ(res(0, 0), a(0, 0));
            res = a + b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, +)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = a;
            EXPECT_EQ(res(0, 0), a(0, 0));
            res = c + d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, +)
        }
    }

    TEST(xvariable_assign, assign_a_minus_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a;
            EXPECT_EQ(res(0, 0), a(0, 0));
            res = a - a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, -)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a;
            EXPECT_EQ(res(0, 0), a(0, 0));
            res = a - b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, -)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = a;
            EXPECT_EQ(res(0, 0), a(0, 0));
            res = c - d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, -)
        }
    }

    TEST(xvariable_assign, assign_a_times_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a;
            EXPECT_EQ(res(0, 0), a(0, 0));
            res = a * a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, *)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a;
            EXPECT_EQ(res(0, 0), a(0, 0));
            res = a * b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, *)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = a;
            EXPECT_EQ(res(0, 0), a(0, 0));
            res = c * d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, *)
        }
    }

    TEST(xvariable_assign, assign_a_divided_by_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a;
            EXPECT_EQ(res(0, 0), a(0, 0));
            res = a / a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, /)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a;
            EXPECT_EQ(res(0, 0), a(0, 0));
            res = a / b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, /)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = a;
            EXPECT_EQ(res(0, 0), a(0, 0));
            res = c / d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, /)
        }
    }
}

