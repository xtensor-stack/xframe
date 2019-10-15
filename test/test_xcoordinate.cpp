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

namespace xf
{
    using slabel_type = std::vector<fstring>;
    using ilabel_type = std::vector<int>;

    TEST(xcoordinate, constructor)
    {
        auto c1 = make_test_coordinate();

        using map_type = typename std::decay_t<decltype(c1)>::map_type;
        map_type m;
        m["abscissa"] = make_test_saxis();
        m["ordinate"] = make_test_iaxis();

        auto c2 = coordinate(m);
        auto c3 = coordinate(std::move(m));

        EXPECT_EQ(c1, c2);
        EXPECT_EQ(c1, c3);
        EXPECT_FALSE(c1 != c2);

        decltype(c1) c4 = {{ fstring("abscissa"), make_test_saxis() }, { fstring("ordinate"), make_test_iaxis() }};
        EXPECT_EQ(c1, c4);

        decltype(c1) c5 = {{"abscissa", make_test_saxis()}, {"ordinate", make_test_iaxis()}};
        EXPECT_EQ(c1, c5);

        auto c6 = coordinate({{"abscissa", make_test_saxis()}, {"ordinate", make_test_iaxis() }});
        EXPECT_EQ(c1, c6);
    }

    TEST(xcoordinate, size)
    {
        auto c1 = make_test_coordinate();
        EXPECT_EQ(2u, c1.size());
        EXPECT_FALSE(c1.empty());

        decltype(c1) c2;
        EXPECT_EQ(0u, c2.size());
        EXPECT_TRUE(c2.empty());
    }

    TEST(xcoordinate, contains)
    {
        auto c = make_test_coordinate();
        EXPECT_TRUE(c.contains("abscissa"));
        EXPECT_FALSE(c.contains("humidity"));
    }

    TEST(xcoordinate, access)
    {
        auto c = make_test_coordinate();
        EXPECT_EQ(c["abscissa"]["a"], 0u);
        EXPECT_EQ(c["abscissa"]["c"], 1u);
        EXPECT_EQ(c["abscissa"]["d"], 2u);
        EXPECT_EQ(c["ordinate"][1], 0u);
        EXPECT_EQ(c["ordinate"][2], 1u);
        EXPECT_EQ(c["ordinate"][4], 2u);

        EXPECT_EQ(c[std::make_pair("abscissa", fstring("a"))], 0u);
        EXPECT_EQ(c[std::make_pair("abscissa", fstring("c"))], 1u);
        EXPECT_EQ(c[std::make_pair("abscissa", fstring("d"))], 2u);
        EXPECT_EQ(c[std::make_pair("ordinate", 1)], 0u);
        EXPECT_EQ(c[std::make_pair("ordinate", 2)], 1u);
        EXPECT_EQ(c[std::make_pair("ordinate", 4)], 2u);
    }

    TEST(xcoordinate, iterator)
    {
        auto c = make_test_coordinate();
        auto iter = c.begin();
        EXPECT_EQ((iter->second)["d"], 2u);
        ++iter;
        EXPECT_EQ((iter->second)[2], 1u);
        ++iter;
        EXPECT_EQ(iter, c.end());
    }

    TEST(xcoordinate, find)
    {
        auto c = make_test_coordinate();
        auto iter = c.find("abscissa");
        EXPECT_EQ((iter->second)["d"], 2u);
        auto iter2 = c.find("not_here");
        EXPECT_EQ(iter2, c.end());
    }

    TEST(xcoordinate, key_iterator)
    {
        auto c = make_test_coordinate();
        auto iter = c.key_begin();
        EXPECT_EQ(*iter, "abscissa");
        ++iter;
        EXPECT_EQ(*iter, "ordinate");
        ++iter;
        EXPECT_EQ(iter, c.key_end());
    }

    TEST(xcoordinate, merge)
    {
        auto coord_res = make_merge_coordinate();
        auto c1 = make_test_coordinate();
        decltype(c1) cres1;
        auto res1 = broadcast_coordinates<join::outer>(cres1, c1, c1);
        EXPECT_TRUE(res1.m_same_dimensions);
        EXPECT_TRUE(res1.m_same_labels);
        EXPECT_EQ(c1, cres1);

        auto c2 = make_test_coordinate3();
        decltype(c2) cres2;
        auto res2 = broadcast_coordinates<join::outer>(cres2, c1, c2);
        EXPECT_FALSE(res2.m_same_dimensions);
        EXPECT_FALSE(res2.m_same_labels);
        EXPECT_EQ(cres2, coord_res);
    }

    TEST(xcoordinate, merge_axis_default)
    {
        auto coord_res = make_merge_coordinate2();
        auto c1 = make_test_coordinate3();
        auto c2 = make_test_coordinate4();
        decltype(c1) cres1;
        broadcast_coordinates<join::outer>(cres1, c1, c2);
        EXPECT_EQ(cres1, coord_res);

        decltype(c1) cres2;
        broadcast_coordinates<join::outer>(cres2, c2, c1);
        EXPECT_EQ(cres1, cres2);
    }

    TEST(xcoordinate, intersect)
    {
        auto c1 = make_test_coordinate();
        auto c2 = make_test_coordinate3();

        auto coord_res = make_intersect_coordinate();

        auto cres = c1;
        auto res = broadcast_coordinates<join::inner>(cres, c2);
        EXPECT_FALSE(res.m_same_dimensions);
        EXPECT_FALSE(res.m_same_labels);
        EXPECT_EQ(cres, coord_res);
    }

    TEST(xcoordinate, intersect_axis_default)
    {
        auto c1 = make_test_coordinate3();
        auto c2 = make_test_coordinate4();

        auto coord_res = make_intersect_coordinate2();

        decltype(c1) cres1;
        broadcast_coordinates<join::inner>(cres1, c1, c2);
        EXPECT_EQ(cres1, coord_res);

        decltype(c1) cres2;
        broadcast_coordinates<join::inner>(cres2, c2, c1);
        EXPECT_EQ(cres2, coord_res);
    }
}
