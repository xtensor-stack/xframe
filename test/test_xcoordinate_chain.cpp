/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"
#include "test_fixture.hpp"

#include "xframe/xcoordinate_chain.hpp"

namespace xf
{
    using slabel_type = std::vector<fstring>;
    using ilabel_type = std::vector<int>;

    inline saxis_type make_test_saxis3()
    {
        return saxis_type({"a", "b", "c", "d"});
    }

    TEST(xcoordinate_chain, constructor)
    {
        auto c = make_test_coordinate();
        using map_type = typename std::decay_t<decltype(c)>::map_type;
        map_type m;
        m["altitude"] = make_test_saxis3();

        EXPECT_ANY_THROW(reindex(c, m));
    }

    template <class C>
    inline xcoordinate_chain<C> make_coordinate_chain(const C& c)
    {
        using map_type = typename std::decay_t<C>::map_type;
        map_type m;
        m["abscissa"] = make_test_saxis3();
        return reindex(c, std::move(m));
    }

    TEST(xcoordinate_chain, size)
    {
        auto c = make_test_coordinate();
        auto cc = make_coordinate_chain(c);

        EXPECT_EQ(2u, cc.size());
        EXPECT_FALSE(cc.empty());
    }

    TEST(xcoordinate_chain, contains)
    {
        auto c = make_test_coordinate();
        auto cc = make_coordinate_chain(c);

        EXPECT_TRUE(cc.contains("abscissa"));
        EXPECT_TRUE(cc.contains("ordinate"));
        EXPECT_FALSE(cc.contains("altitude"));
    }

    TEST(xcoordinate_chain, access)
    {
        auto c = make_test_coordinate();
        auto cc = make_coordinate_chain(c);

        EXPECT_EQ(cc["abscissa"]["a"], 0);
        EXPECT_EQ(cc["abscissa"]["b"], 1);
        EXPECT_EQ(cc["abscissa"]["c"], 2);
        EXPECT_EQ(cc["abscissa"]["d"], 3);
        EXPECT_EQ(cc["ordinate"][1], 0);
        EXPECT_EQ(cc["ordinate"][2], 1);
        EXPECT_EQ(cc["ordinate"][4], 2);

        EXPECT_EQ(cc[std::make_pair("abscissa", fstring("a"))], 0);
        EXPECT_EQ(cc[std::make_pair("abscissa", fstring("b"))], 1);
        EXPECT_EQ(cc[std::make_pair("abscissa", fstring("c"))], 2);
        EXPECT_EQ(cc[std::make_pair("abscissa", fstring("d"))], 3);
        EXPECT_EQ(cc[std::make_pair("ordinate", 1)], 0);
        EXPECT_EQ(cc[std::make_pair("ordinate", 2)], 1);
        EXPECT_EQ(cc[std::make_pair("ordinate", 4)], 2);
    }

    TEST(xcoordinate_chain, iterator)
    {
        auto c = make_test_coordinate();
        auto cc = make_coordinate_chain(c);

        auto iter = cc.begin();
        EXPECT_EQ((iter->second)["b"], 1);
        EXPECT_EQ((iter->second)["c"], 2);
        ++iter;
        EXPECT_EQ((iter->second)[2], 1);
        ++iter;
        EXPECT_EQ(iter, cc.end());
    }

    TEST(xcoordinate_chain, key_iterator)
    {
        auto c = make_test_coordinate();
        auto cc = make_coordinate_chain(c);

        auto iter = c.key_begin();
        EXPECT_EQ(*iter, "abscissa");
        ++iter;
        EXPECT_EQ(*iter, "ordinate");
        ++iter;
        EXPECT_EQ(iter, c.key_end());
    }

    TEST(xcoordinate_chain, comparison)
    {
        auto c = make_test_coordinate();
        auto cc = make_coordinate_chain(c);

        EXPECT_TRUE(cc == cc);
        EXPECT_FALSE(cc != cc);

        using map_type = typename std::decay_t<decltype(c)>::map_type;
        map_type m;
        m["abscissa"] = saxis_type({"a", "d", "e", "f"});
        auto cc2 = reindex(c, std::move(m));
        EXPECT_FALSE(cc == cc2);
        EXPECT_TRUE(cc != cc2);
    }

    TEST(xcoordinate_chain, mixed_comparison)
    {
        auto c = make_test_coordinate();
        auto cc = make_coordinate_chain(c);
        coordinate_type c2 = {{"abscissa", make_test_saxis3()}, {"ordinate", make_test_iaxis()}};

        EXPECT_FALSE(cc == c);
        EXPECT_TRUE(cc != c);
        EXPECT_TRUE(cc == c2);
        EXPECT_FALSE(cc != c2);
    }
}

