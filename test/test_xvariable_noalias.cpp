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

namespace xf
{
    TEST(xvariable_noalias, a_plus_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a;
            noalias(res) = a + a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, +)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a;
            noalias(res) = a + b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, +)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = c;
            noalias(res) = c + d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, +)
        }
    }

    TEST(xvariable_noalias, a_minus_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a;
            noalias(res) = a - a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, -)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a;
            noalias(res) = a - b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, -)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = c;
            noalias(res) = c - d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, -)
        }
    }

    TEST(xvariable_noalias, a_times_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a;
            noalias(res) = a * a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, *)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a;
            noalias(res) = a * b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, *)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = c;
            noalias(res) = c * d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, *)
        }
    }

    TEST(xvariable_noalias, a_divided_by__b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a;
            noalias(res) = a / a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, /)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a;
            noalias(res) = a / b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, /)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = c;
            noalias(res) = c / d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, /)
        }
    }

    TEST(xvariable_noalias, a_plus_equal_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a;
            noalias(res) += a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, +)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a;
            noalias(res) += b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, +)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = c;
            noalias(res) += d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, +)
        }
    }

    TEST(xvariable_noalias, a_minus_equal_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a;
            noalias(res) -= a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, -)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a;
            noalias(res) -= b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, -)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = c;
            noalias(res) -= d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, -)
        }
    }

    TEST(xvariable_noalias, a_times_equal_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a;
            noalias(res) *= a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, *)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a;
            noalias(res) *= b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, *)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = c;
            noalias(res) *= d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, *)
        }
    }

    TEST(xvariable_noalias, a_divided_by_equal_b)
    {
        DEFINE_TEST_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            variable_type res = a;
            noalias(res) /= a;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(res, a, a, sl, /)
        }

        {
            SCOPED_TRACE("different coordinates");
            variable_type res = a;
            noalias(res) /= b;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(res, a, b, sl, /)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            variable_type res = c;
            noalias(res) /= d;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(res, c, d, sl, /)
        }
    }
}
