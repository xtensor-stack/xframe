/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <cstddef>
#include <vector>
#include "gtest/gtest.h"

#include "xframe/xframe_config.hpp"
#include "xframe/xaxis_variant.hpp"

namespace xf
{
    using axis_variant_type = xaxis_variant<XFRAME_DEFAULT_LABEL_LIST, std::size_t>;

    TEST(xaxis_variant, get_labels)
    {
        auto a = axis_variant_type(axis(56));
        auto labels = get_labels<int>(a);

        EXPECT_EQ(0, labels[0]);
        EXPECT_EQ(56u, labels.size());
    }

    TEST(xaxis_variant, size)
    {
        auto a = axis_variant_type(axis(56));
        EXPECT_EQ(56u, a.size());
        EXPECT_FALSE(a.empty());

        auto a2 = axis_variant_type(axis(0));
        EXPECT_EQ(0u, a2.size());
        EXPECT_TRUE(a2.empty());
    }

    TEST(xaxis_variant, is_sorted)
    {
        auto a = axis_variant_type(axis(36, 56, 2));
        EXPECT_TRUE(a.is_sorted());
    }

    TEST(xaxis_variant, contains)
    {
        auto a = axis_variant_type(axis(36));
        EXPECT_TRUE(a.contains(0));
        EXPECT_TRUE(a.contains(15));
        EXPECT_TRUE(a.contains(35));
        EXPECT_FALSE(a.contains(36));
        EXPECT_FALSE(a.contains(126));
        EXPECT_FALSE(a.contains(-2));
    }

    TEST(xaxis_variant, access)
    {
        auto a = axis_variant_type(axis(3));
        auto a0 = a[0];
        auto a1 = a[1];
        auto a2 = a[2];
        EXPECT_EQ(0u, a0);
        EXPECT_EQ(1u, a1);
        EXPECT_EQ(2u, a2);
        EXPECT_THROW(a[3], std::out_of_range);
    }
}
