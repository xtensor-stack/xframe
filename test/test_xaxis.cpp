/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <cstddef>
#include <vector>
#include "gtest/gtest.h"
#include "xframe/xaxis.hpp"
#include "xtl/xbasic_fixed_string.hpp"

namespace xf
{
    using fstring = xtl::xfixed_string<55>;
    using label_type = std::vector<fstring>;
    using axis_type = xaxis<fstring, std::size_t>;

    TEST(xaxis, constructors)
    {
        label_type l = { "a", "b", "c" };
        // From const label_type&
        {
            axis_type a(l);
            EXPECT_EQ(l, a.labels());
        }

        // From label_type&&
        {
            label_type tmp(l);
            axis_type a(std::move(tmp));
            EXPECT_EQ(l, a.labels());
        }

        // From initializer_list
        {
            axis_type a = { "a", "b", "c" };
            EXPECT_EQ(l, a.labels());
        }

        // From iterators
        {
            axis_type a(l.cbegin(), l.cend());
            EXPECT_EQ(l, a.labels());
        }
    }

    TEST(xaxis, size)
    {
        axis_type a = { "a", "b", "c" };
        EXPECT_EQ(3u, a.size());
        EXPECT_FALSE(a.empty());

        axis_type a2;
        EXPECT_EQ(0u, a2.size());
        EXPECT_TRUE(a2.empty());
    }

    TEST(xaxis, contains)
    {
        axis_type a = { "a", "b", "c" };
        EXPECT_TRUE(a.contains("a"));
        EXPECT_FALSE(a.contains("d"));
    }

    TEST(xaxis, access)
    {
        axis_type a = { "a", "b", "c" };
        auto aa = a["a"];
        auto ab = a["b"];
        auto ac = a["c"];
        EXPECT_EQ(0u, aa);
        EXPECT_EQ(1u, ab);
        EXPECT_EQ(2u, ac);
        EXPECT_THROW(a["d"], std::out_of_range);
    }

    TEST(xaxis, iterator)
    {
        axis_type a = { "a", "b", "c" };

        auto it = a.begin();

        EXPECT_TRUE(it == a.cbegin());
        EXPECT_TRUE(it != a.end());
        EXPECT_TRUE(it < a.end());
        EXPECT_TRUE(it <= a.end());
        EXPECT_FALSE(it > a.end());
        EXPECT_FALSE(it >= a.end());
        EXPECT_EQ(a.end(), a.cend());

        EXPECT_EQ(it->first, "a");
        EXPECT_EQ(it->second, a["a"]);
        ++it;
        EXPECT_EQ(it->first, "b");
        EXPECT_EQ(it->second, a["b"]);
        auto tmp = it++;
        EXPECT_EQ(tmp->first, "b");
        EXPECT_EQ(it->first, "c");
        EXPECT_EQ(tmp->second, a["b"]);
        EXPECT_EQ(it->second, a["c"]);
        ++it;
        EXPECT_EQ(it, a.end());

        EXPECT_EQ(a["c"], (a.begin() + 2)->second);
        EXPECT_EQ(a["b"], (a.end() - 2)->second);
        EXPECT_EQ(3, a.end() - a.begin());
    }

    TEST(xaxis, find)
    {
        axis_type a = { "a", "b", "c" };

        auto ita = a.find("a");
        auto itb = a.find("b");
        auto itc = a.find("c");
        auto itd = a.find("d");

        EXPECT_EQ(ita->first, "a");
        EXPECT_EQ(itb->first, "b");
        EXPECT_EQ(itc->first, "c");
        EXPECT_EQ(itd, a.end());
    }

    TEST(xaxis, merge)
    {
        axis_type a1 = { "a", "b", "d", "e" };
        axis_type a2 = { "b", "c", "d" };
        axis_type a3 = { "c", "g" };
        axis_type res; 
        bool t1 = merge_axes(res, a1, a2, a3);
        EXPECT_FALSE(t1);
        EXPECT_EQ(res["a"], 0);
        EXPECT_EQ(res["b"], 1);
        EXPECT_EQ(res["c"], 2);
        EXPECT_EQ(res["d"], 3);
        EXPECT_EQ(res["e"], 4);
        EXPECT_EQ(res["g"], 5);

        axis_type a4;
        axis_type res2;
        bool t2 = merge_axes(res2, a1, a4);
        EXPECT_FALSE(t2);
        EXPECT_EQ(res2, a1);

        xaxis<int, std::size_t> ia1 = { 1, 2, 4};
        xaxis<int, std::size_t> ia2 = { 1, 4, 5};
        xaxis<int, std::size_t> iares;
        bool t3 = merge_axes(iares, ia1, ia2);
        EXPECT_FALSE(t3);
        EXPECT_EQ(iares[1], 0);
        EXPECT_EQ(iares[2], 1);
        EXPECT_EQ(iares[4], 2);
        EXPECT_EQ(iares[5], 3);
    }

    TEST(xaxis, merge_unsorted)
    {
        axis_type a1({ "a", "b", "d", "e" }, false);
        axis_type a2({ "d", "e" }, false);
        axis_type a3({ "h", "c", "a", "b", "d", "e" }, false);
        axis_type a4({ "h", "b", "c", "e" }, false);

        axis_type res1;
        bool t1 = merge_axes(res1, a1, a2);
        EXPECT_FALSE(t1);
        EXPECT_EQ(res1.size(), 4u);
        EXPECT_EQ(res1["a"], 0);
        EXPECT_EQ(res1["b"], 1);
        EXPECT_EQ(res1["d"], 2);
        EXPECT_EQ(res1["e"], 3);

        axis_type res2;
        bool t2 = merge_axes(res2, a1, a3);
        EXPECT_FALSE(t2);
        EXPECT_EQ(res2.size(), 6u);
        EXPECT_EQ(res2["h"], 0);
        EXPECT_EQ(res2["c"], 1);
        EXPECT_EQ(res2["a"], 2);
        EXPECT_EQ(res2["b"], 3);
        EXPECT_EQ(res2["d"], 4);
        EXPECT_EQ(res2["e"], 5);

        axis_type res3;
        bool t3 = merge_axes(res3, a1, a4);
        EXPECT_FALSE(t3);
        EXPECT_EQ(res3.size(), 6u);
        EXPECT_EQ(res3["h"], 0);
        EXPECT_EQ(res3["c"], 1);
        EXPECT_EQ(res3["a"], 2);
        EXPECT_EQ(res3["b"], 3);
        EXPECT_EQ(res3["d"], 4);
        EXPECT_EQ(res3["e"], 5);
    }

    TEST(xaxis, intersect)
    {
        axis_type a1 = { "a", "b", "d", "e" };
        axis_type a2 = { "b", "c", "d" };
        axis_type a3 = { "a", "b", "d", "f" };
        axis_type res = { "b", "d" };
        axis_type tmp = a1;
        bool t1 = intersect_axes(tmp, a2, a3);
        EXPECT_FALSE(t1);
        EXPECT_EQ(tmp, res);

        tmp = a1;
        bool t2 = intersect_axes(tmp, a1);
        EXPECT_TRUE(t2);
        EXPECT_EQ(tmp, a1);
    }
}

