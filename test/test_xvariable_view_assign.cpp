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
#include "xframe/xvariable_view.hpp"

namespace xf
{
#define DEFINE_TEST_VIEW_VARIABLES()                                                              \
    DEFINE_TEST_VARIABLES();                                                                      \
    variable_type resaa =  3 * a;                                                                 \
    variable_type resab = 3 * (a + b);                                                            \
    variable_type rescd = 3 * (c + d);                                                            \
    auto va = select(a, { { "abscissa", range("a", "d") },{ "ordinate", range(1, 4) } });         \
    auto vresaa = select(resaa, { { "abscissa", range("a", "d") },{ "ordinate", range(1, 4) } }); \
    auto vb = select(b, { { "abscissa", range("a", "d") },{ "ordinate", range(1, 4) } });         \
    auto vresab = select(resab, { { "abscissa", range("a", "d") },{ "ordinate", range(1, 4) } }); \
    auto vc = select(c, {{"abscissa", range("a", "e")}, {"ordinate", range(1, 5)}});              \
    auto vd = select(d, {{"abscissa", range("a", "e")}, {"ordinate", range(1, 5)}});              \
    auto vrescd = select(rescd, {{"abscissa", range("a", "e")}, {"ordinate", range(1, 5)}})

    TEST(xvariable_view_assign, a_plus_b)
    {
        DEFINE_TEST_VIEW_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            vresaa = va + va;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(vresaa, va, va, sl, +)
        }

        {
            SCOPED_TRACE("different coordinates");
            vresab = va + vb;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(vresab, va, vb, sl, +)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            vrescd = vc + vd;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(vrescd, vc, vd, sl, +)
        }
    }

    TEST(xvariable_view_assign, assign_a_plus_b)
    {
        DEFINE_TEST_VIEW_VARIABLES();
        {
            SCOPED_TRACE("same coordinate");
            xt::noalias(vresaa) = va + va;
            selector_list sl = make_selector_list_aa();
            CHECK_EQUALITY(vresaa, va, va, sl, +)
        }

        {
            SCOPED_TRACE("different coordinates");
            xt::noalias(vresab) = va + vb;
            selector_list sl = make_selector_list_ab();
            CHECK_EQUALITY(vresab, va, vb, sl, +)
        }

        {
            SCOPED_TRACE("broadcasting coordinates");
            xt::noalias(vrescd) = vc + vd;
            selector_list sl = make_selector_list_cd();
            CHECK_EQUALITY(vrescd, vc, vd, sl, +)
        }

        {
            SCOPED_TRACE("Incompatible shapes");
            EXPECT_ANY_THROW(xt::noalias(vresaa) = va + vb);
        }
    }
}
