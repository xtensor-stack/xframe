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

#include "xframe/xcoordinate_expanded.hpp"

namespace xf
{
    using slabel_type = std::vector<fstring>;
    using ilabel_type = std::vector<int>;

    TEST(xcoordinate_expanded, constructor)
    {
        auto c = make_test_coordinate();
        using map_type = typename std::decay_t<decltype(c)>::map_type;
        map_type m;
        m["abscissa"] = make_test_saxis3();

        EXPECT_ANY_THROW(expand_dims(c, m));
    }

    template <class C>
    inline xcoordinate_expanded<C> make_coordinate_expanded(const C& c)
    {
        using map_type = typename std::decay_t<C>::map_type;
        map_type m;
        m["new_dim"] = make_test_saxis3();
        return expand_dims(c, std::move(m));
    }

    TEST(xcoordinate_expanded, size)
    {
        auto c = make_test_coordinate();
        auto cc = make_coordinate_expanded(c);

        EXPECT_EQ(3u, cc.size());
        EXPECT_FALSE(cc.empty());
    }

    TEST(xcoordinate_expanded, contains)
    {
        auto c = make_test_coordinate();
        auto cc = make_coordinate_expanded(c);

        EXPECT_TRUE(cc.contains("new_dim"));
        EXPECT_TRUE(cc.contains("ordinate"));
        EXPECT_TRUE(cc.contains("abscissa"));
        EXPECT_FALSE(cc.contains("unknown"));

        EXPECT_TRUE(cc.contains("new_dim", "a"));
        EXPECT_FALSE(cc.contains("new_dim", "e"));
        EXPECT_TRUE(cc.contains("ordinate", 2));
        EXPECT_TRUE(cc.contains("ordinate", 4));
        EXPECT_FALSE(cc.contains("ordinate", 6));
        EXPECT_TRUE(cc.contains("abscissa", "c"));
        EXPECT_FALSE(cc.contains("abscissa", "e"));
    }

    TEST(xcoordinate_expanded, access)
    {
        auto c = make_test_coordinate();
        auto cc = make_coordinate_expanded(c);

        EXPECT_EQ(cc["new_dim"]["a"], 0u);
        EXPECT_EQ(cc["new_dim"]["b"], 1u);
        EXPECT_EQ(cc["new_dim"]["c"], 2u);
        EXPECT_EQ(cc["new_dim"]["d"], 3u);
        EXPECT_EQ(cc["ordinate"][1], 0u);
        EXPECT_EQ(cc["ordinate"][2], 1u);
        EXPECT_EQ(cc["ordinate"][4], 2u);

        EXPECT_EQ(cc[std::make_pair("new_dim", fstring("a"))], 0u);
        EXPECT_EQ(cc[std::make_pair("new_dim", fstring("b"))], 1u);
        EXPECT_EQ(cc[std::make_pair("new_dim", fstring("c"))], 2u);
        EXPECT_EQ(cc[std::make_pair("new_dim", fstring("d"))], 3u);
        EXPECT_EQ(cc[std::make_pair("ordinate", 1)], 0u);
        EXPECT_EQ(cc[std::make_pair("ordinate", 2)], 1u);
        EXPECT_EQ(cc[std::make_pair("ordinate", 4)], 2u);
    }

    TEST(xcoordinate_expanded, iterator)
    {
        auto c = make_test_coordinate();
        auto cc = make_coordinate_expanded(c);

        auto iter = cc.begin();
        EXPECT_EQ(iter->first, "abscissa");
        ++iter;
        EXPECT_EQ(iter->first, "ordinate");
        ++iter;
        EXPECT_EQ(iter->first, "new_dim");
        ++iter;
        EXPECT_EQ(iter, cc.end());
        --iter;
        EXPECT_EQ(iter->first, "new_dim");
        --iter;
        EXPECT_EQ(iter->first, "ordinate");
        --iter;
        EXPECT_EQ(iter->first, "abscissa");
        EXPECT_EQ(iter, cc.begin());
    }

    TEST(xcoordinate_expanded, find)
    {
        auto c = make_test_coordinate();
        auto cc = make_coordinate_expanded(c);

        auto iter = cc.find("new_dim");
        EXPECT_EQ((iter->second)["a"], 0u);
        auto iter2 = cc.find("not_here");
        EXPECT_EQ(iter2, cc.end());
    }

    TEST(xcoordinate_expanded, key_iterator)
    {
        auto c = make_test_coordinate();
        auto cc = make_coordinate_expanded(c);

        auto iter = cc.key_begin();

        EXPECT_EQ(*iter, "abscissa");
        ++iter;
        EXPECT_EQ(*iter, "ordinate");
        ++iter;
        EXPECT_EQ(*iter, "new_dim");
        ++iter;
        EXPECT_EQ(iter, cc.key_end());
    }

    TEST(xcoordinate_expanded, comparison)
    {
        auto c = make_test_coordinate();
        auto cc = make_coordinate_expanded(c);

        EXPECT_TRUE(cc == cc);
        EXPECT_FALSE(cc != cc);

        using map_type = typename std::decay_t<decltype(c)>::map_type;
        map_type m;
        m["new_dim"] = saxis_type({"a", "d", "e", "f"});
        auto cc2 = expand_dims(c, std::move(m));
        EXPECT_FALSE(cc == cc2);
        EXPECT_TRUE(cc != cc2);
    }

    TEST(xcoordinate_expanded, mixed_comparison)
    {
        auto c = make_test_coordinate();
        auto cc = make_coordinate_expanded(c);
        coordinate_type c2 = {{"abscissa", make_test_saxis()}, {"ordinate", make_test_iaxis()}, {"new_dim", make_test_saxis3()}};

        using xf::operator<<;
        EXPECT_NE(cc, c);
        EXPECT_EQ(cc, c2);
    }
}
