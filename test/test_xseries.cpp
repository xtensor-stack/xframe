/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"
#include "xtensor/xtensor.hpp"
#include "xframe/xseries.hpp"
#include "xtl/xbasic_fixed_string.hpp"

namespace xf
{
    using fstring = xtl::xfixed_string<55>;
    using label_type = std::vector<fstring>;
    using expression_type = xt::xtensor<double, 1>;
    using series_type = xseries<expression_type, label_type>;

    struct tester
    {
        tester();
        label_type m_label;
        expression_type m_data;
    };

    tester::tester()
        : m_label({"a", "b", "c"}), m_data({1., 2., 3.})
    {
    }

    TEST(xseries, constructors)
    {
        tester t;
        series_type s(t.m_data, t.m_label);
        EXPECT_EQ(t.m_label, s.labels());
        EXPECT_EQ(t.m_data, s.values());
    }

    TEST(xseries, size)
    {
        tester t;
        series_type s(t.m_data, t.m_label);
        EXPECT_EQ(3u, s.size());
        EXPECT_FALSE(s.empty());

        label_type l;
        expression_type e;
        series_type s2(e, l);
        EXPECT_EQ(0u, s2.size());
        EXPECT_TRUE(s2.empty());
    }

    TEST(xseries, access)
    {
        tester t;
        series_type s(t.m_data, t.m_label);
        
        auto aa = s["a"];
        auto ab = s["b"];
        auto ac = s["c"];
        EXPECT_EQ(1.0, aa);
        EXPECT_EQ(2.0, ab);
        EXPECT_EQ(3.0, ac);
        EXPECT_THROW(s["d"], std::out_of_range);
    }

    TEST(xseries, iterator)
    {
        tester t;
        series_type s(t.m_data, t.m_label);

        const series_type& cs = s;
        auto it = s.begin();

        EXPECT_TRUE(cs.begin() == s.cbegin());
        EXPECT_TRUE(it != s.end());
        EXPECT_TRUE(it < s.end());
        EXPECT_TRUE(it <= s.end());
        EXPECT_FALSE(it > s.end());
        EXPECT_FALSE(it >= s.end());
        EXPECT_EQ(cs.end(), s.cend());

        EXPECT_EQ(it->first, "a");
        EXPECT_EQ(it->second, s["a"]);
        ++it;
        EXPECT_EQ(it->first, "b");
        EXPECT_EQ(it->second, s["b"]);
        auto tmp = it++;
        EXPECT_EQ(tmp->first, "b");
        EXPECT_EQ(it->first, "c");
        EXPECT_EQ(tmp->second, s["b"]);
        EXPECT_EQ(it->second, s["c"]);
        ++it;
        EXPECT_EQ(it, s.end());

        EXPECT_EQ(s["c"], (s.begin() + 2)->second);
        EXPECT_EQ(s["b"], (s.end() - 2)->second);
        EXPECT_EQ(3, s.end() - s.begin());
    }
}

