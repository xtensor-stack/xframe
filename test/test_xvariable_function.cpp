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
    struct xfunction_features
    {
        variable_type m_a;
        variable_type m_b;

        xfunction_features();
    };

    xfunction_features::xfunction_features()
    {
        m_a = make_test_variable();
        m_b = make_test_variable2();
    }

    TEST(xvariable_function, size)
    {
        xfunction_features f;
        std::size_t s1 = (f.m_a + f.m_a).size();
        EXPECT_EQ(s1, 9u);
        std::size_t s2 = (f.m_a + f.m_b).size();
        EXPECT_EQ(s2, 12u);
        std::size_t s3 = (f.m_a + f.m_a).template size<join::outer>();
        EXPECT_EQ(s3, 9u);
        std::size_t s4 = (f.m_a + f.m_b).template size<join::outer>();
        EXPECT_EQ(s4, 48u);
    }

    TEST(xvariable_function, dimension)
    {
        xfunction_features f;
        std::size_t s1 = (f.m_a + f.m_a).dimension();
        EXPECT_EQ(s1, 2u);
        std::size_t s2 = (f.m_a + f.m_b).dimension();
        EXPECT_EQ(s2, 3u);
        std::size_t s3 = (f.m_a + f.m_a).template dimension<join::outer>();
        EXPECT_EQ(s3, 2u);
        std::size_t s4 = (f.m_a + f.m_b).template dimension<join::outer>();
        EXPECT_EQ(s4, 3u);
    }

    TEST(xvariable_function, dimension_labels)
    {
        xfunction_features f;
        auto l1 = (f.m_a + f.m_a).dimension_labels();
        EXPECT_EQ(l1, f.m_a.dimension_labels());
        auto l2 = (f.m_a + f.m_a).dimension_labels<join::outer>();
        EXPECT_EQ(l2, f.m_a.dimension_labels());
        auto l3 = (f.m_a + f.m_b).dimension_labels();
        EXPECT_EQ(l3, f.m_b.dimension_labels());
        auto l4 = (f.m_a + f.m_b).dimension_labels<join::outer>();
        EXPECT_EQ(l4, f.m_b.dimension_labels());
    }

    TEST(xvariable_function, coordinates)
    {
        xfunction_features f;
        auto c1 = (f.m_a + f.m_a).coordinates();
        EXPECT_EQ(c1, f.m_a.coordinates());
        auto c2 = (f.m_a + f.m_a).coordinates<join::outer>();
        EXPECT_EQ(c2, f.m_a.coordinates());
        auto c3 = (f.m_a + f.m_b).coordinates();
        EXPECT_EQ(c3, make_intersect_coordinate());
        auto c4 = (f.m_a + f.m_b).coordinates<join::outer>();
        EXPECT_EQ(c4, make_merge_coordinate());
    }

    TEST(xvariable_function, dimension_mapping)
    {
        xfunction_features f;
        auto d1 = (f.m_a + f.m_a).dimension_mapping();
        EXPECT_EQ(d1, f.m_a.dimension_mapping());
        auto d2 = (f.m_a + f.m_a).dimension_mapping<join::outer>();
        EXPECT_EQ(d2, f.m_a.dimension_mapping());
        auto d3 = (f.m_a + f.m_b).dimension_mapping();
        EXPECT_EQ(d3, f.m_b.dimension_mapping());
        auto d4 = (f.m_a + f.m_b).dimension_mapping<join::outer>();
        EXPECT_EQ(d4, f.m_b.dimension_mapping());
    }

    TEST(xvariable_function, shape)
    {
        xfunction_features f;
        using shape_type = std::decay_t<decltype(f.m_a.shape())>;
        auto s1 = (f.m_a + f.m_a).shape();
        EXPECT_EQ(s1, shape_type({ 3, 3 }));
        auto s2 = (f.m_a + f.m_b).shape();
        EXPECT_EQ(s2, shape_type({ 3, 3, 3 }));
    }

    TEST(xvariable_function, access)
    {
        xfunction_features f;
        std::size_t i = f.m_a.data().shape()[0] - 1;
        std::size_t j = f.m_a.data().shape()[1] - 1;

        {
            SCOPED_TRACE("same shape");
            xtl::xoptional<double> a = (f.m_a + f.m_a)(i, j);
            xtl::xoptional<double> b = f.m_a(i, j) + f.m_a(i, j);
            EXPECT_EQ(a, b);
        }

        {
            SCOPED_TRACE("different shape");
            xtl::xoptional<double> a = (f.m_a + f.m_b)(1, i, j);
            xtl::xoptional<double> b = f.m_a(1, i, j) + f.m_b(1, i, j);
            EXPECT_EQ(a, b);
        }
    }

    TEST(xvariable_function, broadcast_coordinate)
    {
        xfunction_features f;

        coordinate_type c1;
        xtrivial_broadcast res1 = (f.m_a + f.m_a).broadcast_coordinates(c1);
        EXPECT_EQ(c1, f.m_a.coordinates());
        EXPECT_TRUE(res1.m_same_dimensions);
        EXPECT_TRUE(res1.m_same_labels);

        coordinate_type c2;
        xtrivial_broadcast res2 = (f.m_a + f.m_a).broadcast_coordinates<join::outer>(c2);
        EXPECT_EQ(c2, f.m_a.coordinates());
        EXPECT_TRUE(res2.m_same_dimensions);
        EXPECT_TRUE(res2.m_same_labels);

        coordinate_type c3;
        xtrivial_broadcast res3 = (f.m_a + f.m_b).broadcast_coordinates(c3);
        EXPECT_EQ(c3, make_intersect_coordinate());
        EXPECT_FALSE(res3.m_same_dimensions);
        EXPECT_FALSE(res3.m_same_labels);

        coordinate_type c4;
        xtrivial_broadcast res4 = (f.m_a + f.m_b).broadcast_coordinates<join::outer>(c4);
        EXPECT_EQ(c4, make_merge_coordinate());
        EXPECT_FALSE(res4.m_same_dimensions);
        EXPECT_FALSE(res4.m_same_labels);
    }

    TEST(xvariable_function, select_inner)
    {
        xfunction_features f;

        {
            SCOPED_TRACE("same shape");
            xtl::xoptional<double> a = (f.m_a + f.m_a).select({{"abscissa", "d"}, {"ordinate", 4}});
            xtl::xoptional<double> b = 2 * f.m_a.select({{"abscissa", "d"}, {"ordinate", 4}});
            EXPECT_EQ(a, b);
            EXPECT_ANY_THROW((f.m_a + f.m_a).select({{"abscissa", "e"}, {"ordinate", 4}}));
        }

        {
            SCOPED_TRACE("different shape");
            
            xtl::xoptional<double> a = (f.m_a + f.m_b).select({{"abscissa", "d"}, {"ordinate", 4}, {"altitude", 2}});
            xtl::xoptional<double> b = f.m_a.select({{"abscissa", "d"}, {"ordinate", 4}, {"altitude", 2}}) +
                                       f.m_b.select({{"abscissa", "d"}, {"ordinate", 4}, {"altitude", 2}});
            EXPECT_EQ(a, b);
            EXPECT_ANY_THROW((f.m_a + f.m_b).select({{"abscissa", "e"}, {"ordinate", 4}, {"altitude", 2}}));
        }
    }

    TEST(xvariable_function, select_outer)
    {
        xfunction_features f;

        {
            SCOPED_TRACE("same shape");
            xtl::xoptional<double> a = (f.m_a + f.m_a).select<join::outer>({{"abscissa", "d"}, {"ordinate", 4}});
            xtl::xoptional<double> b = 2 * f.m_a.select<join::outer>({{"abscissa", "d"}, {"ordinate", 4}});
            EXPECT_EQ(a, b);
            EXPECT_EQ((f.m_a + f.m_a).select<join::outer>({{"abscissa", "e"}, {"ordinate", 4}}), f.m_a.missing());
        }

        {
            SCOPED_TRACE("different shape");
            
            xtl::xoptional<double> a = (f.m_a + f.m_b).select<join::outer>({{"abscissa", "d"}, {"ordinate", 4}, {"altitude", 2}});
            xtl::xoptional<double> b = f.m_a.select<join::outer>({{"abscissa", "d"}, {"ordinate", 4}, {"altitude", 2}}) +
                                       f.m_b.select<join::outer>({{"abscissa", "d"}, {"ordinate", 4}, {"altitude", 2}});
            EXPECT_EQ(a, b);
            EXPECT_EQ((f.m_a + f.m_b).select<join::outer>({{"abscissa", "e"}, {"ordinate", 4}, {"altitude", 2}}), f.m_a.missing());
        }
    }

    TEST(xvariable_function, print)
    {
        auto a = variable_type(
            make_test_data(),
            {
                { "day", xf::axis({ "Monday", "Tuesday", "Wednesday" }) },
                { "city", xf::axis({ "London", "Paris", "Brussels" }) }
            }
        );

        data_type db = { 1., 2., 3. };
        auto b = variable_type(
            db,
            {
                { "city", xf::axis({ "Paris", "London", "Brussels" }) }
            }
        );

        auto f = a + b;
        std::string expected =
R"variable({{  3,   3, N/A},
 {N/A,   6,   9},
 {  9,   9,  12}}
Coordinates:
day : (Monday, Tuesday, Wednesday, )
city: (London, Paris, Brussels, )
)variable";

        std::ostringstream oss;
        oss << f;
        std::string res = oss.str();
        EXPECT_EQ(res, expected);
    }
}

