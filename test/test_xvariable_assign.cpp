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


#define CHECK_COMPOSED_EQUALITY(RES, A, B, SL)                                 \
    for(std::size_t i = 0; i < sl.size(); ++i)                                 \
    {                                                                          \
        EXPECT_EQ(RES.select(SL[i]), 3 * (A.select(SL[i]) + B.select(SL[i]))); \
    }

    TEST(xvariable_assign, composed_function)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a;
            EXPECT_EQ(res(0, 0), a(0, 0));
            res = 3 * (a + a);
            selector_list sl = make_selector_list_aa();
            CHECK_COMPOSED_EQUALITY(res, a, a, sl)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a;
            EXPECT_EQ(res(0, 0), a(0, 0));
            res = 3 * (a + b);
            selector_list sl = make_selector_list_ab();
            CHECK_COMPOSED_EQUALITY(res, a, b, sl)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = a;
            EXPECT_EQ(res(0, 0), a(0, 0));
            res = 3 * (c + d);
            selector_list sl = make_selector_list_cd();
            CHECK_COMPOSED_EQUALITY(res, c, d, sl)
        }
    }

    TEST(xvariable_assign, unsorted_labels)
    {
        auto a = variable_type(
            make_test_data(),
            {
                {"day", xf::axis({ "Monday", "Tuesday", "Wednesday" })},
                {"city", xf::axis({ "London", "Paris", "Bruxel" })}
            }
        );
        
        auto b = variable_type(
            make_test_data(),
            {
                { "day", xf::axis({ "Monday", "Tuesday", "Wednesday" })},
                { "city", xf::axis({ "Paris", "London", "Bruxel" })}
            }
        );

        variable_type res = a + b;
        EXPECT_EQ(res.locate("Monday", "Paris"), a.locate("Monday", "Paris") + b.locate("Monday", "Paris"));
        EXPECT_EQ(res.locate("Monday", "London"), a.locate("Monday", "London") + b.locate("Monday", "London"));
        EXPECT_EQ(res.locate("Monday", "Bruxel"), a.locate("Monday", "Bruxel") + b.locate("Monday", "Bruxel"));
        EXPECT_EQ(res.locate("Tuesday", "Paris"), a.locate("Tuesday", "Paris") + b.locate("Tuesday", "Paris"));
        EXPECT_EQ(res.locate("Tuesday", "London"), a.locate("Tuesday", "London") + b.locate("Tuesday", "London"));
        EXPECT_EQ(res.locate("Tuesday", "Bruxel"), a.locate("Tuesday", "Bruxel") + b.locate("Tuesday", "Bruxel"));
        EXPECT_EQ(res.locate("Wednesday", "Paris"), a.locate("Wednesday", "Paris") + b.locate("Wednesday", "Paris"));
        EXPECT_EQ(res.locate("Wednesday", "London"), a.locate("Wednesday", "London") + b.locate("Wednesday", "London"));
        EXPECT_EQ(res.locate("Wednesday", "Bruxel"), a.locate("Wednesday", "Bruxel") + b.locate("Wednesday", "Bruxel"));
    }
}

