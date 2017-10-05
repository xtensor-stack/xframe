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
    using axis_type = xaxis<label_type, std::size_t>;

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
        label_type l = { "a", "b", "c" };
        axis_type a(l);
        EXPECT_EQ(3u, a.size());
        EXPECT_FALSE(a.empty());

        label_type l2;
        axis_type a2(l2);
        EXPECT_EQ(0u, a2.size());
        EXPECT_TRUE(a2.empty());
    }

    TEST(xaxis, contains)
    {
        label_type l = { "a", "b", "c" };
        axis_type a(l);
        EXPECT_TRUE(a.contains("a"));
        EXPECT_FALSE(a.contains("d"));
    }

    TEST(xaxis, access)
    {
        label_type l = { "a", "b", "c" };
        axis_type a(l);
        
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
        label_type l = { "a", "b", "c" };
        axis_type a(l);

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

    TEST(xaxis, merge)
    {
        label_type l1 = { "a", "b", "d", "e" };
        label_type l2 = { "b", "c", "d" };
        label_type l3 = { "c", "g" };

        axis_type a1(l1), a2(l2), a3(l3);
        axis_type res; 
        merge_axes(res, a1, a2, a3);
        EXPECT_EQ(res["a"], 0);
        EXPECT_EQ(res["b"], 1);
        EXPECT_EQ(res["c"], 2);
        EXPECT_EQ(res["d"], 3);
        EXPECT_EQ(res["e"], 4);
        EXPECT_EQ(res["g"], 5);
    }
}

