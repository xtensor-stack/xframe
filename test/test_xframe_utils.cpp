/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"
#include "xframe/xframe_utils.hpp"

#include <vector>

namespace xf
{
    TEST(xframe_utils, merge_to)
    {
        std::vector<int> v1 = { 1, 3, 4, 5, 7, 9, 10 };
        std::vector<int> v2 = { 1, 2, 4, 5, 6, 10, 11 };
        std::vector<int> v3 = { 4, 5, 8, 10 };
        std::vector<int> vres = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

        merge_to(v1, v2, v3);
        EXPECT_EQ(vres, v1);
    }
}

