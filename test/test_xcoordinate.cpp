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

    auto make_test_saxis()
    {
        slabel_type sl = { "a", "c", "d" };
        saxis_type sa(std::move(sl));
        return sa;
    }

    auto make_test_iaxis()
    {
        ilabel_type il = { 1, 2, 4 };
        iaxis_type ia(std::move(il));
        return ia;
    }

    auto make_test_coordinate()
    {
        fstring n1 = "temperature";
        fstring n2 = "pression";

        return coordinate(std::make_pair(n1, make_test_saxis()), std::make_pair(n2, make_test_iaxis()));
    }

    TEST(xcoordinate, constructor)
    {
        auto c1 = make_test_coordinate();
        
        using map_type = typename std::decay_t<decltype(c1)>::map_type;
        map_type m;
        m["temperature"] = make_test_saxis();
        m["pression"] = make_test_iaxis();

        auto c2 = coordinate(m);
        auto c3 = coordinate(std::move(m));

        EXPECT_EQ(c1, c2);
        EXPECT_EQ(c1, c3);
        EXPECT_FALSE(c1 != c2);
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
        EXPECT_EQ(xtl::get<saxis_type>(c["temperature"])["a"], 0);
        EXPECT_EQ(xtl::get<saxis_type>(c["temperature"])["c"], 1);
        EXPECT_EQ(xtl::get<saxis_type>(c["temperature"])["d"], 2);
        EXPECT_EQ(xtl::get<iaxis_type>(c["pression"])[1], 0);
        EXPECT_EQ(xtl::get<iaxis_type>(c["pression"])[2], 1);
        EXPECT_EQ(xtl::get<iaxis_type>(c["pression"])[4], 2);

        EXPECT_EQ(c[std::make_pair("temperature", fstring("a"))], 0);
        EXPECT_EQ(c[std::make_pair("temperature", fstring("c"))], 1);
        EXPECT_EQ(c[std::make_pair("temperature", fstring("d"))], 2);
        EXPECT_EQ(c[std::make_pair("pression", 1)], 0);
        EXPECT_EQ(c[std::make_pair("pression", 2)], 1);
        EXPECT_EQ(c[std::make_pair("pression", 4)], 2);
    }

    template <class L>
    struct iterator_tester;

    template <>
    struct iterator_tester<fstring>
    {
        template <class S>
        static bool run(const xaxis<fstring, S>& arg)
        {
            return arg["d"] == 2;
        }
    };

    template <>
    struct iterator_tester<int>
    {
        template <class S>
        static bool run(const xaxis<int, S>& arg)
        {
            return arg[2] == 1;
        }
    };

    TEST(xcoordinate, iterator)
    {
        auto c = make_test_coordinate();
        auto lambda = [](auto&& arg) -> bool
        {
            using axis_type = std::decay_t<decltype(arg)>;
            return iterator_tester<typename axis_type::key_type>::run(arg);
        };

        auto iter = c.begin();
        EXPECT_TRUE(xtl::visit(lambda, iter->second));
        ++iter;
        EXPECT_TRUE(xtl::visit(lambda, iter->second));
        ++iter;
        EXPECT_EQ(iter, c.end());
    }

}

