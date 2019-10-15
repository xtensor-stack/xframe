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

#include "xframe/xaxis.hpp"
#include "xframe/xaxis_default.hpp"

namespace xf
{
    using axis_type = xaxis<int>;
    using label_type = std::vector<int>;
    using axis_default_type = xaxis_default<int>;

    TEST(xaxis_default, axis)
    {
        auto a = axis(4);
        auto labels = a.labels();

        EXPECT_EQ(0, labels[0]);
        EXPECT_EQ(1, labels[1]);
        EXPECT_EQ(2, labels[2]);
        EXPECT_EQ(3, labels[3]);
        EXPECT_EQ(4u, labels.size());
    }

    TEST(xaxis_default, labels)
    {
        axis_default_type a(4);
        label_type labels = a.labels();

        EXPECT_EQ(0, labels[0]);
        EXPECT_EQ(1, labels[1]);
        EXPECT_EQ(2, labels[2]);
        EXPECT_EQ(3, labels[3]);
        EXPECT_EQ(4u, labels.size());
    }

    TEST(xaxis_default, size)
    {
        axis_default_type a(36);
        EXPECT_EQ(36u, a.size());
        EXPECT_EQ(a.labels().size(), a.size());
        EXPECT_FALSE(a.empty());

        axis_default_type a2;
        EXPECT_EQ(0u, a2.size());
        EXPECT_TRUE(a2.empty());
    }

    TEST(xaxis_default, is_sorted)
    {
        axis_default_type a(36);
        EXPECT_TRUE(a.is_sorted());
    }

    TEST(xaxis_default, contains)
    {
        axis_default_type a(36);
        EXPECT_TRUE(a.contains(0));
        EXPECT_TRUE(a.contains(15));
        EXPECT_TRUE(a.contains(35));
        EXPECT_FALSE(a.contains(36));
        EXPECT_FALSE(a.contains(126));
        EXPECT_FALSE(a.contains(-2));
    }

    TEST(xaxis_default, access)
    {
        axis_default_type a(3);
        auto a0 = a[0];
        auto a1 = a[1];
        auto a2 = a[2];
        EXPECT_EQ(0u, a0);
        EXPECT_EQ(1u, a1);
        EXPECT_EQ(2u, a2);
        EXPECT_THROW(a[3], std::out_of_range);
    }

    TEST(xaxis_default, iterator)
    {
        axis_default_type a(3);

        auto it = a.begin();

        EXPECT_TRUE(it == a.cbegin());
        EXPECT_TRUE(it != a.end());
        EXPECT_TRUE(it < a.end());
        EXPECT_TRUE(it <= a.end());
        EXPECT_FALSE(it > a.end());
        EXPECT_FALSE(it >= a.end());
        EXPECT_EQ(a.end(), a.cend());

        EXPECT_EQ(it->first, 0);
        EXPECT_EQ(it->second, a[0]);
        ++it;
        EXPECT_EQ(it->first, 1);
        EXPECT_EQ(it->second, a[1]);
        auto tmp = it++;
        EXPECT_EQ(tmp->first, 1);
        EXPECT_EQ(it->first, 2);
        EXPECT_EQ(tmp->second, a[1]);
        EXPECT_EQ(it->second, a[2]);
        ++it;
        EXPECT_EQ(it, a.end());

        EXPECT_EQ(a[2], (a.begin() + 2)->second);
        EXPECT_EQ(a[1], (a.end() - 2)->second);
        EXPECT_EQ(3, a.end() - a.begin());
    }

    TEST(xaxis_default, find)
    {
        axis_default_type a(3);

        auto it0 = a.find(0);
        auto it1 = a.find(1);
        auto it2 = a.find(2);
        auto it3 = a.find(3);

        EXPECT_EQ(it0->first, 0);
        EXPECT_EQ(it1->first, 1);
        EXPECT_EQ(it2->first, 2);
        EXPECT_EQ(it3, a.end());
    }

    TEST(xaxis_default, merge)
    {
        axis_type a1 = { 3, 4, 5, 6 };
        axis_default_type a2(3);
        axis_default_type a3(5);
        axis_type res;
        bool t1 = merge_axes(res, a1, a2, a3);
        EXPECT_FALSE(t1);
        EXPECT_EQ(res[0], 0u);
        EXPECT_EQ(res[1], 1u);
        EXPECT_EQ(res[2], 2u);
        EXPECT_EQ(res[3], 3u);
        EXPECT_EQ(res[4], 4u);
        EXPECT_EQ(res[5], 5u);
        EXPECT_EQ(res[6], 6u);

        // Merging and xaxis_default with an xaxis
        axis_type a4;
        axis_type res2;
        bool t2 = merge_axes(res2, a2, a4);
        EXPECT_FALSE(t2);
        EXPECT_EQ(res2, a2);

        // Merging two xaxis_default together returns an xaxis
        axis_default_type a5(4);
        axis_default_type a6(4);
        axis_type res3;
        bool t3 = merge_axes(res3, a5, a6);
        EXPECT_TRUE(t3);
    }

    TEST(xaxis_default, filter)
    {
        axis_default_type a(36);
        axis_type res = a.filter([](const auto& arg) { return arg >= 16; });
        EXPECT_TRUE(res.is_sorted());
        EXPECT_EQ(res.size(), 20u);
        EXPECT_EQ(res.labels()[0], 16);
        EXPECT_EQ(res[16], 0u);
        EXPECT_EQ(res[35], 19u);
    }

    TEST(xaxis_default, filter_size)
    {
        axis_default_type a(36);
        axis_type res = a.filter([](const auto& arg) { return arg >= 26; }, std::size_t(10));
        EXPECT_TRUE(res.is_sorted());
        EXPECT_EQ(res.size(), 10u);
        EXPECT_EQ(res.labels()[0], 26);
        EXPECT_EQ(res[26], 0u);
        EXPECT_EQ(res[35], 9u);
    }
}
