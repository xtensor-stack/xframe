/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"
#include "test_fixture.hpp"
#include "xframe/xvariable_view.hpp"

#define DEFINE_TEST_VIEW_VARIABLES                                                              \
    DEFINE_TEST_VARIABLES();                                                                    \
    variable_type resaa =  3 * a;                                                               \
    variable_type resab = 3 * (a + b);                                                          \
    variable_type rescd = 3 * (c + d);                                                          \
    auto va = select(a, {{"abscissa", range("a", "d")}, {"ordinate", range(1, 4, 2)}});         \
    auto vresaa = select(resaa, {{"abscissa", range("a", "d")}, {"ordinate", range(1, 4, 2)}})

namespace xf
{
    /*template <class T>
    struct PRINT;
    TEST(xvariable_view_asign, compilation)
    {
        variable_type a = make_test_variable();
        variable_type res = a;
        auto va = select(a, { { "abscissa", range("a", "d") },{ "ordinate", range(1, 4, 2) } });
        auto vres = select(res, { { "abscissa", range("a", "d") },{ "ordinate", range(1, 4, 2) } });
        vres = va + va;
    }*/

    /*TEST(xvariable_view_assign, a_plus_b)
    {
        DEFINE_TEST_VIEW_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            vresaa = va + va;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(vresaa, va, va, sl, +)
        }
    }*/
}
