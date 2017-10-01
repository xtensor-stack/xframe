/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"
#include "xtensor/xtensor.hpp"
#include "xtensor/xnoalias.hpp"
#include "xframe/xseries.hpp"
#include "xtl/xbasic_fixed_string.hpp"

namespace xf
{
    using fstring = xtl::xfixed_string<55>;
    using label_type = std::vector<fstring>;
    using expression_type = xt::xtensor<double, 1>;
    using series_type = xseries<expression_type, label_type>;

    struct tester_math
    {
        tester_math();
        label_type m_label;
        expression_type m_data;
        expression_type m_zero;
        expression_type m_res;
    };

    tester_math::tester_math()
        : m_label({"a", "b", "c"}), m_data({1., 2., 3.}), m_zero({0., 0., 0.}), m_res({2., 4., 6.})
    {
    }

    TEST(xseries_math, plus)
    {
        tester_math t;
        series_type s1(t.m_data, t.m_label);
        series_type s2(t.m_data, t.m_label);
        series_type res(t.m_zero, t.m_label);

        xt::noalias(res) = s1 + s2;

        EXPECT_EQ(t.m_res, res.values());
    }
}

