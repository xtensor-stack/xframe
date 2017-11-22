/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
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

        xfunction_features();
    };

    xfunction_features::xfunction_features()
    {
        m_a = make_test_variable();
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
    }
}

