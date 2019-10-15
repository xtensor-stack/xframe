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
    template <class T>
    struct DEBUG;

    TEST(xvariable_scalar, a_plus_b)
    {
        variable_type a = make_test_variable();
        selector_list sl = make_selector_list_aa();
        variable_type res = 2. + a;
        CHECK_SCALAR1_EQUALITY(res, 2, a, sl, +)
        variable_type res2 = a + 2.;
        CHECK_SCALAR2_EQUALITY(res2, a, 2, sl, +)
    }

    TEST(xvariable_scalar, a_minus_b)
    {
        variable_type a = make_test_variable();
        selector_list sl = make_selector_list_aa();
        variable_type res = 2. - a;
        CHECK_SCALAR1_EQUALITY(res, 2, a, sl, -)
        variable_type res2 = a - 2.;
        CHECK_SCALAR2_EQUALITY(res2, a, 2, sl, -)
    }

    TEST(xvariable_scalar, a_times_b)
    {
        variable_type a = make_test_variable();
        selector_list sl = make_selector_list_aa();
        variable_type res = 2. * a;
        CHECK_SCALAR1_EQUALITY(res, 2, a, sl, *)
        variable_type res2 = a * 2.;
        CHECK_SCALAR2_EQUALITY(res2, a, 2, sl, *)
    }

    TEST(xvariable_scalar, a_divided_by_b)
    {
        variable_type a = make_test_variable();
        selector_list sl = make_selector_list_aa();
        variable_type res = 2. / a;
        CHECK_SCALAR1_EQUALITY(res, 2, a, sl, /)
        variable_type res2 = a / 2.;
        CHECK_SCALAR2_EQUALITY(res2, a, 2, sl, /)
    }

    TEST(xvariable_scalar, a_plus_equal_b)
    {
        variable_type a = make_test_variable();
        variable_type b = a;
        selector_list sl = make_selector_list_aa();
        a += 2.;
        CHECK_SCALAR2_EQUALITY(a, b, 2, sl, +)
    }

    TEST(xvariable_scalar, a_minus_equal_b)
    {
        variable_type a = make_test_variable();
        variable_type b = a;
        selector_list sl = make_selector_list_aa();
        a -= 2.;
        CHECK_SCALAR2_EQUALITY(a, b, 2, sl, -)
    }

    TEST(xvariable_scalar, a_times_equal_b)
    {
        variable_type a = make_test_variable();
        variable_type b = a;
        selector_list sl = make_selector_list_aa();
        a *= 2.;
        CHECK_SCALAR2_EQUALITY(a, b, 2, sl, *)
    }

    TEST(xvariable_scalar, a_divided_by_equal_b)
    {
        variable_type a = make_test_variable();
        variable_type b = a;
        selector_list sl = make_selector_list_aa();
        a /= 2.;
        CHECK_SCALAR2_EQUALITY(a, b, 2, sl, /)
    }
}
