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

        auto view2 = reindex(var, {{ "abscissa", xf::axis({"a", "b", "c", "d"})}});
        EXPECT_TRUE(view2.coordinates() == coordinate_res);
        EXPECT_EQ(view2.dimension_mapping(), var.dimension_mapping());

        auto var3 = make_test_variable3();
        auto res1 = align<join::inner>(var, var3);
        auto coord = coordinate<fstring>({
                {fstring("abscissa"), saxis_type({"a", "d"})},
                {fstring("ordinate"), iaxis_type({1, 4})}
        });
        EXPECT_EQ(std::get<0>(res1).coordinates(), coord);
        EXPECT_EQ(std::get<1>(res1).coordinates(), coord);

        auto res2 = align<join::outer>(var, var3);
        auto coord2 = coordinate<fstring>({
                {fstring("abscissa"), saxis_type({"a", "c", "d", "e"})},
                {fstring("ordinate"), iaxis_type({1, 2, 4, 5})}
        });
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

    TEST(xreindex_view, access)
    {
        auto var = make_test_variable();
        coordinate_map new_coord = make_new_coordinate();
        auto view = reindex(var, new_coord);

        EXPECT_EQ(view(0, 0), 1.);
        EXPECT_EQ(view(0, 1), 2);
        EXPECT_EQ(view(0, 2), view.missing());
        EXPECT_EQ(view(1, 0), view.missing());
        EXPECT_EQ(view(1, 1), view.missing());
        EXPECT_EQ(view(1, 2), view.missing());
        EXPECT_EQ(view(2, 0), view.missing());
        EXPECT_EQ(view(2, 1), 5);
        EXPECT_EQ(view(2, 2), 6);
        EXPECT_EQ(view(3, 0), 7);
        EXPECT_EQ(view(3, 1), 8);
        EXPECT_EQ(view(3, 2), 9);
    }

    TEST(xreindex_view, element)
    {
        auto var = make_test_variable();
        coordinate_map new_coord = make_new_coordinate();
        auto view = reindex(var, new_coord);

        EXPECT_EQ(view.element({0, 0}), 1.);
        EXPECT_EQ(view.element({0, 1}), 2);
        EXPECT_EQ(view.element({0, 2}), view.missing());
        EXPECT_EQ(view.element({1, 0}), view.missing());
        EXPECT_EQ(view.element({1, 1}), view.missing());
        EXPECT_EQ(view.element({1, 2}), view.missing());
        EXPECT_EQ(view.element({2, 0}), view.missing());
        EXPECT_EQ(view.element({2, 1}), 5);
        EXPECT_EQ(view.element({2, 2}), 6);
        EXPECT_EQ(view.element({3, 0}), 7);
        EXPECT_EQ(view.element({3, 1}), 8);
        EXPECT_EQ(view.element({3, 2}), 9);
    }

    TEST(xreindex_view, locate)
    {
        auto var = make_test_variable();
        coordinate_map new_coord = make_new_coordinate();
        auto view = reindex(var, new_coord);

        auto t00 = view.locate("a", 1);
        auto t01 = view.locate("a", 2);
        auto t02 = view.locate("a", 4);
        auto t10 = view.locate("b", 1);
        auto t11 = view.locate("b", 2);
        auto t12 = view.locate("b", 4);
        auto t20 = view.locate("c", 1);
        auto t21 = view.locate("c", 2);
        auto t22 = view.locate("c", 4);
        auto t30 = view.locate("d", 1);
        auto t31 = view.locate("d", 2);
        auto t32 = view.locate("d", 4);

        EXPECT_EQ(t00, view(0, 0));
        EXPECT_EQ(t01, view(0, 1));
        EXPECT_EQ(t02, view(0, 2));
        EXPECT_EQ(t10, view(1, 0));
        EXPECT_EQ(t11, view(1, 1));
        EXPECT_EQ(t12, view(1, 2));
        EXPECT_EQ(t20, view(2, 0));
        EXPECT_EQ(t21, view(2, 1));
        EXPECT_EQ(t22, view(2, 2));
        EXPECT_EQ(t30, view(3, 0));
        EXPECT_EQ(t31, view(3, 1));
        EXPECT_EQ(t32, view(3, 2));
    }

    TEST(xreindex_view, locate_element)
    {
        auto var = make_test_variable();
        coordinate_map new_coord = make_new_coordinate();
        auto view = reindex(var, new_coord);

        auto t00 = view.locate_element({ "a", 1 });
        auto t01 = view.locate_element({ "a", 2 });
        auto t02 = view.locate_element({ "a", 4 });
        auto t10 = view.locate_element({ "b", 1 });
        auto t11 = view.locate_element({ "b", 2 });
        auto t12 = view.locate_element({ "b", 4 });
        auto t20 = view.locate_element({ "c", 1 });
        auto t21 = view.locate_element({ "c", 2 });
        auto t22 = view.locate_element({ "c", 4 });
        auto t30 = view.locate_element({ "d", 1 });
        auto t31 = view.locate_element({ "d", 2 });
        auto t32 = view.locate_element({ "d", 4 });

        EXPECT_EQ(t00, view(0, 0));
        EXPECT_EQ(t01, view(0, 1));
        EXPECT_EQ(t02, view(0, 2));
        EXPECT_EQ(t10, view(1, 0));
        EXPECT_EQ(t11, view(1, 1));
        EXPECT_EQ(t12, view(1, 2));
        EXPECT_EQ(t20, view(2, 0));
        EXPECT_EQ(t21, view(2, 1));
        EXPECT_EQ(t22, view(2, 2));
        EXPECT_EQ(t30, view(3, 0));
        EXPECT_EQ(t31, view(3, 1));
        EXPECT_EQ(t32, view(3, 2));
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

        EXPECT_EQ(t00, view(0, 0));
        EXPECT_EQ(t01, view(0, 1));
        EXPECT_EQ(t02, view(0, 2));
        EXPECT_EQ(t10, view(1, 0));
        EXPECT_EQ(t11, view(1, 1));
        EXPECT_EQ(t12, view(1, 2));
        EXPECT_EQ(t20, view(2, 0));
        EXPECT_EQ(t21, view(2, 1));
        EXPECT_EQ(t22, view(2, 2));
        EXPECT_EQ(t30, view(3, 0));
        EXPECT_EQ(t31, view(3, 1));
        EXPECT_EQ(t32, view(3, 2));

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

        EXPECT_EQ(t00, view(0, 0));
        EXPECT_EQ(t01, view(0, 1));
        EXPECT_EQ(t02, view(0, 2));
        EXPECT_EQ(t10, view(1, 0));
        EXPECT_EQ(t11, view(1, 1));
        EXPECT_EQ(t12, view(1, 2));
        EXPECT_EQ(t20, view(2, 0));
        EXPECT_EQ(t21, view(2, 1));
        EXPECT_EQ(t22, view(2, 2));
        EXPECT_EQ(t30, view(3, 0));
        EXPECT_EQ(t31, view(3, 1));
        EXPECT_EQ(t32, view(3, 2));

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

        EXPECT_EQ(t00, view(0, 0));
        EXPECT_EQ(t01, view(0, 1));
        EXPECT_EQ(t02, view(0, 2));
        EXPECT_EQ(t10, view(1, 0));
        EXPECT_EQ(t11, view(1, 1));
        EXPECT_EQ(t12, view(1, 2));
        EXPECT_EQ(t20, view(2, 0));
        EXPECT_EQ(t21, view(2, 1));
        EXPECT_EQ(t22, view(2, 2));
        EXPECT_EQ(t30, view(3, 0));
        EXPECT_EQ(t31, view(3, 1));
        EXPECT_EQ(t32, view(3, 2));
    }

    TEST(xreindex_view, data)
    {
        auto missing = xtl::missing<double>();
        using data_type = xt::xoptional_assembly<xt::xarray<double>, xt::xarray<bool>>;
        auto var = make_test_variable();
        coordinate_map new_coord = make_new_coordinate();
        auto view = reindex(var, new_coord);

        const auto& d = view.data();
        EXPECT_EQ(d.shape(), view.shape());
        EXPECT_EQ(d(0, 0), view(0, 0));
        std::array<size_t, 2> idx = {0u, 1u};
        EXPECT_EQ(d[idx], view(0, 1));

        auto var3 = make_test_variable3();
        auto res = align<join::inner>(var, var3);
        data_type exp0 = {{1., missing}, {7., 9.}};
        data_type exp1 = {{1., 2.}, {missing, 5.}};
        EXPECT_EQ(std::get<0>(res).data(), exp0);
        EXPECT_EQ(std::get<1>(res).data(), exp1);
    }
}
