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
    using dict_type = typename variable_type::selector_map_type<>;
    using selector_list = std::vector<dict_type>;

    inline selector_list make_selector_list_aa()
    {
        selector_list sl(9);
        sl[0] = {{"abscissa", "a"}, {"ordinate", 1}};
        sl[1] = {{"abscissa", "a"}, {"ordinate", 2}};
        sl[2] = {{"abscissa", "a"}, {"ordinate", 4}};
        sl[3] = {{"abscissa", "c"}, {"ordinate", 1}};
        sl[4] = {{"abscissa", "c"}, {"ordinate", 2}};
        sl[5] = {{"abscissa", "c"}, {"ordinate", 4}};
        sl[6] = {{"abscissa", "d"}, {"ordinate", 1}};
        sl[7] = {{"abscissa", "d"}, {"ordinate", 2}};
        sl[8] = {{"abscissa", "d"}, {"ordinate", 4}};
        return sl;
    }

    inline selector_list make_selector_list_ab()
    {
        selector_list sl(12);
        sl[0]  = {{"abscissa", "a"}, {"ordinate", 1}, {"altitude", 1}};
        sl[1]  = {{"abscissa", "a"}, {"ordinate", 1}, {"altitude", 2}};
        sl[2]  = {{"abscissa", "a"}, {"ordinate", 1}, {"altitude", 4}};
        sl[3]  = {{"abscissa", "a"}, {"ordinate", 4}, {"altitude", 1}};
        sl[4]  = {{"abscissa", "a"}, {"ordinate", 4}, {"altitude", 2}};
        sl[5]  = {{"abscissa", "a"}, {"ordinate", 4}, {"altitude", 4}};
        sl[6]  = {{"abscissa", "d"}, {"ordinate", 1}, {"altitude", 1}};
        sl[7]  = {{"abscissa", "d"}, {"ordinate", 1}, {"altitude", 2}};
        sl[8]  = {{"abscissa", "d"}, {"ordinate", 1}, {"altitude", 4}};
        sl[9]  = {{"abscissa", "d"}, {"ordinate", 4}, {"altitude", 1}};
        sl[10] = {{"abscissa", "d"}, {"ordinate", 4}, {"altitude", 2}};
        sl[11] = {{"abscissa", "d"}, {"ordinate", 4}, {"altitude", 4}};
        return sl;
    }

    inline selector_list make_selector_list_cd()
    {
        selector_list sl(27);
        sl[0]  = {{"altitude", 1}, {"abscissa", "a"}, {"ordinate", 1}};
        sl[1]  = {{"altitude", 1}, {"abscissa", "a"}, {"ordinate", 4}};
        sl[2]  = {{"altitude", 1}, {"abscissa", "a"}, {"ordinate", 5}};
        sl[3]  = {{"altitude", 1}, {"abscissa", "d"}, {"ordinate", 1}};
        sl[4]  = {{"altitude", 1}, {"abscissa", "d"}, {"ordinate", 4}};
        sl[5]  = {{"altitude", 1}, {"abscissa", "d"}, {"ordinate", 5}};
        sl[6]  = {{"altitude", 1}, {"abscissa", "e"}, {"ordinate", 1}};
        sl[7]  = {{"altitude", 1}, {"abscissa", "e"}, {"ordinate", 4}};
        sl[8]  = {{"altitude", 1}, {"abscissa", "e"}, {"ordinate", 5}};
        sl[9]  = {{"altitude", 2}, {"abscissa", "a"}, {"ordinate", 1}};
        sl[10] = {{"altitude", 2}, {"abscissa", "a"}, {"ordinate", 4}};
        sl[11] = {{"altitude", 2}, {"abscissa", "a"}, {"ordinate", 5}};
        sl[12] = {{"altitude", 2}, {"abscissa", "d"}, {"ordinate", 1}};
        sl[13] = {{"altitude", 2}, {"abscissa", "d"}, {"ordinate", 4}};
        sl[14] = {{"altitude", 2}, {"abscissa", "d"}, {"ordinate", 5}};
        sl[15] = {{"altitude", 2}, {"abscissa", "e"}, {"ordinate", 1}};
        sl[16] = {{"altitude", 2}, {"abscissa", "e"}, {"ordinate", 4}};
        sl[17] = {{"altitude", 2}, {"abscissa", "e"}, {"ordinate", 5}};
        sl[18] = {{"altitude", 4}, {"abscissa", "a"}, {"ordinate", 1}};
        sl[19] = {{"altitude", 4}, {"abscissa", "a"}, {"ordinate", 4}};
        sl[20] = {{"altitude", 4}, {"abscissa", "a"}, {"ordinate", 5}};
        sl[21] = {{"altitude", 4}, {"abscissa", "d"}, {"ordinate", 1}};
        sl[22] = {{"altitude", 4}, {"abscissa", "d"}, {"ordinate", 4}};
        sl[23] = {{"altitude", 4}, {"abscissa", "d"}, {"ordinate", 5}};
        sl[24] = {{"altitude", 4}, {"abscissa", "e"}, {"ordinate", 1}};
        sl[25] = {{"altitude", 4}, {"abscissa", "e"}, {"ordinate", 4}};
        sl[26] = {{"altitude", 4}, {"abscissa", "e"}, {"ordinate", 5}};
        return sl;
    }

#define DEFINE_TEST_VARIABLES()                                           \
        variable_type a = make_test_variable();                           \
        variable_type b = make_test_variable2();                          \
        variable_type c = make_test_variable3();                          \
        variable_type d = make_test_variable4()

#define CHECK_EQUALITY(RES, A, B, SL, OP)                                 \
    for(std::size_t i = 0; i < sl.size(); ++i)                            \
    {                                                                     \
        EXPECT_EQ(RES.select(SL[i]), A.select(SL[i]) OP B.select(SL[i])); \
    }

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
}

