/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <cmath>
#include "gtest/gtest.h"
#include "xframe/xsequence_view.hpp"

namespace xf
{
    class slice_test
    {
    public:

        using size_type = std::size_t;

        slice_test() = default;
        slice_test(size_type min_val, size_type max_val, size_type step) noexcept
            : m_min(min_val), m_size(size_type(std::ceil(double(max_val - min_val) / double(step)))), m_step(step)
        {
        }

        std::size_t size() const noexcept { return m_size; }
        size_type operator()(size_type i) const noexcept { return m_min + i * m_step; }

    private:

        size_type m_min;
        size_type m_size;
        size_type m_step;
    };

    struct xsequence_view_feature
    {
        xsequence_view_feature();

        using data_type = std::vector<int>;
        using view_type = xsequence_view<data_type, slice_test>;
        data_type m_data;
        slice_test m_slice;
        view_type m_view;
    };

    xsequence_view_feature::xsequence_view_feature()
        : m_data({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }), m_slice(0, 10, 2), m_view(m_data, m_slice)
    {
    }

    TEST(xsequence_view, size)
    {
        xsequence_view_feature m_feature;
        EXPECT_EQ(m_feature.m_view.size(), 5u);
    }

    TEST(xsequence_view, empty)
    {
        xsequence_view_feature m_feature;
        EXPECT_FALSE(m_feature.m_view.empty());
    }

    TEST(xsequence_view, access)
    {
        xsequence_view_feature m_feature;
        EXPECT_EQ(m_feature.m_view[0], 0);
        EXPECT_EQ(m_feature.m_view[1], 2);
        EXPECT_EQ(m_feature.m_view[2], 4);
        EXPECT_EQ(m_feature.m_view[3], 6);
        EXPECT_EQ(m_feature.m_view[4], 8);

        EXPECT_EQ(m_feature.m_view.at(0), 0);
        EXPECT_EQ(m_feature.m_view.at(1), 2);
        EXPECT_EQ(m_feature.m_view.at(2), 4);
        EXPECT_EQ(m_feature.m_view.at(3), 6);
        EXPECT_EQ(m_feature.m_view.at(4), 8);
        EXPECT_ANY_THROW(m_feature.m_view.at(5));

        EXPECT_EQ(m_feature.m_view.front(), 0);
        EXPECT_EQ(m_feature.m_view.back(), 8);
    }

    TEST(xsequence_view, iterator)
    {
        xsequence_view_feature m_feature;
        auto iter = m_feature.m_view.begin();
        auto citer = m_feature.m_view.cbegin();

        for (size_t i = 0; i < m_feature.m_view.size(); ++i)
        {
            EXPECT_EQ(*iter, m_feature.m_view[i]);
            EXPECT_EQ(*citer, m_feature.m_view[i]);
            ++iter, ++citer;
        }

        EXPECT_EQ(iter, m_feature.m_view.end());
        EXPECT_EQ(citer, m_feature.m_view.cend());

        iter = m_feature.m_view.begin();
        citer = m_feature.m_view.cbegin();
        iter += 2;
        citer += 2;
        EXPECT_EQ(*iter, m_feature.m_view[2]);
        EXPECT_EQ(*citer, m_feature.m_view[2]);
        EXPECT_EQ(m_feature.m_view.end() - iter, 3);
        EXPECT_EQ(m_feature.m_view.cend() - citer, 3);
    }

    TEST(xsequence_view, comparison)
    {
        xsequence_view_feature m_feature;
        xsequence_view_feature::data_type data = { 0, 2, 4, 6, 8 };
        xsequence_view_feature::data_type data2 = { 0, 2, 5, 6, 8 };

        EXPECT_TRUE(m_feature.m_view == data);
        EXPECT_TRUE(m_feature.m_view != data2);

        EXPECT_TRUE(data == m_feature.m_view);
        EXPECT_TRUE(data2 != m_feature.m_view);
    }
}
