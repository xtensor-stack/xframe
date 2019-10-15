/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <vector>

#include "gtest/gtest.h"

#include "test_fixture.hpp"

#include "xframe/xexpand_dims_view.hpp"

namespace xf
{
    TEST(xexpand_dims, ctor)
    {
        auto var = make_test_variable();

        auto res1 = expand_dims(var, {"new_dim"});
        auto res2 = expand_dims(var, {"new_dim", "new_dim2"});
        auto res3 = expand_dims(var, {{"new_dim", 0}, {"new_dim2", 2}});
    }

    TEST(xexpand_dims, size)
    {
        auto var = make_test_variable();

        auto res = expand_dims(var, {"new_dim", "new_dim2"});

        EXPECT_EQ(var.size(), res.size());
    }

    TEST(xexpand_dims, dimension)
    {
        auto var = make_test_variable();

        auto res = expand_dims(var, {"new_dim", "new_dim2"});

        EXPECT_EQ(res.dimension(), std::size_t(4));
    }

    TEST(xexpand_dims, dimension_labels)
    {
        auto var = make_test_variable();

        auto res = expand_dims(var, {"new_dim"});
        auto expected_dim_mapping = std::vector<fstring>({"new_dim", "abscissa", "ordinate"});
        EXPECT_EQ(res.dimension_labels(), expected_dim_mapping);

        auto res1 = expand_dims(var, {"new_dim", "new_dim2"});
        expected_dim_mapping = {"new_dim", "new_dim2", "abscissa", "ordinate"};
        EXPECT_EQ(res1.dimension_labels(), expected_dim_mapping);

        auto res2 = expand_dims(var, {{"new_dim", 1}, {"new_dim2", 2}});
        expected_dim_mapping = {"abscissa", "new_dim", "new_dim2", "ordinate"};
        EXPECT_EQ(res2.dimension_labels(), expected_dim_mapping);

        auto res3 = expand_dims(var, {{"new_dim", 2}});
        expected_dim_mapping = {"abscissa", "ordinate", "new_dim"};
        EXPECT_EQ(res3.dimension_labels(), expected_dim_mapping);
    }

    TEST(xexpand_dims, coordinates)
    {
        auto var = make_test_variable();

        auto res = expand_dims(var, {"new_dim"});
        const auto& coords = res.coordinates();
        EXPECT_NE(coords.find("new_dim"), coords.cend());
        EXPECT_NE(coords.find("abscissa"), coords.cend());
        EXPECT_NE(coords.find("ordinate"), coords.cend());
        EXPECT_EQ(coords.size(), std::size_t(3));
        EXPECT_EQ(coords.find("new_dim")->second.size(), std::size_t(1));
        EXPECT_EQ(coords.find("new_dim")->second[0], std::size_t(0));
    }

    TEST(xexpand_dims, dimension_mapping)
    {
        auto var = make_test_variable();

        auto res = expand_dims(var, {"new_dim"});
        auto expected_dim_mapping = xdimension<fstring>({"new_dim", "abscissa", "ordinate"});
        EXPECT_EQ(res.dimension_mapping(), expected_dim_mapping);

        auto res1 = expand_dims(var, {"new_dim", "new_dim2"});
        auto expected_dim_mapping1 = xdimension<fstring>({"new_dim", "new_dim2", "abscissa", "ordinate"});
        EXPECT_EQ(res1.dimension_mapping(), expected_dim_mapping1);

        auto res2 = expand_dims(var, {{"new_dim", 1}, {"new_dim2", 2}});
        auto expected_dim_mapping2 = xdimension<fstring>({"abscissa", "new_dim", "new_dim2", "ordinate"});
        EXPECT_EQ(res2.dimension_mapping(), expected_dim_mapping2);

        auto res3 = expand_dims(var, {{"new_dim", 2}});
        auto expected_dim_mapping3 = xdimension<fstring>({"abscissa", "ordinate", "new_dim"});
        EXPECT_EQ(res3.dimension_mapping(), expected_dim_mapping3);
    }

    TEST(xexpand_dims, shape)
    {
        auto var = make_test_variable();

        auto res = expand_dims(var, {"new_dim", "new_dim2"});
        std::vector<std::size_t> expected_shape = {1, 1, 3, 3};

        EXPECT_EQ(res.shape(), expected_shape);
    }

    TEST(xexpand_dims, data)
    {
        auto var = make_test_variable();

        auto res = expand_dims(var, {"new_dim"});
        const auto& expanded_data_view = res.data();
        std::vector<std::size_t> expected_shape = {1, 3, 3};

        EXPECT_EQ(expanded_data_view.dimension(), std::size_t(3));
        EXPECT_EQ(expanded_data_view.shape(), expected_shape);

        auto res1 = expand_dims(var, {"new_dim", "new_dim2"});
        const auto& expanded_data_view1 = res1.data();
        expected_shape = {1, 1, 3, 3};

        EXPECT_EQ(expanded_data_view1.dimension(), std::size_t(4));
        EXPECT_EQ(expanded_data_view1.shape(), expected_shape);

        auto res2 = expand_dims(var, {{"new_dim", 1}, {"new_dim2", 2}});
        const auto& expanded_data_view2 = res2.data();
        expected_shape = {3, 1, 1, 3};

        EXPECT_EQ(expanded_data_view2.dimension(), std::size_t(4));
        EXPECT_EQ(expanded_data_view2.shape(), expected_shape);

        auto res3 = expand_dims(var, {{"new_dim", 2}});
        const auto& expanded_data_view3 = res3.data();
        expected_shape = {3, 3, 1};

        EXPECT_EQ(expanded_data_view3.dimension(), std::size_t(3));
        EXPECT_EQ(expanded_data_view3.shape(), expected_shape);
    }

    TEST(xexpand_dims, access)
    {
        auto var = make_test_variable();

        auto res = expand_dims(var, {"new_dim"});
        EXPECT_EQ(res(0, 2, 0), 7.0);
        EXPECT_EQ(res(0, 2, 1), 8.0);
        EXPECT_EQ(res(0, 2, 2), 9.0);

        auto res1 = expand_dims(var, {"new_dim", "new_dim2"});
        EXPECT_EQ(res1(0, 0, 2, 0), 7.0);
        EXPECT_EQ(res1(0, 0, 2, 1), 8.0);
        EXPECT_EQ(res1(0, 0, 2, 2), 9.0);

        auto res2 = expand_dims(var, {{"new_dim", 1}, {"new_dim2", 2}});
        EXPECT_EQ(res2(2, 0, 0, 0), 7.0);
        EXPECT_EQ(res2(2, 0, 0, 1), 8.0);
        EXPECT_EQ(res2(2, 0, 0, 2), 9.0);

        auto res3 = expand_dims(var, {{"new_dim", 2}});
        EXPECT_EQ(res3(2, 0, 0), 7.0);
        EXPECT_EQ(res3(2, 1, 0), 8.0);
        EXPECT_EQ(res3(2, 2, 0), 9.0);
    }

    TEST(xexpand_dims, element)
    {
        auto var = make_test_variable();

        auto res = expand_dims(var, {"new_dim"});
        EXPECT_EQ(res.element({0, 2, 0}), 7.0);
        EXPECT_EQ(res.element({0, 2, 1}), 8.0);
        EXPECT_EQ(res.element({0, 2, 2}), 9.0);

        auto res1 = expand_dims(var, {"new_dim", "new_dim2"});
        EXPECT_EQ(res1.element({0, 0, 2, 0}), 7.0);
        EXPECT_EQ(res1.element({0, 0, 2, 1}), 8.0);
        EXPECT_EQ(res1.element({0, 0, 2, 2}), 9.0);

        auto res2 = expand_dims(var, {{"new_dim", 1}, {"new_dim2", 2}});
        EXPECT_EQ(res2.element({2, 0, 0, 0}), 7.0);
        EXPECT_EQ(res2.element({2, 0, 0, 1}), 8.0);
        EXPECT_EQ(res2.element({2, 0, 0, 2}), 9.0);

        auto res3 = expand_dims(var, {{"new_dim", 2}});
        EXPECT_EQ(res3.element({2, 0, 0}), 7.0);
        EXPECT_EQ(res3.element({2, 1, 0}), 8.0);
        EXPECT_EQ(res3.element({2, 2, 0}), 9.0);
    }

    TEST(xexpand_dims, locate)
    {
        auto var = make_test_variable();

        auto res = expand_dims(var, {"new_dim"});
        EXPECT_EQ(res.locate(0, "d", 1), 7.0);
        EXPECT_EQ(res.locate(0, "d", 2), 8.0);
        EXPECT_EQ(res.locate(0, "d", 4), 9.0);

        auto res1 = expand_dims(var, {"new_dim", "new_dim2"});
        EXPECT_EQ(res1.locate(0, 0, "d", 1), 7.0);
        EXPECT_EQ(res1.locate(0, 0, "d", 2), 8.0);
        EXPECT_EQ(res1.locate(0, 0, "d", 4), 9.0);

        auto res2 = expand_dims(var, {{"new_dim", 1}, {"new_dim2", 2}});
        EXPECT_EQ(res2.locate("d", 0, 0, 1), 7.0);
        EXPECT_EQ(res2.locate("d", 0, 0, 2), 8.0);
        EXPECT_EQ(res2.locate("d", 0, 0, 4), 9.0);

        auto res3 = expand_dims(var, {{"new_dim", 2}});
        EXPECT_EQ(res3.locate("d", 1, 0), 7.0);
        EXPECT_EQ(res3.locate("d", 2, 0), 8.0);
        EXPECT_EQ(res3.locate("d", 4, 0), 9.0);
    }

    TEST(xexpand_dims, iselect)
    {
        auto var = make_test_variable();

        auto res = expand_dims(var, {{"new_dim", 2}});
        EXPECT_EQ(res.iselect({{"abscissa", 2}, {"ordinate", 0}}), 7.0);
        EXPECT_EQ(res.iselect({{"new_dim", 0}, {"abscissa", 2}, {"ordinate", 1}}), 8.0);
        EXPECT_EQ(res.iselect({{"new_dim", 0}, {"abscissa", 2}, {"ordinate", 2}}), 9.0);

        auto res1 = expand_dims(var, {"new_dim", "new_dim2"});
        EXPECT_EQ(res1.iselect({{"abscissa", 2}, {"ordinate", 0}}), 7.0);
        EXPECT_EQ(res1.iselect({{"new_dim2", 0}, {"abscissa", 2}, {"ordinate", 1}}), 8.0);
        EXPECT_EQ(res1.iselect({{"new_dim", 0}, {"new_dim2", 0}, {"abscissa", 2}, {"ordinate", 2}}), 9.0);
    }

    TEST(xexpand_dims, select)
    {
        auto var = make_test_variable();
        auto missing = xtl::missing<double>();

        auto res = expand_dims(var, {{"new_dim", 2}});
        EXPECT_EQ(res.select({{"abscissa", "d"}, {"ordinate", 1}}), 7.0);
        EXPECT_EQ(res.select({{"new_dim", 0}, {"abscissa", "d"}, {"ordinate", 2}}), 8.0);
        EXPECT_EQ(res.select({{"new_dim", 0}, {"abscissa", "d"}, {"ordinate", 4}}), 9.0);

        auto res1 = expand_dims(var, {"new_dim", "new_dim2"});
        EXPECT_EQ(res1.select({{"abscissa", "d"}, {"ordinate", 1}}), 7.0);
        EXPECT_EQ(res1.select({{"new_dim2", 0}, {"abscissa", "d"}, {"ordinate", 2}}), 8.0);
        EXPECT_EQ(res1.select({{"new_dim", 0}, {"new_dim2", 0}, {"abscissa", "d"}, {"ordinate", 4}}), 9.0);
        EXPECT_ANY_THROW(res1.select({{"new_dim", 2}, {"new_dim2", 0}, {"abscissa", "d"}, {"ordinate", 4}}));
        EXPECT_EQ(res1.select<join::outer>({{"new_dim", 2}, {"new_dim2", 0}, {"abscissa", "d"}, {"ordinate", 4}}), missing);
    }
}
