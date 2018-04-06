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

        EXPECT_NO_THROW(var.select({{ "abscissa", "d" }, { "ordinate", 1 }}));
        // TODO: fix
        //EXPECT_ANY_THROW(view.select({{ "abscissa", "d" }, { "ordinate", 1 }}));
    }
}
