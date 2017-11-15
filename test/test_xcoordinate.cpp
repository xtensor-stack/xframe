/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/


#include "gtest/gtest.h"
#include "xframe/xcoordinate.hpp"
#include "xtl/xbasic_fixed_string.hpp"

namespace xf
{
    using fstring = xtl::xfixed_string<55>;
    using slabel_type = std::vector<fstring>;
    using saxis_type = xaxis<fstring, std::size_t>;
    using ilabel_type = std::vector<int>;
    using iaxis_type = xaxis<int, std::size_t>;

    namespace
    {
        auto make_test_saxis()
        {
            return saxis_type({ "a", "c", "d" });
        }

        auto make_test_iaxis()
        {
            return iaxis_type({ 1, 2, 4 });
        }

        auto make_test_coordinate()
        {
            fstring n1 = "temperature";
            fstring n2 = "pressure";

            return coordinate(std::make_pair(n1, make_test_saxis()), std::make_pair(n2, make_test_iaxis()));
        }
    }

    TEST(xcoordinate, constructor)
    {
        auto c1 = make_test_coordinate();
        
        using map_type = typename std::decay_t<decltype(c1)>::map_type;
        map_type m;
        m["temperature"] = make_test_saxis();
        m["pressure"] = make_test_iaxis();

        auto c2 = coordinate(m);
        auto c3 = coordinate(std::move(m));

        EXPECT_EQ(c1, c2);
        EXPECT_EQ(c1, c3);
        EXPECT_FALSE(c1 != c2);

        decltype(c1) c4 = {{ fstring("temperature"), make_test_saxis() }, { fstring("pressure"), make_test_iaxis() }};
        EXPECT_EQ(c1, c4);

        decltype(c1) c5 = {{"temperature", make_test_saxis()}, {"pressure", make_test_iaxis()}};
        EXPECT_EQ(c1, c5);
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
        EXPECT_TRUE(c.contains("temperature"));
        EXPECT_FALSE(c.contains("humidity"));
    }

    TEST(xcoordinate, access)
    {
        auto c = make_test_coordinate();
        EXPECT_EQ(c["temperature"]["a"], 0);
        EXPECT_EQ(c["temperature"]["a"], 0);
        EXPECT_EQ(c["temperature"]["c"], 1);
        EXPECT_EQ(c["temperature"]["d"], 2);
        EXPECT_EQ(c["pressure"][1], 0);
        EXPECT_EQ(c["pressure"][2], 1);
        EXPECT_EQ(c["pressure"][4], 2);

        EXPECT_EQ(c[std::make_pair("temperature", fstring("a"))], 0);
        EXPECT_EQ(c[std::make_pair("temperature", fstring("c"))], 1);
        EXPECT_EQ(c[std::make_pair("temperature", fstring("d"))], 2);
        EXPECT_EQ(c[std::make_pair("pressure", 1)], 0);
        EXPECT_EQ(c[std::make_pair("pressure", 2)], 1);
        EXPECT_EQ(c[std::make_pair("pressure", 4)], 2);
    }

    TEST(xcoordinate, iterator)
    {
        auto c = make_test_coordinate();
        auto iter = c.begin();
        EXPECT_EQ((iter->second)["d"], 2);
        ++iter;
        EXPECT_EQ((iter->second)[2], 1);
        ++iter;
        EXPECT_EQ(iter, c.end());
    }

    auto make_test_saxis2()
    {
        slabel_type sl = { "a", "d", "e" };
        saxis_type sa(std::move(sl));
        return sa;
    }

    auto make_test_iaxis2()
    {
        ilabel_type il = { 1, 4, 5 };
        iaxis_type ia(std::move(il));
        return ia;
    }

    auto make_test_coordinate2()
    {
        return coordinate(std::make_pair(fstring("temperature"), make_test_saxis2()),
                          std::make_pair(fstring("pressure"), make_test_iaxis2()),
                          std::make_pair(fstring("altitude"), make_test_iaxis()));
    }

    auto make_test_saxis_res()
    {
        slabel_type sl = { "a", "c", "d", "e" };
        saxis_type sa(std::move(sl));
        return sa;
    }

    auto make_test_iaxis_res()
    {
        ilabel_type il = { 1, 2, 4, 5 };
        iaxis_type ia(std::move(il));
        return ia;
    }

    auto make_test_coordinate_res()
    {
        return coordinate(std::make_pair(fstring("temperature"), make_test_saxis_res()),
                          std::make_pair(fstring("pressure"), make_test_iaxis_res()),
                          std::make_pair(fstring("altitude"), make_test_iaxis()));
    }

    TEST(xcoordinate, merge)
    {
        auto c1 = make_test_coordinate();
        decltype(c1) cres1;
        auto res1 = merge_coordinates(cres1, c1, c1);
        EXPECT_TRUE(res1.first);
        EXPECT_TRUE(res1.second);
        EXPECT_EQ(c1, cres1);

        auto c2 = make_test_coordinate2();
        decltype(c2) cres2;
        auto res2 = merge_coordinates(cres2, c1, c2);
        EXPECT_FALSE(res2.first);
        EXPECT_FALSE(res2.second);
        EXPECT_EQ(cres2, make_test_coordinate_res());
    }

    TEST(xcoordinate, intersect)
    {
        auto c1 = make_test_coordinate();
        auto c2 = make_test_coordinate2();

        slabel_type sres = { "a", "d" };
        ilabel_type ires = { 1, 4 };
        auto coord_res = coordinate(std::make_pair(fstring("temperature"), saxis_type(std::move(sres))),
                                    std::make_pair(fstring("pressure"), iaxis_type(std::move(ires))));

        auto cres = c1;
        auto res = intersect_coordinates(cres, c2);
        EXPECT_FALSE(res.first);
        EXPECT_FALSE(res.second);
        EXPECT_EQ(cres, coord_res);
    }
}

