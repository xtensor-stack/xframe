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

namespace xf
{
    TEST(xvariable_view, select_builder)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = build_view(var);
        variable_view_type view2 = select(var, {{ "abscissa", range("f", "n") }, { "ordinate", range(1, 6, 2) }});
        EXPECT_EQ(view, view2);
    }

    TEST(xvariable_view, size)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = build_view(var);
        EXPECT_EQ(view.size(), 15u);
    }

    TEST(xvariable_view, dimension)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = build_view(var);
        EXPECT_EQ(2u, view.dimension());
    }

    TEST(xvariable_view, dimension_labels)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = build_view(var);
        saxis_type s = { "abscissa", "ordinate" };

        EXPECT_EQ(view.dimension_labels(), s.labels());
    }

    TEST(xvariable_view, access)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = build_view(var);

        auto vi00 = view(0, 0);
        auto vi01 = view(0, 1);
        auto vi02 = view(0, 2);
        auto vi10 = view(1, 0);
        auto vi11 = view(1, 1);
        auto vi12 = view(1, 2);
        auto vi20 = view(2, 0);
        auto vi21 = view(2, 1);
        auto vi22 = view(2, 2);
        auto vi30 = view(3, 0);
        auto vi31 = view(3, 1);
        auto vi32 = view(3, 2);
        auto vi40 = view(4, 0);
        auto vi41 = view(4, 1);
        auto vi42 = view(4, 2);

        auto v00 = var(3, 0);
        auto v01 = var(3, 2);
        auto v02 = var(3, 4);
        auto v10 = var(4, 0);
        auto v11 = var(4, 2);
        auto v12 = var(4, 4);
        auto v20 = var(5, 0);
        auto v21 = var(5, 2);
        auto v22 = var(5, 4);
        auto v30 = var(6, 0);
        auto v31 = var(6, 2);
        auto v32 = var(6, 4);
        auto v40 = var(7, 0);
        auto v41 = var(7, 2);
        auto v42 = var(7, 4);

        EXPECT_EQ(v00, vi00);
        EXPECT_EQ(v01, vi01);
        EXPECT_EQ(v02, vi02);
        EXPECT_EQ(v10, vi10);
        EXPECT_EQ(v11, vi11);
        EXPECT_EQ(v12, vi12);
        EXPECT_EQ(v20, vi20);
        EXPECT_EQ(v21, vi21);
        EXPECT_EQ(v22, vi22);
        EXPECT_EQ(v30, vi30);
        EXPECT_EQ(v31, vi31);
        EXPECT_EQ(v32, vi32);
        EXPECT_EQ(v40, vi40);
        EXPECT_EQ(v41, vi41);
        EXPECT_EQ(v42, vi42);
    }

    TEST(xvariable_view, element)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = build_view(var);

        EXPECT_EQ(view.element({ 0, 0 }), var(3, 0));
        EXPECT_EQ(view.element({ 0, 1 }), var(3, 2));
        EXPECT_EQ(view.element({ 0, 2 }), var(3, 4));
        EXPECT_EQ(view.element({ 1, 0 }), var(4, 0));
        EXPECT_EQ(view.element({ 1, 1 }), var(4, 2));
        EXPECT_EQ(view.element({ 1, 2 }), var(4, 4));
        EXPECT_EQ(view.element({ 2, 0 }), var(5, 0));
        EXPECT_EQ(view.element({ 2, 1 }), var(5, 2));
        EXPECT_EQ(view.element({ 2, 2 }), var(5, 4));
        EXPECT_EQ(view.element({ 3, 0 }), var(6, 0));
        EXPECT_EQ(view.element({ 3, 1 }), var(6, 2));
        EXPECT_EQ(view.element({ 3, 2 }), var(6, 4));
        EXPECT_EQ(view.element({ 4, 0 }), var(7, 0));
        EXPECT_EQ(view.element({ 4, 1 }), var(7, 2));
        EXPECT_EQ(view.element({ 4, 2 }), var(7, 4));
    }

    TEST(xvariable_view, select_inner)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = build_view(var);

        auto vi00 = view.select({ { "abscissa", "f" },{ "ordinate", 1 } });
        auto vi01 = view.select({ { "abscissa", "f" },{ "ordinate", 4 } });
        auto vi02 = view.select({ { "abscissa", "f" },{ "ordinate", 6 } });
        auto vi10 = view.select({ { "abscissa", "g" },{ "ordinate", 1 } });
        auto vi11 = view.select({ { "abscissa", "g" },{ "ordinate", 4 } });
        auto vi12 = view.select({ { "abscissa", "g" },{ "ordinate", 6 } });
        auto vi20 = view.select({ { "abscissa", "h" },{ "ordinate", 1 } });
        auto vi21 = view.select({ { "abscissa", "h" },{ "ordinate", 4 } });
        auto vi22 = view.select({ { "abscissa", "h" },{ "ordinate", 6 } });
        auto vi30 = view.select({ { "abscissa", "m" },{ "ordinate", 1 } });
        auto vi31 = view.select({ { "abscissa", "m" },{ "ordinate", 4 } });
        auto vi32 = view.select({ { "abscissa", "m" },{ "ordinate", 6 } });
        auto vi40 = view.select({ { "abscissa", "n" },{ "ordinate", 1 } });
        auto vi41 = view.select({ { "abscissa", "n" },{ "ordinate", 4 } });
        auto vi42 = view.select({ { "abscissa", "n" },{ "ordinate", 6 } });

        auto v00 = var.select({ { "abscissa", "f" },{ "ordinate", 1 } });
        auto v01 = var.select({ { "abscissa", "f" },{ "ordinate", 4 } });
        auto v02 = var.select({ { "abscissa", "f" },{ "ordinate", 6 } });
        auto v10 = var.select({ { "abscissa", "g" },{ "ordinate", 1 } });
        auto v11 = var.select({ { "abscissa", "g" },{ "ordinate", 4 } });
        auto v12 = var.select({ { "abscissa", "g" },{ "ordinate", 6 } });
        auto v20 = var.select({ { "abscissa", "h" },{ "ordinate", 1 } });
        auto v21 = var.select({ { "abscissa", "h" },{ "ordinate", 4 } });
        auto v22 = var.select({ { "abscissa", "h" },{ "ordinate", 6 } });
        auto v30 = var.select({ { "abscissa", "m" },{ "ordinate", 1 } });
        auto v31 = var.select({ { "abscissa", "m" },{ "ordinate", 4 } });
        auto v32 = var.select({ { "abscissa", "m" },{ "ordinate", 6 } });
        auto v40 = var.select({ { "abscissa", "n" },{ "ordinate", 1 } });
        auto v41 = var.select({ { "abscissa", "n" },{ "ordinate", 4 } });
        auto v42 = var.select({ { "abscissa", "n" },{ "ordinate", 6 } });

        EXPECT_EQ(v00, vi00);
        EXPECT_EQ(v01, vi01);
        EXPECT_EQ(v02, vi02);
        EXPECT_EQ(v10, vi10);
        EXPECT_EQ(v11, vi11);
        EXPECT_EQ(v12, vi12);
        EXPECT_EQ(v20, vi20);
        EXPECT_EQ(v21, vi21);
        EXPECT_EQ(v22, vi22);
        EXPECT_EQ(v30, vi30);
        EXPECT_EQ(v31, vi31);
        EXPECT_EQ(v32, vi32);
        EXPECT_EQ(v40, vi40);
        EXPECT_EQ(v41, vi41);
        EXPECT_EQ(v42, vi42);

        auto vi100 = view.select({{ "abscissa", "f" },{ "ordinate", 1 }, { "altitude", 1 }});
        EXPECT_EQ(vi100, vi00);

        EXPECT_NO_THROW(var.select({{ "abscissa", "d" }, { "ordinate", 1 }}));
        EXPECT_ANY_THROW(view.select({{ "abscissa", "d" }, { "ordinate", 1 }}));
        EXPECT_ANY_THROW(view.select({{ "abscissa", "e" }, { "ordinate", 1 }}));
    }

    TEST(xvariable_view, select_outer)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = build_view(var);

        auto vi00 = view.select<join::outer>({ { "abscissa", "f" },{ "ordinate", 1 } });
        auto vi01 = view.select<join::outer>({ { "abscissa", "f" },{ "ordinate", 4 } });
        auto vi02 = view.select<join::outer>({ { "abscissa", "f" },{ "ordinate", 6 } });
        auto vi10 = view.select<join::outer>({ { "abscissa", "g" },{ "ordinate", 1 } });
        auto vi11 = view.select<join::outer>({ { "abscissa", "g" },{ "ordinate", 4 } });
        auto vi12 = view.select<join::outer>({ { "abscissa", "g" },{ "ordinate", 6 } });
        auto vi20 = view.select<join::outer>({ { "abscissa", "h" },{ "ordinate", 1 } });
        auto vi21 = view.select<join::outer>({ { "abscissa", "h" },{ "ordinate", 4 } });
        auto vi22 = view.select<join::outer>({ { "abscissa", "h" },{ "ordinate", 6 } });
        auto vi30 = view.select<join::outer>({ { "abscissa", "m" },{ "ordinate", 1 } });
        auto vi31 = view.select<join::outer>({ { "abscissa", "m" },{ "ordinate", 4 } });
        auto vi32 = view.select<join::outer>({ { "abscissa", "m" },{ "ordinate", 6 } });
        auto vi40 = view.select<join::outer>({ { "abscissa", "n" },{ "ordinate", 1 } });
        auto vi41 = view.select<join::outer>({ { "abscissa", "n" },{ "ordinate", 4 } });
        auto vi42 = view.select<join::outer>({ { "abscissa", "n" },{ "ordinate", 6 } });

        auto v00 = var.select({ { "abscissa", "f" },{ "ordinate", 1 } });
        auto v01 = var.select({ { "abscissa", "f" },{ "ordinate", 4 } });
        auto v02 = var.select({ { "abscissa", "f" },{ "ordinate", 6 } });
        auto v10 = var.select({ { "abscissa", "g" },{ "ordinate", 1 } });
        auto v11 = var.select({ { "abscissa", "g" },{ "ordinate", 4 } });
        auto v12 = var.select({ { "abscissa", "g" },{ "ordinate", 6 } });
        auto v20 = var.select({ { "abscissa", "h" },{ "ordinate", 1 } });
        auto v21 = var.select({ { "abscissa", "h" },{ "ordinate", 4 } });
        auto v22 = var.select({ { "abscissa", "h" },{ "ordinate", 6 } });
        auto v30 = var.select({ { "abscissa", "m" },{ "ordinate", 1 } });
        auto v31 = var.select({ { "abscissa", "m" },{ "ordinate", 4 } });
        auto v32 = var.select({ { "abscissa", "m" },{ "ordinate", 6 } });
        auto v40 = var.select({ { "abscissa", "n" },{ "ordinate", 1 } });
        auto v41 = var.select({ { "abscissa", "n" },{ "ordinate", 4 } });
        auto v42 = var.select({ { "abscissa", "n" },{ "ordinate", 6 } });

        EXPECT_EQ(v00, vi00);
        EXPECT_EQ(v01, vi01);
        EXPECT_EQ(v02, vi02);
        EXPECT_EQ(v10, vi10);
        EXPECT_EQ(v11, vi11);
        EXPECT_EQ(v12, vi12);
        EXPECT_EQ(v20, vi20);
        EXPECT_EQ(v21, vi21);
        EXPECT_EQ(v22, vi22);
        EXPECT_EQ(v30, vi30);
        EXPECT_EQ(v31, vi31);
        EXPECT_EQ(v32, vi32);
        EXPECT_EQ(v40, vi40);
        EXPECT_EQ(v41, vi41);
        EXPECT_EQ(v42, vi42);

        auto vi100 = view.select<join::outer>({{ "abscissa", "f" },{ "ordinate", 1 },{ "altitude", 1 }});
        EXPECT_EQ(vi100, vi00);

        auto mis = view.select<join::outer>({{ "abscissa", "e" },{ "ordinate", 1 }});
        EXPECT_EQ(mis, view.missing());
    }

    TEST(xvariable_view, iselect)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = build_view(var);

        auto vi00 = view.iselect({ { "abscissa", 0 },{ "ordinate", 0 } });
        auto vi01 = view.iselect({ { "abscissa", 0 },{ "ordinate", 1 } });
        auto vi02 = view.iselect({ { "abscissa", 0 },{ "ordinate", 2 } });
        auto vi10 = view.iselect({ { "abscissa", 1 },{ "ordinate", 0 } });
        auto vi11 = view.iselect({ { "abscissa", 1 },{ "ordinate", 1 } });
        auto vi12 = view.iselect({ { "abscissa", 1 },{ "ordinate", 2 } });
        auto vi20 = view.iselect({ { "abscissa", 2 },{ "ordinate", 0 } });
        auto vi21 = view.iselect({ { "abscissa", 2 },{ "ordinate", 1 } });
        auto vi22 = view.iselect({ { "abscissa", 2 },{ "ordinate", 2 } });
        auto vi30 = view.iselect({ { "abscissa", 3 },{ "ordinate", 0 } });
        auto vi31 = view.iselect({ { "abscissa", 3 },{ "ordinate", 1 } });
        auto vi32 = view.iselect({ { "abscissa", 3 },{ "ordinate", 2 } });
        auto vi40 = view.iselect({ { "abscissa", 4 },{ "ordinate", 0 } });
        auto vi41 = view.iselect({ { "abscissa", 4 },{ "ordinate", 1 } });
        auto vi42 = view.iselect({ { "abscissa", 4 },{ "ordinate", 2 } });

        auto v00 = var.iselect({ { "abscissa", 3 },{ "ordinate", 0 } });
        auto v01 = var.iselect({ { "abscissa", 3 },{ "ordinate", 2 } });
        auto v02 = var.iselect({ { "abscissa", 3 },{ "ordinate", 4 } });
        auto v10 = var.iselect({ { "abscissa", 4 },{ "ordinate", 0 } });
        auto v11 = var.iselect({ { "abscissa", 4 },{ "ordinate", 2 } });
        auto v12 = var.iselect({ { "abscissa", 4 },{ "ordinate", 4 } });
        auto v20 = var.iselect({ { "abscissa", 5 },{ "ordinate", 0 } });
        auto v21 = var.iselect({ { "abscissa", 5 },{ "ordinate", 2 } });
        auto v22 = var.iselect({ { "abscissa", 5 },{ "ordinate", 4 } });
        auto v30 = var.iselect({ { "abscissa", 6 },{ "ordinate", 0 } });
        auto v31 = var.iselect({ { "abscissa", 6 },{ "ordinate", 2 } });
        auto v32 = var.iselect({ { "abscissa", 6 },{ "ordinate", 4 } });
        auto v40 = var.iselect({ { "abscissa", 7 },{ "ordinate", 0 } });
        auto v41 = var.iselect({ { "abscissa", 7 },{ "ordinate", 2 } });
        auto v42 = var.iselect({ { "abscissa", 7 },{ "ordinate", 4 } });

        EXPECT_EQ(v00, vi00);
        EXPECT_EQ(v01, vi01);
        EXPECT_EQ(v02, vi02);
        EXPECT_EQ(v10, vi10);
        EXPECT_EQ(v11, vi11);
        EXPECT_EQ(v12, vi12);
        EXPECT_EQ(v20, vi20);
        EXPECT_EQ(v21, vi21);
        EXPECT_EQ(v22, vi22);
        EXPECT_EQ(v30, vi30);
        EXPECT_EQ(v31, vi31);
        EXPECT_EQ(v32, vi32);
        EXPECT_EQ(v40, vi40);
        EXPECT_EQ(v41, vi41);
        EXPECT_EQ(v42, vi42);
    }

    TEST(xvariable_view, locate)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = build_view(var);

        auto vi00 = view.locate("f", 1);
        auto vi01 = view.locate("f", 4);
        auto vi02 = view.locate("f", 6);
        auto vi10 = view.locate("g", 1);
        auto vi11 = view.locate("g", 4);
        auto vi12 = view.locate("g", 6);
        auto vi20 = view.locate("h", 1);
        auto vi21 = view.locate("h", 4);
        auto vi22 = view.locate("h", 6);
        auto vi30 = view.locate("m", 1);
        auto vi31 = view.locate("m", 4);
        auto vi32 = view.locate("m", 6);
        auto vi40 = view.locate("n", 1);
        auto vi41 = view.locate("n", 4);
        auto vi42 = view.locate("n", 6);

        auto v00 = var.locate("f", 1);
        auto v01 = var.locate("f", 4);
        auto v02 = var.locate("f", 6);
        auto v10 = var.locate("g", 1);
        auto v11 = var.locate("g", 4);
        auto v12 = var.locate("g", 6);
        auto v20 = var.locate("h", 1);
        auto v21 = var.locate("h", 4);
        auto v22 = var.locate("h", 6);
        auto v30 = var.locate("m", 1);
        auto v31 = var.locate("m", 4);
        auto v32 = var.locate("m", 6);
        auto v40 = var.locate("n", 1);
        auto v41 = var.locate("n", 4);
        auto v42 = var.locate("n", 6);

        EXPECT_EQ(v00, vi00);
        EXPECT_EQ(v01, vi01);
        EXPECT_EQ(v02, vi02);
        EXPECT_EQ(v10, vi10);
        EXPECT_EQ(v11, vi11);
        EXPECT_EQ(v12, vi12);
        EXPECT_EQ(v20, vi20);
        EXPECT_EQ(v21, vi21);
        EXPECT_EQ(v22, vi22);
        EXPECT_EQ(v30, vi30);
        EXPECT_EQ(v31, vi31);
        EXPECT_EQ(v32, vi32);
        EXPECT_EQ(v40, vi40);
        EXPECT_EQ(v41, vi41);
        EXPECT_EQ(v42, vi42);
    }

    TEST(xvariable_view, locate_element)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = build_view(var);

        auto vi00 = view.locate_element({ "f", 1 });
        auto vi01 = view.locate_element({ "f", 4 });
        auto vi02 = view.locate_element({ "f", 6 });
        auto vi10 = view.locate_element({ "g", 1 });
        auto vi11 = view.locate_element({ "g", 4 });
        auto vi12 = view.locate_element({ "g", 6 });
        auto vi20 = view.locate_element({ "h", 1 });
        auto vi21 = view.locate_element({ "h", 4 });
        auto vi22 = view.locate_element({ "h", 6 });
        auto vi30 = view.locate_element({ "m", 1 });
        auto vi31 = view.locate_element({ "m", 4 });
        auto vi32 = view.locate_element({ "m", 6 });
        auto vi40 = view.locate_element({ "n", 1 });
        auto vi41 = view.locate_element({ "n", 4 });
        auto vi42 = view.locate_element({ "n", 6 });

        auto v00 = var.locate("f", 1);
        auto v01 = var.locate("f", 4);
        auto v02 = var.locate("f", 6);
        auto v10 = var.locate("g", 1);
        auto v11 = var.locate("g", 4);
        auto v12 = var.locate("g", 6);
        auto v20 = var.locate("h", 1);
        auto v21 = var.locate("h", 4);
        auto v22 = var.locate("h", 6);
        auto v30 = var.locate("m", 1);
        auto v31 = var.locate("m", 4);
        auto v32 = var.locate("m", 6);
        auto v40 = var.locate("n", 1);
        auto v41 = var.locate("n", 4);
        auto v42 = var.locate("n", 6);

        EXPECT_EQ(v00, vi00);
        EXPECT_EQ(v01, vi01);
        EXPECT_EQ(v02, vi02);
        EXPECT_EQ(v10, vi10);
        EXPECT_EQ(v11, vi11);
        EXPECT_EQ(v12, vi12);
        EXPECT_EQ(v20, vi20);
        EXPECT_EQ(v21, vi21);
        EXPECT_EQ(v22, vi22);
        EXPECT_EQ(v30, vi30);
        EXPECT_EQ(v31, vi31);
        EXPECT_EQ(v32, vi32);
        EXPECT_EQ(v40, vi40);
        EXPECT_EQ(v41, vi41);
        EXPECT_EQ(v42, vi42);
    }

    TEST(xvariable_view, view_squeeze)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = select(var, { { "abscissa", "f"}, { "ordinate", range(1, 6, 2) } });
        EXPECT_EQ(view.size(), 3u);
        EXPECT_EQ(view.dimension(), 1u);
        saxis_type s = { "ordinate" };
        EXPECT_EQ(view.dimension_labels(), s.labels());

        auto v0 = var.select({ { "abscissa", "f" },{ "ordinate", 1 } });
        auto v1 = var.select({ { "abscissa", "f" },{ "ordinate", 4 } });
        auto v2 = var.select({ { "abscissa", "f" },{ "ordinate", 6 } });

        auto via0 = view(0);
        auto via1 = view(1);
        auto via2 = view(2);

        EXPECT_EQ(via0, v0);
        EXPECT_EQ(via1, v1);
        EXPECT_EQ(via2, v2);

        auto viae0 = view.element({ 0 });
        auto viae1 = view.element({ 1 });
        auto viae2 = view.element({ 2 });

        EXPECT_EQ(viae0, v0);
        EXPECT_EQ(viae1, v1);
        EXPECT_EQ(viae2, v2);

        auto vi0 = view.select({ { "ordinate", 1 } });
        auto vi1 = view.select({ { "ordinate", 4 } });
        auto vi2 = view.select({ { "ordinate", 6 } });

        EXPECT_EQ(vi0, v0);
        EXPECT_EQ(vi1, v1);
        EXPECT_EQ(vi2, v2);

        EXPECT_NO_THROW(var.select({ { "abscissa", "f" },{ "ordinate", 5 } }));
        EXPECT_ANY_THROW(view.select({ { "ordinate", 5 } }));

        auto vio0 = view.select<join::outer>({ { "ordinate", 1 } });
        auto vio1 = view.select<join::outer>({ { "ordinate", 4 } });
        auto vio2 = view.select<join::outer>({ { "ordinate", 6 } });

        EXPECT_EQ(vio0, v0);
        EXPECT_EQ(vio1, v1);
        EXPECT_EQ(vio2, v2);

        EXPECT_EQ(view.missing(), view.select<join::outer>({ { "ordinate", 2 } }));

        auto vil0 = view.locate(1);
        auto vil1 = view.locate(4);
        auto vil2 = view.locate(6);

        EXPECT_EQ(vil0, v0);
        EXPECT_EQ(vil1, v1);
        EXPECT_EQ(vil2, v2);

        auto vile0 = view.locate_element({ 1 });
        auto vile1 = view.locate_element({ 4 });
        auto vile2 = view.locate_element({ 6 });

        EXPECT_EQ(vile0, v0);
        EXPECT_EQ(vile1, v1);
        EXPECT_EQ(vile2, v2);

        auto vii0 = view.iselect({ { "ordinate", 0 } });
        auto vii1 = view.iselect({ { "ordinate", 1 } });
        auto vii2 = view.iselect({ { "ordinate", 2 } });

        EXPECT_EQ(vii0, v0);
        EXPECT_EQ(vii1, v1);
        EXPECT_EQ(vii2, v2);
    }

    TEST(xvariable_view, locate_builder)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = build_view(var);
        variable_view_type view2 = locate(var, range("f", "n"), range(1, 6, 2));
        EXPECT_EQ(view, view2);

        variable_view_type view3 = select(var, { { "abscissa", "f" },{ "ordinate", range(1, 6, 2) } });
        variable_view_type view4 = locate(var, "f", range(1, 6, 2));
        EXPECT_EQ(view3, view4);

        variable_view_type view5 = select(var, { {"abscissa", "f"} });
        variable_view_type view6 = locate(var, "f", all());
        EXPECT_EQ(view5, view6);
    }

    TEST(xvariable_view, iselect_builder)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = build_view(var);
        variable_view_type view2 = iselect(var, { {"abscissa", irange(3, 8)},
                                                  {"ordinate", irange(0, 5, 2)} });
        EXPECT_EQ(view, view2);

        variable_view_type view3 = select(var, { { "abscissa", "f" },{ "ordinate", range(1, 6, 2) } });
        variable_view_type view4 = iselect(var, { { "abscissa", 3 },{ "ordinate", irange(0, 5, 2) } });
        EXPECT_EQ(view3, view4);

        variable_view_type view5 = select(var, { { "abscissa", "f" } });
        variable_view_type view6 = iselect(var, { { "abscissa", 3 } });
        EXPECT_EQ(view5, view6);
    }

    TEST(xvariable_view, view_builder)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = build_view(var);
        variable_view_type view2 = ilocate(var, irange(3, 8), irange(0, 5, 2));
        EXPECT_EQ(view, view2);

        variable_view_type view3 = select(var, { { "abscissa", "f" },{ "ordinate", range(1, 6, 2) } });
        variable_view_type view4 = ilocate(var, 3, irange(0, 5, 2));
        EXPECT_EQ(view3, view4);

        variable_view_type view5 = select(var, { { "abscissa", "f" } });
        variable_view_type view6 = ilocate(var, 3, iall());
        EXPECT_EQ(view5, view6);
    }

    TEST(xvariable_view, keep_slice)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = locate(var, range("f", "n"), range(1, 6, 2));
        variable_view_type view2 = locate(var, xf::keep("f", "g", "h", "m", "n"), range(1, 6, 2));
        variable_view_type view3 = ilocate(var, ikeep(3, 4, 5, 6, 7), irange(0, 5, 2));
        variable_view_type view4 = select(var, { { "abscissa", xf::keep("f", "g", "h", "m", "n") },
                                                 { "ordinate", range(1, 6, 2) } });
        variable_view_type view5 = iselect(var, { { "abscissa", ikeep(3, 4, 5, 6, 7) },
                                                  { "ordinate", irange(0, 5, 2) } });
        
        EXPECT_EQ(view, view2);
        EXPECT_EQ(view, view3);
        EXPECT_EQ(view, view4);
        EXPECT_EQ(view, view5);
    }

    TEST(xvariable_view, drop_slice)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = locate(var, range("f", "n"), range(1, 6, 2));
        variable_view_type view2 = locate(var, xf::drop("a", "c", "d"), range(1, 6, 2));
        variable_view_type view3 = ilocate(var, idrop(0, 1, 2), irange(0, 5, 2));
        variable_view_type view4 = select(var, { { "abscissa", xf::drop("a", "c", "d") },
                                                 { "ordinate", range(1, 6, 2) } });
        variable_view_type view5 = iselect(var, { { "abscissa", idrop(0, 1, 2) },
                                                  { "ordinate", irange(0, 5, 2) } });

        EXPECT_EQ(view, view2);
        EXPECT_EQ(view, view3);
        EXPECT_EQ(view, view4);
        EXPECT_EQ(view, view5);
    }
}
