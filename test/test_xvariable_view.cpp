/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
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

    TEST(xvariable_view, view_squeeze)
    {
        variable_type var = make_test_view_variable();
        variable_view_type view = select(var, { { "abscissa", "f"}, { "ordinate", range(1, 6, 2) } });
        EXPECT_EQ(view.size(), 3u);
        EXPECT_EQ(view.dimension(), 1u);
        saxis_type s = { "ordinate" };
        EXPECT_EQ(view.dimension_labels(), s.labels());

        auto vi0 = view.select({ { "ordinate", 1 } });
        auto vi1 = view.select({ { "ordinate", 4 } });
        auto vi2 = view.select({ { "ordinate", 6 } });

        auto v0 = var.select({ { "abscissa", "f" },{ "ordinate", 1 } });
        auto v1 = var.select({ { "abscissa", "f" },{ "ordinate", 4 } });
        auto v2 = var.select({ { "abscissa", "f" },{ "ordinate", 6 } });

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
    }
}
