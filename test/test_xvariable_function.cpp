/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"
#include "xframe/xvariable.hpp"

namespace xf
{
    struct xfunction_features
    {
        using fstring = xtl::xfixed_string<55>;
        using key_type = fstring;
        using variable_type = xvariable<key_type, xt::xarray<double>, xt::xarray<bool>>;
        using saxis_type = xaxis<fstring, std::size_t>;
        using iaxis_type = xaxis<int, std::size_t>;
        using data_type = xt::xoptional_assembly<xt::xarray<double>, xt::xarray<bool>>;
        variable_type m_a;

        xfunction_features();
    };

    xfunction_features::xfunction_features()
    {
        saxis_type temp_axis({"a", "c", "d"});
        iaxis_type pres_axis({1, 2, 4});
        
        data_type d = {{ 1., 2., 3.},
                       { 4., 5., 6.},
                       { 7., 8., 9.}};

        key_type t = "temperature";
        key_type p = "pressure";
        saxis_type dims({"pressure", "temperature"});
        auto coords = coordinate(std::make_pair(t, temp_axis), std::make_pair(p, pres_axis));
        m_a = variable_type(d, coords, dims);
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

