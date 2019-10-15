/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <algorithm>

#include "gtest/gtest.h"

#include "xtensor/xarray.hpp"
#include "xtensor/xoptional_assembly.hpp"

#include "test_fixture.hpp"

namespace xf
{
    TEST(xvariable_math, plus)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(+(a.select(sel)), (+a).select(sel));
    }

    TEST(xvariable_math, minus)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(-(a.select(sel)), (-a).select(sel));
    }

    TEST(xvariable_math, add)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) + a.select(sel), (a + a).select(sel));

        double sb = 1.2;
        EXPECT_EQ(a.select(sel) + sb, (a + sb).select(sel));

        double sa = 1.2;
        EXPECT_EQ(sa + a.select(sel), (sa + a).select(sel));
    }

    TEST(xvariable_math, substract)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) - a.select(sel), (a - a).select(sel));

        double sb = 1.2;
        EXPECT_EQ(a.select(sel) - sb, (a - sb).select(sel));

        double sa = 1.2;
        EXPECT_EQ(sa - a.select(sel), (sa - a).select(sel));
    }

    TEST(xvariable_math, multiply)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) * a.select(sel), (a * a).select(sel));

        double sb = 1.2;
        EXPECT_EQ(a.select(sel) * sb, (a * sb).select(sel));

        double sa = 1.2;
        EXPECT_EQ(sa * a.select(sel), (sa * a).select(sel));
    }

    TEST(xvariable_math, divide)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) / a.select(sel), (a / a).select(sel));

        double sb = 1.2;
        EXPECT_EQ(a.select(sel) / sb, (a / sb).select(sel));

        double sa = 1.2;
        EXPECT_EQ(sa / a.select(sel), (sa / a).select(sel));
    }

    TEST(xvariable_math, modulus)
    {
        int_variable_type a = make_test_int_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) % a.select(sel), (a % a).select(sel));

        int sb = 2;
        EXPECT_EQ(a.select(sel) % sb, (a % sb).select(sel));

        int sa = 4;
        EXPECT_EQ(sa % a.select(sel), (sa % a).select(sel));
    }

    TEST(xvariable_math, logical_and)
    {
        bool_variable_type a = make_test_bool_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) && a.select(sel), (a && a).select(sel));

        bool sb = false;
        EXPECT_EQ(a.select(sel) && sb, (a && sb).select(sel));

        bool sa = false;
        EXPECT_EQ(sa && a.select(sel), (sa && a).select(sel));
    }

    TEST(xvariable_math, logical_or)
    {
        bool_variable_type a = make_test_bool_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) || a.select(sel), (a || a).select(sel));

        bool sb = false;
        EXPECT_EQ(a.select(sel) || sb, (a || sb).select(sel));

        bool sa = false;
        EXPECT_EQ(sa || a.select(sel), (sa || a).select(sel));
    }

    TEST(xvariable_math, logical_not)
    {
        bool_variable_type a = make_test_bool_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(!(a.select(sel)), (!a).select(sel));
    }

    TEST(xvariable_math, bitwise_and)
    {
        int_variable_type a = make_test_int_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) & a.select(sel), (a & a).select(sel));

        int sb = 4;
        EXPECT_EQ(a.select(sel) & sb, (a & sb).select(sel));

        int sa = 9;
        EXPECT_EQ(sa & a.select(sel), (sa & a).select(sel));
    }

    TEST(xvariable_math, bitwise_or)
    {
        int_variable_type a = make_test_int_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) | a.select(sel), (a | a).select(sel));

        int sb = 4;
        EXPECT_EQ(a.select(sel) | sb, (a | sb).select(sel));

        int sa = 9;
        EXPECT_EQ(sa | a.select(sel), (sa | a).select(sel));
    }

    TEST(xvariable_math, bitwise_xor)
    {
        int_variable_type a = make_test_int_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(a.select(sel) ^ a.select(sel), (a ^ a).select(sel));

        int sb = 4;
        EXPECT_EQ(a.select(sel) ^ sb, (a ^ sb).select(sel));

        int sa = 9;
        EXPECT_EQ(sa ^ a.select(sel), (sa ^ a).select(sel));
    }

    TEST(xvariable_math, bitwise_not)
    {
        int_variable_type a = make_test_int_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(~(a.select(sel)), (~a).select(sel));
    }

    TEST(xvariable_math, equal)
    {
        variable_type a = make_test_variable();
        EXPECT_TRUE(a == a);
    }

    TEST(xvariable_math, not_equal)
    {
        variable_type a = make_test_variable();
        EXPECT_FALSE(a != a);
    }

    TEST(xvariable_math, less)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(xtl::xoptional<bool>(false), (a < a).select(sel));
    }

    TEST(xvariable_math, less_equal)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(xtl::xoptional<bool>(true), (a <= a).select(sel));
    }

    TEST(xvariable_math, greater)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(xtl::xoptional<bool>(false), (a > a).select(sel));
    }

    TEST(xvariable_math, greater_equal)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(xtl::xoptional<bool>(true), (a >= a).select(sel));
    }

    TEST(xvariable_math, element_wise_equal)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(xtl::xoptional<bool>(true), equal(a, a).select(sel));
    }

    TEST(xvariable_math, element_wise_not_equal)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(xtl::xoptional<bool>(false), not_equal(a, a).select(sel));
    }

    TEST(xvariable_math, abs)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(abs(a.select(sel)), (abs(a)).select(sel));
    }

    TEST(xvariable_math, fabs)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(fabs(a.select(sel)), (fabs(a)).select(sel));
    }

    TEST(xvariable_math, fmod)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(fmod(a.select(sel), a.select(sel)), fmod(a, a).select(sel));

        double sb = 1.2;
        EXPECT_EQ(fmod(a.select(sel), sb), fmod(a, sb).select(sel));

        double sa = 1.2;
        EXPECT_EQ(fmod(sa, a.select(sel)), fmod(sa, a).select(sel));
    }

    TEST(xvariable_math, remainder)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(remainder(a.select(sel), a.select(sel)), remainder(a, a).select(sel));

        double sb = 1.2;
        EXPECT_EQ(remainder(a.select(sel), sb), remainder(a, sb).select(sel));

        double sa = 1.2;
        EXPECT_EQ(remainder(sa, a.select(sel)), remainder(sa, a).select(sel));
    }

    TEST(xvariable_math, fma)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(fma(a.select(sel), a.select(sel), a.select(sel)), xf::fma(a, a, a).select(sel));

        double sb = 1.2;
        EXPECT_EQ(fma(a.select(sel), sb, a.select(sel)), xf::fma(a, sb, a).select(sel));

        double sa = 1.2;
        EXPECT_EQ(fma(sa, a.select(sel), a.select(sel)), xf::fma(sa, a, a).select(sel));

        EXPECT_EQ(fma(sa, sb, a.select(sel)), xf::fma(sa, sb, a).select(sel));
    }

    TEST(xvariable_math, fmax)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(fmax(a.select(sel), a.select(sel)), fmax(a, a).select(sel));

        double sb = 1.2;
        EXPECT_EQ(fmax(a.select(sel), sb), fmax(a, sb).select(sel));

        double sa = 1.2;
        EXPECT_EQ(fmax(sa, a.select(sel)), fmax(sa, a).select(sel));
    }

    TEST(xvariable_math, fmin)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(fmin(a.select(sel), a.select(sel)), fmin(a, a).select(sel));

        double sb = 1.2;
        EXPECT_EQ(fmin(a.select(sel), sb), fmin(a, sb).select(sel));

        double sa = 1.2;
        EXPECT_EQ(fmin(sa, a.select(sel)), fmin(sa, a).select(sel));
    }

    TEST(xvariable_math, fdim)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(fdim(a.select(sel), a.select(sel)), fdim(a, a).select(sel));

        double sb = 1.2;
        EXPECT_EQ(fdim(a.select(sel), sb), fdim(a, sb).select(sel));

        double sa = 1.2;
        EXPECT_EQ(fdim(sa, a.select(sel)), fdim(sa, a).select(sel));
    }

    TEST(xvariable_math, maximum)
    {
        auto missing = xtl::missing<double>();
        using data_type = xt::xoptional_assembly<xt::xarray<double>, xt::xarray<bool>>;

        variable_type a = make_test_variable();
        variable_type b = make_test_variable2();

        variable_type res = maximum(a, b);
        data_type expected = {{{      1,       2, missing},
                               {missing, missing, missing}},
                              {{     11,      12,      13},
                               {missing,      15,      16}}};
        EXPECT_EQ(res.data(), expected);
    }

    TEST(xvariable_math, minimum)
    {
        auto missing = xtl::missing<double>();
        using data_type = xt::xoptional_assembly<xt::xarray<double>, xt::xarray<bool>>;

        variable_type a = make_test_variable();
        variable_type b = make_test_variable2();

        variable_type res = minimum(a, b);
        data_type expected = {{{      1,       1, missing},
                               {missing, missing, missing}},
                              {{      7,       7,       7},
                               {missing,       9,       9}}};
        EXPECT_EQ(res.data(), expected);
    }

    TEST(xvariable_math, clip)
    {
        auto missing = xtl::missing<double>();
        using data_type = xt::xoptional_assembly<xt::xarray<double>, xt::xarray<bool>>;

        variable_type a = make_test_variable();

        variable_type res = clip(a, 2., 8.0);
        data_type expected = {{      2,   2, missing},
                              {missing,   5,       6},
                              {      7,   8,       8}};
        EXPECT_EQ(res.data(), expected);

        variable_type res2 = clip(a, missing, missing);
        data_type expected2 = {{missing, missing, missing},
                               {missing, missing, missing},
                               {missing, missing, missing}};
        EXPECT_EQ(res2.data(), expected2);

        variable_type res3 = clip(a, missing, 4.);
        data_type expected3 = {{missing, missing, missing},
                               {missing, missing, missing},
                               {missing, missing, missing}};
        EXPECT_EQ(res3.data(), expected3);
    }

    TEST(xvariable_math, sign)
    {
        auto missing = xtl::missing<double>();
        using data_type = xt::xoptional_assembly<xt::xarray<double>, xt::xarray<bool>>;

        variable_type a = make_test_variable();
        a(0, 0) = -5.;

        variable_type res = sign(a);
        data_type expected = {{     -1,   1, missing},
                              {missing,   1,       1},
                              {      1,   1,       1}};
        EXPECT_EQ(res.data(), expected);
    }

    TEST(xvariable_math, exp)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(exp(a.select(sel)), exp(a).select(sel));
    }

    TEST(xvariable_math, exp2)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(exp2(a.select(sel)), exp2(a).select(sel));
    }

    TEST(xvariable_math, expm1)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(expm1(a.select(sel)), expm1(a).select(sel));
    }

    TEST(xvariable_math, log)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(log(a.select(sel)), log(a).select(sel));
    }

    TEST(xvariable_math, log10)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(log10(a.select(sel)), log10(a).select(sel));
    }

    TEST(xvariable_math, log2)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(log2(a.select(sel)), log2(a).select(sel));
    }

    TEST(xvariable_math, log1p)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(log1p(a.select(sel)), log1p(a).select(sel));
    }

    TEST(xvariable_math, pow)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(pow(a.select(sel), a.select(sel)), pow(a, a).select(sel));

        double sb = 1.2;
        EXPECT_EQ(pow(a.select(sel), sb), pow(a, sb).select(sel));

        double sa = 1.2;
        EXPECT_EQ(pow(sa, a.select(sel)), pow(sa, a).select(sel));
    }

    TEST(xvariable_math, sqrt)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(sqrt(a.select(sel)), sqrt(a).select(sel));
    }

    TEST(xvariable_math, cbrt)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(cbrt(a.select(sel)), cbrt(a).select(sel));
    }

    TEST(xvariable_math, hypot)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(hypot(a.select(sel), a.select(sel)), hypot(a, a).select(sel));

        double sb = 1.2;
        EXPECT_EQ(hypot(a.select(sel), sb), hypot(a, sb).select(sel));

        double sa = 1.2;
        EXPECT_EQ(hypot(sa, a.select(sel)), hypot(sa, a).select(sel));
    }

    TEST(xvariable_math, sin)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(sin(a.select(sel)), sin(a).select(sel));
    }

    TEST(xvariable_math, cos)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(cos(a.select(sel)), cos(a).select(sel));
    }

    TEST(xvariable_math, tan)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(tan(a.select(sel)), tan(a).select(sel));
    }

    TEST(xvariable_math, asin)
    {
        variable_type a = make_test_variable() / 10.;
        dict_type sel = make_selector_aa();
        EXPECT_EQ(asin(a.select(sel)), asin(a).select(sel));
    }

    TEST(xvariable_math, acos)
    {
        variable_type a = make_test_variable() / 10.;
        dict_type sel = make_selector_aa();
        EXPECT_EQ(acos(a.select(sel)), acos(a).select(sel));
    }

    TEST(xvariable_math, atan)
    {
        variable_type a = make_test_variable() / 10.;
        dict_type sel = make_selector_aa();
        EXPECT_EQ(atan(a.select(sel)), atan(a).select(sel));
    }

    TEST(xvariable_math, atan2)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(atan2(a.select(sel), 2. * a.select(sel)), atan2(a, 2. * a).select(sel));
    }

    TEST(xvariable_math, sinh)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(sinh(a.select(sel)), sinh(a).select(sel));
    }

    TEST(xvariable_math, cosh)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(cosh(a.select(sel)), cosh(a).select(sel));
    }

    TEST(xvariable_math, tanh)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(tanh(a.select(sel)), tanh(a).select(sel));
    }

    TEST(xvariable_math, asinh)
    {
        variable_type a = make_test_variable() / 10.;
        dict_type sel = make_selector_aa();
        EXPECT_EQ(asinh(a.select(sel)), asinh(a).select(sel));
    }

    TEST(xvariable_math, acosh)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(acosh(a.select(sel)), acosh(a).select(sel));
    }

    TEST(xvariable_math, atanh)
    {
        variable_type a = make_test_variable() / 10.;
        dict_type sel = make_selector_aa();
        EXPECT_EQ(atanh(a.select(sel)), atanh(a).select(sel));
    }

    TEST(xvariable_math, erf)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(erf(a.select(sel)), erf(a).select(sel));
    }

    TEST(xvariable_math, erfc)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(erfc(a.select(sel)), erfc(a).select(sel));
    }

    TEST(xvariable_math, tgamma)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(tgamma(a.select(sel)), tgamma(a).select(sel));
    }

    TEST(xvariable_math, lgamma)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(lgamma(a.select(sel)), lgamma(a).select(sel));
    }

    TEST(xvariable_math, ceil)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(ceil(a.select(sel)), ceil(a).select(sel));
    }

    TEST(xvariable_math, floor)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(floor(a.select(sel)), floor(a).select(sel));
    }

    TEST(xvariable_math, trunc)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(trunc(a.select(sel)), trunc(a).select(sel));
    }

    TEST(xvariable_math, round)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(round(a.select(sel)), round(a).select(sel));
    }

    TEST(xvariable_math, nearbyint)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(nearbyint(a.select(sel)), nearbyint(a).select(sel));
    }

    TEST(xvariable_math, rint)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(rint(a.select(sel)), rint(a).select(sel));
    }

    TEST(xvariable_math, isfinite)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(isfinite(a.select(sel)), xf::isfinite(a).select(sel));
    }

    TEST(xvariable_math, isinf)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(isinf(a.select(sel)), xf::isinf(a).select(sel));
    }

    TEST(xvariable_math, isnan)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_EQ(isnan(a.select(sel)), xf::isnan(a).select(sel));
    }

    TEST(xvariable_math, where)
    {
        auto missing = xtl::missing<double>();
        using data_type = xt::xoptional_assembly<xt::xarray<double>, xt::xarray<bool>>;

        variable_type a = make_test_variable();
        variable_type b = make_test_variable2();

        variable_type res = where(a < 6, b, a);

        data_type expected = {{{     1.,      2., missing},
                               {missing, missing, missing}},
                              {{     7.,      7.,      7.},
                               {     9.,      9.,      9.}}};
        EXPECT_EQ(res.data(), expected);

        variable_type res2 = where(a < 6, 0., a);
        data_type expected2 = {{      0,   0, missing},
                               {missing,   0,       6},
                               {      7,   8,       9}};
        EXPECT_EQ(res2.data(), expected2);
    }

    // Needs a fix in xtensor
    /*TEST(xvariable_math, isclose)
    {
        variable_type a = make_test_variable();
        dict_type sel = make_selector_aa();
        EXPECT_TRUE(isclose(a, a).select(sel));
    }*/
}
