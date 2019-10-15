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

#include "xtensor/xarray.hpp"
#include "xtensor/xfunction.hpp"

#include "xframe/xnamed_axis.hpp"

#include "test_fixture.hpp"

namespace xf
{
    TEST(xaxis_function, plus_operator)
    {
        auto axis1 = named_axis("abs", axis(16));
        auto axis2 = named_axis("ord", axis(10, 20, 1));

        auto func1 = axis1 + axis2;
        EXPECT_EQ(func1({{"abs", 10}, {"ord", 5}}), 25);
        EXPECT_EQ(func1({{"abs", 15}, {"ord", 9}}), 34);
    }

    TEST(xaxis_function, pow_operator)
    {
        auto axis1 = named_axis("abs", axis(15));
        auto axis2 = named_axis("ord", axis(10, 20, 1));

        auto func1 = pow(axis2, axis1);
        EXPECT_EQ(func1({{"abs", 2}, {"ord", 5}}), 225);

        auto func2 = axis1 + axis2 + pow(axis2, axis1);
        EXPECT_EQ(func2({{"abs", 2}, {"ord", 5}}), 242);
    }

    TEST(xaxis_function, bool_operator)
    {
        auto axis1 = named_axis("abs", axis(15));
        auto axis2 = named_axis("ord", axis(10, 20, 1));
        auto axis3 = named_axis("alt", axis('b', 'j'));

        auto func1 = axis1 < 13;
        EXPECT_EQ(func1({{"abs", 10}}), true);
        EXPECT_EQ(func1({{"abs", 14}}), false);

        auto func2 = axis1 > axis2;
        EXPECT_EQ(func2({{"abs", 10}, {"ord", 3}}), false);
        EXPECT_EQ(func2({{"abs", 14}, {"ord", 3}}), true);

        auto func3 = equal(axis3, 'b') || axis3 >= 'g' && not_equal(axis3, 'i');
        EXPECT_EQ(func3({{"alt", 0}}), true);  // b
        EXPECT_EQ(func3({{"alt", 1}}), false); // c
        EXPECT_EQ(func3({{"alt", 2}}), false); // d
        EXPECT_EQ(func3({{"alt", 3}}), false); // e
        EXPECT_EQ(func3({{"alt", 4}}), false); // f
        EXPECT_EQ(func3({{"alt", 5}}), true);  // g
        EXPECT_EQ(func3({{"alt", 6}}), true);  // h
        EXPECT_EQ(func3({{"alt", 7}}), false); // i
    }

    TEST(xaxis_function, xaxis_scalar)
    {
        auto axis1 = named_axis("abs", axis(15));
        auto axis2 = named_axis("ord", axis(10, 20, 1));

        auto func1 = axis1 < 13;
        EXPECT_EQ(func1({{"abs", 10}}), true);
        EXPECT_EQ(func1({{"abs", 14}}), false);

        auto func2 = axis1 + axis2 + 4;
        EXPECT_EQ(func2({{"abs", 10}, {"ord", 5}}), 29);
    }

    TEST(xaxis_function, mask)
    {
        auto axis1 = named_axis(fstring("abs"), axis({0, 2, 5}));
        auto axis2 = named_axis(fstring("ord"), axis({'a', 'c', 'i'}));

        auto array = xt::xarray<bool>({
            {true, true, true},
            {true, true, true},
            {true, true, true}
        });

        auto mask = axis_function_mask(
            equal(axis2, 'c') || equal(axis1, 0),
            dimension_type({"abs", "ord"}),
            array.shape()
        );

        auto expected = xt::xarray<bool>({
            { true,  true,  true},
            {false,  true, false},
            {false,  true, false}
        });

        EXPECT_EQ(mask, expected);
    }

    TEST(xaxis_function, mask_op)
    {
        auto axis1 = named_axis(fstring("abs"), axis({0, 2, 5}));
        auto axis2 = named_axis(fstring("ord"), axis({'a', 'c', 'i'}));

        auto array = xt::xarray<bool>({
            { true,  true, false},
            { true,  true,  true},
            { true,  true,  true}
        });

        auto mask = axis_function_mask(
            equal(axis2, 'i') || equal(axis1, 0),
            dimension_type({"abs", "ord"}),
            array.shape()
        );

        auto expected = xt::xarray<bool>({
            { true,  true, false},
            {false, false,  true},
            {false, false,  true}
        });

        xt::xarray<bool> val = array && mask;
        EXPECT_EQ(val, expected);
    }
}
