/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <cstddef>
#include <vector>
#include "gtest/gtest.h"
#include "xtl/xbasic_fixed_string.hpp"
#include "xframe/xaxis_view.hpp"

namespace xf
{
    using fstring = xtl::xfixed_string<55>;
    using saxis_type = xaxis<fstring, std::size_t>;
    using size_type = std::size_t;
    using axis_variant = xaxis_variant<DEFAULT_LABEL_LIST, size_type>;
    using view_type = xaxis_view<DEFAULT_LABEL_LIST, size_type, typename axis_variant::map_container_tag>;
    using slice_type = xaxis_istepped_range<size_type>;

    // { "a", "c", "d" }
    inline axis_variant make_test_saxis()
    {
        return axis_variant(saxis_type({ "a", "c", "d", "f", "g", "h", "m", "n" }));
    }

    // stepped_range(1, 7, 2)
    inline slice_type make_slice()
    {
        return slice_type(size_type(1), size_type(7), size_type(2));
    }

    TEST(xaxis_view, label)
    {
        auto a = make_test_saxis();
        view_type v = view_type(a, make_slice());
        EXPECT_EQ(xtl::get<fstring>(v.label(0)), v.labels<fstring>()[0]);
    }

    TEST(xaxis_view, size)
    {
        auto a = make_test_saxis();
        view_type v = view_type(a, make_slice());
        EXPECT_EQ(v.size(), 3);
        EXPECT_FALSE(v.empty());

        view_type v2 = view_type(a, slice_type(size_type(1), size_type(1), size_type(2)));
        EXPECT_EQ(v2.size(), 0u);
        EXPECT_TRUE(v2.empty());
    }

    TEST(xaxis_view, contains)
    {
        auto a = make_test_saxis();
        view_type v = view_type(a, make_slice());

        EXPECT_TRUE(v.contains("c"));
        EXPECT_FALSE(v.contains("d"));
    }

    TEST(xaxis_view, access)
    {
        auto a = make_test_saxis();
        view_type v = view_type(a, make_slice());

        auto vc = v["c"];
        auto vf = v["f"];
        auto vh = v["h"];

        EXPECT_EQ(vc, 1u);
        EXPECT_EQ(vf, 3u);
        EXPECT_EQ(vh, 5u);
        EXPECT_THROW(v["m"], std::out_of_range);
    }

    TEST(xaxis_view, iterator)
    {
        auto a = make_test_saxis();
        view_type v = view_type(a, make_slice());

        auto it = v.begin();

        EXPECT_TRUE(it == v.cbegin());
        EXPECT_TRUE(it != v.end());
        EXPECT_TRUE(it < v.end());
        EXPECT_TRUE(it <= v.end());
        EXPECT_FALSE(it > v.end());
        EXPECT_FALSE(it >= v.end());
        EXPECT_EQ(v.end(), v.cend());

        // TODO: fix xaxis_variant iterator
        /*EXPECT_EQ(it->first, "c");
        EXPECT_EQ(it->second, v["c"]);
        ++it;
        EXPECT_EQ(it->first, "f");
        EXPECT_EQ(it->second, v["f"]);
        auto tmp = it++;
        EXPECT_EQ(tmp->first, "f");
        EXPECT_EQ(tmp->second, v["f"]);
        EXPECT_EQ(it->first, "h");
        EXPECT_EQ(it->second, v["h"]);
        ++it;
        EXPECT_EQ(it, a.end());

        EXPECT_EQ(v["h"], (v.begin() + 2)->first);
        EXPECT_EQ(v["c"], (v.end() - 2)->first);
        EXPECT_EQ(3, v.end() - v.begin());*/
    }
}
