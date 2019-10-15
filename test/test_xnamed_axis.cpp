/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <string>
#include "gtest/gtest.h"

#include "xframe/xnamed_axis.hpp"

namespace xf
{
    TEST(xnamed_axis, get_labels)
    {
        auto a = axis(56);
        auto n_a = named_axis("axis_a", a);
        auto labels = get_labels<int>(n_a);

        EXPECT_EQ(labels, a.labels());
        EXPECT_EQ(0, labels[0]);
        EXPECT_EQ(56u, labels.size());
    }

    TEST(xnamed_axis, name)
    {
        auto a = axis(56);
        auto n_a = named_axis("axis_a", a);
        EXPECT_EQ("axis_a", n_a.name());
    }
}
