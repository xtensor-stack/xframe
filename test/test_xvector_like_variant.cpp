/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "xtensor/xstorage.hpp"
#include "xframe/xvector_like_variant.hpp"

namespace xf
{
    TEST(xvector_like_variant, types)
    {
        using vector_type1 = xvector_like_variant<std::vector<double>, xt::svector<double>, xt::uvector<double>>;
        using vector_type2 = xvector_like_variant<std::vector<double>, std::vector<int>>;
        using vector_type3 = xvector_like_variant<std::vector<double>, xt::svector<int>>;

        constexpr bool res1 = std::is_same<vector_type1::value_type, double>::value;
        constexpr bool res2 = std::is_same<vector_type2::value_type, xtl::variant<double, int>>::value;
        constexpr bool res3 = std::is_same<vector_type3::value_type, xtl::variant<double, int>>::value;

        EXPECT_TRUE(res1);
        EXPECT_TRUE(res2);
        EXPECT_TRUE(res3);
    }

    TEST(xvector_like_variant_ref, types)
    {
        using vector_type1 = xvector_like_variant_ref<std::vector<double>, xt::svector<double>, xt::uvector<double>>;
        using vector_type2 = xvector_like_variant_ref<std::vector<double>, std::vector<int>>;
        using vector_type3 = xvector_like_variant_ref<std::vector<double>, xt::svector<int>>;

        constexpr bool res1 = std::is_same<vector_type1::value_type, double>::value;
        constexpr bool res2 = std::is_same<vector_type2::value_type, xtl::variant<double, int>>::value;
        constexpr bool res3 = std::is_same<vector_type3::value_type, xtl::variant<double, int>>::value;

        EXPECT_TRUE(res1);
        EXPECT_TRUE(res2);
        EXPECT_TRUE(res3);
    }

    TEST(xvector_like_variant_cref, types)
    {
        using vector_type1 = xvector_like_variant_cref<std::vector<double>, xt::svector<double>, xt::uvector<double>>;
        using vector_type2 = xvector_like_variant_cref<std::vector<double>, std::vector<int>>;
        using vector_type3 = xvector_like_variant_cref<std::vector<double>, xt::svector<int>>;

        constexpr bool res1 = std::is_same<vector_type1::value_type, double>::value;
        constexpr bool res2 = std::is_same<vector_type2::value_type, xtl::variant<double, int>>::value;
        constexpr bool res3 = std::is_same<vector_type3::value_type, xtl::variant<double, int>>::value;

        EXPECT_TRUE(res1);
        EXPECT_TRUE(res2);
        EXPECT_TRUE(res3);
    }

    using variant_type = xvector_like_variant<std::vector<double>, xt::svector<int>, xt::uvector<double>>;
    using variant_ref_type = xvector_like_variant_ref<std::vector<double>, xt::svector<int>, xt::uvector<double>>;
    using variant_cref_type = xvector_like_variant_cref<std::vector<double>, xt::svector<int>, xt::uvector<double>>;

    std::vector<double> build_vector()
    {
        return {1., 3., 4.};
    }

    xt::svector<int> build_svector()
    {
        return {1, 3, 4};
    }

    xt::uvector<double> build_uvector()
    {
        return {1., 3., 4.};
    }

    /*****************************
     * constructors / assignment *
     *****************************/
    
    TEST(xvector_like_variant, constructor)
    {
        auto v = build_vector();
        variant_type v1(v);
        variant_type v2(std::move(v));
        EXPECT_EQ(v1, v2);

        auto sv = build_svector();
        variant_type v3(sv);
        variant_type v4(std::move(sv));
        EXPECT_EQ(v3, v4);

        auto uv = build_uvector();
        variant_type v5(uv);
        variant_type v6(std::move(uv));
        EXPECT_EQ(v5, v6);
    }

    TEST(xvector_like_variant, copy_constructor)
    {
        auto v = build_vector();
        variant_type v1(v);
        variant_type v2(v1);
        EXPECT_EQ(v1, v2);

        auto sv = build_svector();
        variant_type v3(sv);
        variant_type v4(v3);
        EXPECT_EQ(v3, v4);

        auto uv = build_uvector();
        variant_type v5(uv);
        variant_type v6(v5);
        EXPECT_EQ(v5, v6);
    }

    TEST(xvector_like_variant_ref, copy_constructor)
    {
        auto v = build_vector();
        variant_ref_type v1(v);
        variant_ref_type v2(v1);
        EXPECT_EQ(v1, v2);

        auto sv = build_svector();
        variant_ref_type v3(sv);
        variant_ref_type v4(v3);
        EXPECT_EQ(v3, v4);

        auto uv = build_uvector();
        variant_ref_type v5(uv);
        variant_ref_type v6(v5);
        EXPECT_EQ(v5, v6);
    }

    TEST(xvector_like_variant_cref, copy_constructor)
    {
        auto v = build_vector();
        variant_cref_type v1(v);
        variant_cref_type v2(v1);
        EXPECT_EQ(v1, v2);

        auto sv = build_svector();
        variant_cref_type v3(sv);
        variant_cref_type v4(v3);
        EXPECT_EQ(v3, v4);

        auto uv = build_uvector();
        variant_cref_type v5(uv);
        variant_cref_type v6(v5);
        EXPECT_EQ(v5, v6);
    }

    TEST(xvector_like_variant, move_constructor)
    {
        auto v = build_vector();
        variant_type v1(v);
        variant_type v1_bis(v1);
        variant_type v2(std::move(v1_bis));
        EXPECT_EQ(v1, v2);

        auto sv = build_svector();
        variant_type v3(sv);
        variant_type v3_bis(v3);
        variant_type v4(std::move(v3_bis));
        EXPECT_EQ(v3, v4);

        auto uv = build_uvector();
        variant_type v5(uv);
        variant_type v5_bis(v5);
        variant_type v6(std::move(v5_bis));
        EXPECT_EQ(v5, v6);
    }

    TEST(xvector_like_variant_ref, move_constructor)
    {
        auto v = build_vector();
        variant_ref_type v1(v);
        variant_ref_type v1_bis(v1);
        variant_ref_type v2(std::move(v1_bis));
        EXPECT_EQ(v1, v2);

        auto sv = build_svector();
        variant_ref_type v3(sv);
        variant_ref_type v3_bis(v3);
        variant_ref_type v4(std::move(v3_bis));
        EXPECT_EQ(v3, v4);

        auto uv = build_uvector();
        variant_ref_type v5(uv);
        variant_ref_type v5_bis(v5);
        variant_ref_type v6(std::move(v5_bis));
        EXPECT_EQ(v5, v6);
    }

    TEST(xvector_like_variant_cref, move_constructor)
    {
        auto v = build_vector();
        variant_cref_type v1(v);
        variant_cref_type v1_bis(v1);
        variant_cref_type v2(std::move(v1_bis));
        EXPECT_EQ(v1, v2);

        auto sv = build_svector();
        variant_cref_type v3(sv);
        variant_cref_type v3_bis(v3);
        variant_cref_type v4(std::move(v3_bis));
        EXPECT_EQ(v3, v4);

        auto uv = build_uvector();
        variant_cref_type v5(uv);
        variant_cref_type v5_bis(v5);
        variant_cref_type v6(std::move(v5_bis));
        EXPECT_EQ(v5, v6);
    }

    TEST(xvector_like_variant, assign)
    {
        auto v = build_vector();
        variant_type v1(v);

        auto uv = build_uvector();
        variant_type v2(uv);
        v1 = v2;
        EXPECT_EQ(v1, v2);
    }
    
    TEST(xvector_like_variant_ref, assign)
    {
        auto v = build_vector();
        variant_ref_type v1(v);

        auto uv = build_uvector();
        variant_ref_type v2(uv);
        v1 = v2;
        EXPECT_EQ(v1, v2);
    }

    TEST(xvector_like_variant, move_assign)
    {
        auto v = build_vector();
        variant_type v1(v);

        auto uv = build_uvector();
        variant_type v2_bis(uv);
        variant_type v2(v2_bis);
        v1 = std::move(v2_bis);
        EXPECT_EQ(v1, v2);
    }

    TEST(xvector_like_variant_ref, move_assign)
    {
        auto v = build_vector();
        variant_ref_type v1(v);

        auto uv = build_uvector();
        variant_ref_type v2_bis(uv);
        variant_ref_type v2(v2_bis);
        v1 = std::move(v2_bis);
        EXPECT_EQ(v1, v2);
    }

    /*********************
     * size and capacity *
     *********************/

    TEST(xvector_like_variant, empty)
    {
        auto v = build_vector();
        variant_type vt(v);
        EXPECT_FALSE(vt.empty());

        std::vector<double> v2;
        variant_type vt2(v2);
        EXPECT_TRUE(vt2.empty());
    }

    TEST(xvector_like_variant_ref, empty)
    {
        auto v = build_vector();
        variant_ref_type vt(v);
        EXPECT_FALSE(vt.empty());

        std::vector<double> v2;
        variant_ref_type vt2(v2);
        EXPECT_TRUE(vt2.empty());
    }

    TEST(xvector_like_variant_cref, empty)
    {
        auto v = build_vector();
        variant_cref_type vt(v);
        EXPECT_FALSE(vt.empty());

        std::vector<double> v2;
        variant_cref_type vt2(v2);
        EXPECT_TRUE(vt2.empty());
    }

    TEST(xvector_like_variant, size)
    {
        auto v = build_vector();
        variant_type vt(v);
        variant_type vt2(build_vector());

        EXPECT_EQ(vt.size(), v.size());
        EXPECT_EQ(vt2.size(), v.size());
    }

    TEST(xvector_like_variant_ref, size)
    {
        auto v = build_vector();
        auto v2= build_vector();
        variant_ref_type vt(v);
        variant_ref_type vt2(v2);

        EXPECT_EQ(vt.size(), v.size());
        EXPECT_EQ(vt2.size(), v.size());
    }

    TEST(xvector_like_variant_cref, size)
    {
        auto v = build_vector();
        auto v2= build_vector();
        variant_cref_type vt(v);
        variant_cref_type vt2(v2);

        EXPECT_EQ(vt.size(), v.size());
        EXPECT_EQ(vt2.size(), v.size());
    }

    TEST(xvector_like_variant, resize)
    {
        auto v = build_vector();
        variant_type vt(v);

        vt.resize(8u);
        EXPECT_EQ(vt.size(), 8u);
    }

    TEST(xvector_like_variant_ref, resize)
    {
        auto v = build_vector();
        variant_ref_type vt(v);

        vt.resize(8u);
        EXPECT_EQ(vt.size(), 8u);
        EXPECT_EQ(v.size(), 8u);
    }

    // Needs fix in xtensor
    /*TEST(xvector_like_variant, max_size)
    {
        auto v = build_vector();
        variant_type vt(v);
        variant_type vt2(build_vector());

        EXPECT_EQ(vt.max_size(), v.max_size());
        EXPECT_EQ(vt2.max_size(), v.max_size());
    }

    TEST(xvector_like_variant, capacity)
    {
        auto v = build_vector();
        variant_type vt(v);
        variant_type vt2(build_vector());

        EXPECT_EQ(vt.capacity(), v.capacity());
        EXPECT_EQ(vt2.capacity(), v.capacity());
    }

    TEST(xvector_like_variant, reserve)
    {
        auto v = build_vector();
        variant_type vt(v);
        vt.reserve(20u);
        EXPECT_EQ(vt.capacity(), 20u);
    }

    TEST(xvector_like_variant, shrink_to_fit)
    {
        auto v = build_vector();
        variant_type vt(v);
        vt.resize(2);
        vt.shrink_to_fit();
        EXPECT_EQ(vt.size(), vt.capacity());
    }

    TEST(xvector_like_variant, clear)
    {
        auto v = build_vector();
        variant_type vt(v);

        vt.clear();
        EXPECT_TRUE(vt.empty());
    }*/

    /**********
     * Access *
     **********/
    
    TEST(xvector_like_variant, access)
    {
        auto v = build_svector();
        variant_type vt(v);
        const variant_type& vt2(vt);

        xtl::xget<int&>(vt[0]) = 4;
        int d = xtl::xget<const int&>(vt[0]);
        EXPECT_EQ(d, 4);

        int d2 = xtl::xget<const int&>(vt2[1]);
        EXPECT_EQ(d2, v[1]);
    }

    TEST(xvector_like_variant_ref, access)
    {
        auto v = build_svector();
        variant_ref_type vt(v);

        xtl::xget<int&>(vt[0]) = 4;
        int d = xtl::xget<const int&>(vt[0]);
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v[0], 4);
    }

    TEST(xvector_like_variant_cref, access)
    {
        auto v = build_svector();
        variant_cref_type vt(v);

        int d = xtl::xget<const int&>(vt[0]);
        EXPECT_EQ(v[0], d);
    }

    /*TEST(xvector_like_variant, at)
    {
        auto v = build_svector();
        variant_type vt(v);
        const variant_type& vt2(vt);

        xtl::xget<int&>(vt.at(0)) = 4;
        int d = xtl::xget<const int&>(vt.at(0));
        EXPECT_EQ(d, 4);

        int d2 = xtl::xget<const int&>(vt2.at(1));
        EXPECT_EQ(d2, v[1]);

        EXPECT_ANY_THROW(vt.at(6));
    }

    TEST(xvector_like_variant_ref, at)
    {
        auto v = build_svector();
        variant_ref_type vt(v);

        xtl::xget<int&>(vt.at(0)) = 4;
        int d = xtl::xget<const int&>(vt.at(0));
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v[0], 4);

        EXPECT_ANY_THROW(vt.at(6));
    }

    TEST(xvector_like_variant_cref, at)
    {
        auto v = build_svector();
        variant_ref_type vt(v);

        int d = xtl::xget<const int&>(vt.at(0));
        EXPECT_EQ(v[0], d);

        EXPECT_ANY_THROW(vt.at(6));
    }*/

    TEST(xvector_like_variant, front)
    {
        auto v = build_svector();
        variant_type vt(v);
        const variant_type& vt2(vt);

        xtl::xget<int&>(vt.front()) = 4;
        int d = xtl::xget<const int&>(vt.front());
        EXPECT_EQ(d, 4);

        int d2 = xtl::xget<const int&>(vt2.front());
        EXPECT_EQ(d2, 4);
    }

    TEST(xvector_like_variant_ref, front)
    {
        auto v = build_svector();
        variant_ref_type vt(v);

        xtl::xget<int&>(vt.front()) = 4;
        int d = xtl::xget<const int&>(vt.front());
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v.front(), 4);
    }

    TEST(xvector_like_variant_cref, front)
    {
        auto v = build_svector();
        variant_ref_type vt(v);

        int d = xtl::xget<const int&>(vt.front());
        EXPECT_EQ(v.front(), d);
    }

    TEST(xvector_like_variant, back)
    {
        auto v = build_svector();
        variant_type vt(v);
        const variant_type& vt2(vt);

        xtl::xget<int&>(vt.back()) = 4;
        int d = xtl::xget<const int&>(vt.back());
        EXPECT_EQ(d, 4);

        int d2 = xtl::xget<const int&>(vt2.back());
        EXPECT_EQ(d2, 4);
    }

    TEST(xvector_like_variant_ref, back)
    {
        auto v = build_svector();
        variant_ref_type vt(v);

        xtl::xget<int&>(vt.back()) = 4;
        int d = xtl::xget<const int&>(vt.back());
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v.back(), 4);
    }

    TEST(xvector_like_variant_cref, back)
    {
        auto v = build_svector();
        variant_ref_type vt(v);

        int d = xtl::xget<const int&>(vt.back());
        EXPECT_EQ(v.back(), d);
    }

    TEST(xvector_like_variant, data)
    {
        auto v = build_svector();
        variant_type vt(v);
        const variant_type& vt2 = vt;

        xtl::xget<int*>(vt.data())[0] = 4;
        int d = xtl::xget<const int*>(vt2.data())[0];
        EXPECT_EQ(d, 4);
    }

    TEST(xvector_like_variant_ref, data)
    {
        auto v = build_svector();
        variant_ref_type vt(v);
        const variant_ref_type& vt2 = vt;

        xtl::xget<int*>(vt.data())[0] = 4;
        int d = xtl::xget<const int*>(vt2.data())[0];
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v[0], 4);
    }

    TEST(xvector_like_variant_cref, data)
    {
        auto v = build_svector();
        variant_cref_type vt(v);

        int d = xtl::xget<const int*>(vt.data())[0];
        EXPECT_EQ(v[0], d);
    }

    TEST(xvector_like_variant, storage)
    {
        auto v = build_svector();
        variant_type vt(v);
        const variant_type& vt2 = vt;

        xtl::xget<xt::svector<int>>(vt.storage())[0] = 4;
        int d = xtl::xget<xt::svector<int>>(vt2.storage())[0];
        EXPECT_EQ(d, 4);
    }

    TEST(xvector_like_variant_ref, storage)
    {
        auto v = build_svector();
        variant_ref_type vt(v);
        const variant_ref_type& vt2 = vt;

        xtl::xget<xt::svector<int>&>(vt.storage())[0] = 4;
        int d = xtl::xget<const xt::svector<int>&>(vt2.storage())[0];
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v[0], 4);
    }

    TEST(xvector_like_variant_cref, storage)
    {
        auto v = build_svector();
        variant_cref_type vt(v);

        int d = xtl::xget<const xt::svector<int>&>(vt.storage())[0];
        EXPECT_EQ(v[0], d);
    }
}

