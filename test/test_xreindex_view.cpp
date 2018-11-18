/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"
#include "test_fixture.hpp"
#include "xframe/xreindex_view.hpp"

namespace xf
{
    using coordinate_map = typename coordinate_type::map_type;

    coordinate_map make_new_coordinate()
    {
        coordinate_map m;
        m["abscissa"] = make_test_saxis3();
        return m;
    }

    TEST(xreindex_view, builders)
    {
        auto var = make_test_variable();
        coordinate_map new_coord = make_new_coordinate();
        coordinate_type coordinate_res = {{"abscissa", make_test_saxis3()}, {"ordinate", make_test_iaxis()}};

        auto view = reindex(var, new_coord);
        EXPECT_TRUE(view.coordinates() == coordinate_res);
        EXPECT_EQ(view.dimension_mapping(), var.dimension_mapping());
    }

    TEST(xreindex_view, size)
    {
        auto var = make_test_variable();
        coordinate_map new_coord = make_new_coordinate();
        auto view = reindex(var, new_coord);
        EXPECT_EQ(12u, view.size());
    }

    TEST(xreindex_view, dimension)
    {
        auto var = make_test_variable();
        coordinate_map new_coord = make_new_coordinate();
        auto view = reindex(var, new_coord);
        EXPECT_EQ(2u, view.dimension());
    }

    TEST(xreindex_view, dimension_labels)
    {
        auto var = make_test_variable();
        coordinate_map new_coord = make_new_coordinate();
        auto view = reindex(var, new_coord);
        saxis_type s = { "abscissa", "ordinate" };
        EXPECT_EQ(view.dimension_labels(), s.labels());
    }

    TEST(xreindex_view, shape)
    {
        auto var = make_test_variable();
        coordinate_map new_coord = make_new_coordinate();
        auto view = reindex(var, new_coord);
        auto shape = view.shape();
        std::decay_t<decltype(view.shape())> expected = { 4, 3 };
        EXPECT_EQ(shape, expected);
    }

    TEST(xreindex_view, select_inner)
    {
        auto var = make_test_variable();
        coordinate_map new_coord = make_new_coordinate();
        auto view = reindex(var, new_coord);

        auto t00 = view.select({{"abscissa", "a"},{"ordinate", 1}});
        auto t01 = view.select({{"abscissa", "a"},{"ordinate", 2}});
        auto t02 = view.select({{"abscissa", "a"},{"ordinate", 4}});
        auto t10 = view.select({{"abscissa", "b"},{"ordinate", 1}});
        auto t11 = view.select({{"abscissa", "b"},{"ordinate", 2}});
        auto t12 = view.select({{"abscissa", "b"},{"ordinate", 4}});
        auto t20 = view.select({{"abscissa", "c"},{"ordinate", 1}});
        auto t21 = view.select({{"abscissa", "c"},{"ordinate", 2}});
        auto t22 = view.select({{"abscissa", "c"},{"ordinate", 4}});
        auto t30 = view.select({{"abscissa", "d"},{"ordinate", 1}});
        auto t31 = view.select({{"abscissa", "d"},{"ordinate", 2}});
        auto t32 = view.select({{"abscissa", "d"},{"ordinate", 4}});

        EXPECT_EQ(t00, 1.);
        EXPECT_EQ(t01, 2);
        EXPECT_EQ(t02, view.missing());
        EXPECT_EQ(t10, view.missing());
        EXPECT_EQ(t11, view.missing());
        EXPECT_EQ(t12, view.missing());
        EXPECT_EQ(t20, view.missing());
        EXPECT_EQ(t21, 5);
        EXPECT_EQ(t22, 6);
        EXPECT_EQ(t30, 7);
        EXPECT_EQ(t31, 8);
        EXPECT_EQ(t32, 9);

        auto t100 = view.select({{"abscissa", "a"}, {"ordinate", 1}, {"altitude", 1}});
        EXPECT_EQ(t100, t00);

        EXPECT_ANY_THROW(view.select({{"abscissa", "e"}, {"ordinate", 1}}));
    }

    TEST(xreindex_view, select_outer)
    {
        auto var = make_test_variable();
        coordinate_map new_coord = make_new_coordinate();
        auto view = reindex(var, new_coord);

        auto t00 = view.select<join::outer>({{"abscissa", "a"},{"ordinate", 1}});
        auto t01 = view.select<join::outer>({{"abscissa", "a"},{"ordinate", 2}});
        auto t02 = view.select<join::outer>({{"abscissa", "a"},{"ordinate", 4}});
        auto t10 = view.select<join::outer>({{"abscissa", "b"},{"ordinate", 1}});
        auto t11 = view.select<join::outer>({{"abscissa", "b"},{"ordinate", 2}});
        auto t12 = view.select<join::outer>({{"abscissa", "b"},{"ordinate", 4}});
        auto t20 = view.select<join::outer>({{"abscissa", "c"},{"ordinate", 1}});
        auto t21 = view.select<join::outer>({{"abscissa", "c"},{"ordinate", 2}});
        auto t22 = view.select<join::outer>({{"abscissa", "c"},{"ordinate", 4}});
        auto t30 = view.select<join::outer>({{"abscissa", "d"},{"ordinate", 1}});
        auto t31 = view.select<join::outer>({{"abscissa", "d"},{"ordinate", 2}});
        auto t32 = view.select<join::outer>({{"abscissa", "d"},{"ordinate", 4}});

        EXPECT_EQ(t00, 1.);
        EXPECT_EQ(t01, 2);
        EXPECT_EQ(t02, view.missing());
        EXPECT_EQ(t10, view.missing());
        EXPECT_EQ(t11, view.missing());
        EXPECT_EQ(t12, view.missing());
        EXPECT_EQ(t20, view.missing());
        EXPECT_EQ(t21, 5);
        EXPECT_EQ(t22, 6);
        EXPECT_EQ(t30, 7);
        EXPECT_EQ(t31, 8);
        EXPECT_EQ(t32, 9);

        auto t100 = view.select<join::outer>({{"abscissa", "a"}, {"ordinate", 1}, {"altitude", 1}});
        EXPECT_EQ(t100, t00);

        auto mis = view.select<join::outer>({{"abscissa", "e"}, {"ordinate", 1}});
        EXPECT_EQ(mis, view.missing());
    }

    TEST(xreindex_view, iselect)
    {
        auto var = make_test_variable();
        coordinate_map new_coord = make_new_coordinate();
        auto view = reindex(var, new_coord);

        auto t00 = view.iselect({{"abscissa", 0},{"ordinate", 0}});
        auto t01 = view.iselect({{"abscissa", 0},{"ordinate", 1}});
        auto t02 = view.iselect({{"abscissa", 0},{"ordinate", 2}});
        auto t10 = view.iselect({{"abscissa", 1},{"ordinate", 0}});
        auto t11 = view.iselect({{"abscissa", 1},{"ordinate", 1}});
        auto t12 = view.iselect({{"abscissa", 1},{"ordinate", 2}});
        auto t20 = view.iselect({{"abscissa", 2},{"ordinate", 0}});
        auto t21 = view.iselect({{"abscissa", 2},{"ordinate", 1}});
        auto t22 = view.iselect({{"abscissa", 2},{"ordinate", 2}});
        auto t30 = view.iselect({{"abscissa", 3},{"ordinate", 0}});
        auto t31 = view.iselect({{"abscissa", 3},{"ordinate", 1}});
        auto t32 = view.iselect({{"abscissa", 3},{"ordinate", 2}});

        EXPECT_EQ(t00, 1.);
        EXPECT_EQ(t01, 2);
        EXPECT_EQ(t02, view.missing());
        EXPECT_EQ(t10, view.missing());
        EXPECT_EQ(t11, view.missing());
        EXPECT_EQ(t12, view.missing());
        EXPECT_EQ(t20, view.missing());
        EXPECT_EQ(t21, 5);
        EXPECT_EQ(t22, 6);
        EXPECT_EQ(t30, 7);
        EXPECT_EQ(t31, 8);
        EXPECT_EQ(t32, 9);
    }
}
