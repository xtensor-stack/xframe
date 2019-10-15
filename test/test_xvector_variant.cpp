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
#include <vector>

#include "gtest/gtest.h"

#include "xtensor/xstorage.hpp"
#include "xframe/xvector_variant.hpp"

namespace xf
{
    TEST(xvector_variant, types)
    {
        using vector_type1 = xvector_variant<std::vector<double>, xt::svector<double>, xt::uvector<double>>;
        using vector_type2 = xvector_variant<std::vector<double>, std::vector<int>>;
        using vector_type3 = xvector_variant<std::vector<double>, xt::svector<int>>;

        constexpr bool res1 = std::is_same<vector_type1::value_type, double>::value;
        constexpr bool res2 = std::is_same<vector_type2::value_type, xtl::variant<double, int>>::value;
        constexpr bool res3 = std::is_same<vector_type3::value_type, xtl::variant<double, int>>::value;

        EXPECT_TRUE(res1);
        EXPECT_TRUE(res2);
        EXPECT_TRUE(res3);
    }

    TEST(xvector_variant_ref, types)
    {
        using vector_type1 = xvector_variant_ref<std::vector<double>, xt::svector<double>, xt::uvector<double>>;
        using vector_type2 = xvector_variant_ref<std::vector<double>, std::vector<int>>;
        using vector_type3 = xvector_variant_ref<std::vector<double>, xt::svector<int>>;

        constexpr bool res1 = std::is_same<vector_type1::value_type, double>::value;
        constexpr bool res2 = std::is_same<vector_type2::value_type, xtl::variant<double, int>>::value;
        constexpr bool res3 = std::is_same<vector_type3::value_type, xtl::variant<double, int>>::value;

        EXPECT_TRUE(res1);
        EXPECT_TRUE(res2);
        EXPECT_TRUE(res3);
    }

    TEST(xvector_variant_cref, types)
    {
        using vector_type1 = xvector_variant_cref<std::vector<double>, xt::svector<double>, xt::uvector<double>>;
        using vector_type2 = xvector_variant_cref<std::vector<double>, std::vector<int>>;
        using vector_type3 = xvector_variant_cref<std::vector<double>, xt::svector<int>>;

        constexpr bool res1 = std::is_same<vector_type1::value_type, double>::value;
        constexpr bool res2 = std::is_same<vector_type2::value_type, xtl::variant<double, int>>::value;
        constexpr bool res3 = std::is_same<vector_type3::value_type, xtl::variant<double, int>>::value;

        EXPECT_TRUE(res1);
        EXPECT_TRUE(res2);
        EXPECT_TRUE(res3);
    }

    using variant_type = xvector_variant<std::vector<double>, xt::svector<int>, xt::uvector<double>>;
    using variant_ref_type = xvector_variant_ref<std::vector<double>, xt::svector<int>, xt::uvector<double>>;
    using variant_cref_type = xvector_variant_cref<std::vector<double>, xt::svector<int>, xt::uvector<double>>;

    std::vector<double> build_vector()
    {
        return {1., 3., 4., 6., 7.};
    }

    xt::svector<int> build_svector()
    {
        return {1, 3, 4, 6, 7};
    }

    xt::uvector<double> build_uvector()
    {
        return {1., 3., 4., 6., 7.};
    }

    /*****************************
     * constructors / assignment *
     *****************************/
    
    TEST(xvector_variant, constructor)
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

    TEST(xvector_variant, copy_constructor)
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

    TEST(xvector_variant_ref, copy_constructor)
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

    TEST(xvector_variant_cref, copy_constructor)
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

    TEST(xvector_variant, move_constructor)
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

    TEST(xvector_variant_ref, move_constructor)
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

    TEST(xvector_variant_cref, move_constructor)
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

    TEST(xvector_variant, assign)
    {
        auto v = build_vector();
        variant_type v1(v);

        auto uv = build_uvector();
        variant_type v2(uv);
        v1 = v2;
        EXPECT_EQ(v1, v2);
    }
    
    TEST(xvector_variant_ref, assign)
    {
        auto v = build_vector();
        variant_ref_type v1(v);

        auto uv = build_uvector();
        variant_ref_type v2(uv);
        v1 = v2;
        EXPECT_EQ(v1, v2);
    }

    TEST(xvector_variant, move_assign)
    {
        auto v = build_vector();
        variant_type v1(v);

        auto uv = build_uvector();
        variant_type v2_bis(uv);
        variant_type v2(v2_bis);
        v1 = std::move(v2_bis);
        EXPECT_EQ(v1, v2);
    }

    TEST(xvector_variant_ref, move_assign)
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

    TEST(xvector_variant, empty)
    {
        auto v = build_vector();
        variant_type vt(v);
        EXPECT_FALSE(vt.empty());

        std::vector<double> v2;
        variant_type vt2(v2);
        EXPECT_TRUE(vt2.empty());
    }

    TEST(xvector_variant_ref, empty)
    {
        auto v = build_vector();
        variant_ref_type vt(v);
        EXPECT_FALSE(vt.empty());

        std::vector<double> v2;
        variant_ref_type vt2(v2);
        EXPECT_TRUE(vt2.empty());
    }

    TEST(xvector_variant_cref, empty)
    {
        auto v = build_vector();
        variant_cref_type vt(v);
        EXPECT_FALSE(vt.empty());

        std::vector<double> v2;
        variant_cref_type vt2(v2);
        EXPECT_TRUE(vt2.empty());
    }

    TEST(xvector_variant, size)
    {
        auto v = build_vector();
        variant_type vt(v);
        variant_type vt2(build_vector());

        EXPECT_EQ(vt.size(), v.size());
        EXPECT_EQ(vt2.size(), v.size());
    }

    TEST(xvector_variant_ref, size)
    {
        auto v = build_vector();
        auto v2= build_vector();
        variant_ref_type vt(v);
        variant_ref_type vt2(v2);

        EXPECT_EQ(vt.size(), v.size());
        EXPECT_EQ(vt2.size(), v.size());
    }

    TEST(xvector_variant_cref, size)
    {
        auto v = build_vector();
        auto v2= build_vector();
        variant_cref_type vt(v);
        variant_cref_type vt2(v2);

        EXPECT_EQ(vt.size(), v.size());
        EXPECT_EQ(vt2.size(), v.size());
    }

    TEST(xvector_variant, resize)
    {
        auto v = build_vector();
        variant_type vt(v);

        vt.resize(8u);
        EXPECT_EQ(vt.size(), 8u);
    }

    TEST(xvector_variant_ref, resize)
    {
        auto v = build_vector();
        variant_ref_type vt(v);

        vt.resize(8u);
        EXPECT_EQ(vt.size(), 8u);
        EXPECT_EQ(v.size(), 8u);
    }

    TEST(xvector_variant, max_size)
    {
        auto v = build_vector();
        variant_type vt(v);
        variant_type vt2(build_vector());

        EXPECT_EQ(vt.max_size(), v.max_size());
        EXPECT_EQ(vt2.max_size(), v.max_size());
    }

    TEST(xvector_variant, capacity)
    {
        auto v = build_vector();
        variant_type vt(v);
        variant_type vt2(build_vector());

        EXPECT_EQ(vt.capacity(), v.capacity());
        EXPECT_EQ(vt2.capacity(), v.capacity());
    }

    TEST(xvector_variant, reserve)
    {
        auto v = build_vector();
        variant_type vt(v);
        vt.reserve(20u);
        EXPECT_EQ(vt.capacity(), 20u);
    }

    TEST(xvector_variant, shrink_to_fit)
    {
        auto v = build_vector();
        variant_type vt(v);
        vt.resize(2);
        vt.shrink_to_fit();
        EXPECT_EQ(vt.size(), vt.capacity());
    }

    TEST(xvector_variant, clear)
    {
        auto v = build_vector();
        variant_type vt(v);

        vt.clear();
        EXPECT_TRUE(vt.empty());
    }

    /**********
     * Access *
     **********/
    
    TEST(xvector_variant, access)
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

    TEST(xvector_variant_ref, access)
    {
        auto v = build_svector();
        variant_ref_type vt(v);

        xtl::xget<int&>(vt[0]) = 4;
        int d = xtl::xget<const int&>(vt[0]);
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v[0], 4);
    }

    TEST(xvector_variant_cref, access)
    {
        auto v = build_svector();
        variant_cref_type vt(v);

        int d = xtl::xget<const int&>(vt[0]);
        EXPECT_EQ(v[0], d);
    }

    TEST(xvector_variant, at)
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

    TEST(xvector_variant_ref, at)
    {
        auto v = build_svector();
        variant_ref_type vt(v);

        xtl::xget<int&>(vt.at(0)) = 4;
        int d = xtl::xget<const int&>(vt.at(0));
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v[0], 4);

        EXPECT_ANY_THROW(vt.at(6));
    }

    TEST(xvector_variant_cref, at)
    {
        auto v = build_svector();
        variant_ref_type vt(v);

        int d = xtl::xget<const int&>(vt.at(0));
        EXPECT_EQ(v[0], d);

        EXPECT_ANY_THROW(vt.at(6));
    }

    TEST(xvector_variant, front)
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

    TEST(xvector_variant_ref, front)
    {
        auto v = build_svector();
        variant_ref_type vt(v);

        xtl::xget<int&>(vt.front()) = 4;
        int d = xtl::xget<const int&>(vt.front());
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v.front(), 4);
    }

    TEST(xvector_variant_cref, front)
    {
        auto v = build_svector();
        variant_ref_type vt(v);

        int d = xtl::xget<const int&>(vt.front());
        EXPECT_EQ(v.front(), d);
    }

    TEST(xvector_variant, back)
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

    TEST(xvector_variant_ref, back)
    {
        auto v = build_svector();
        variant_ref_type vt(v);

        xtl::xget<int&>(vt.back()) = 4;
        int d = xtl::xget<const int&>(vt.back());
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v.back(), 4);
    }

    TEST(xvector_variant_cref, back)
    {
        auto v = build_svector();
        variant_ref_type vt(v);

        int d = xtl::xget<const int&>(vt.back());
        EXPECT_EQ(v.back(), d);
    }

    TEST(xvector_variant, data)
    {
        auto v = build_svector();
        variant_type vt(v);
        const variant_type& vt2 = vt;

        xtl::xget<int*>(vt.data())[0] = 4;
        int d = xtl::xget<const int*>(vt2.data())[0];
        EXPECT_EQ(d, 4);
    }

    TEST(xvector_variant_ref, data)
    {
        auto v = build_svector();
        variant_ref_type vt(v);
        const variant_ref_type& vt2 = vt;

        xtl::xget<int*>(vt.data())[0] = 4;
        int d = xtl::xget<const int*>(vt2.data())[0];
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v[0], 4);
    }

    TEST(xvector_variant_cref, data)
    {
        auto v = build_svector();
        variant_cref_type vt(v);

        int d = xtl::xget<const int*>(vt.data())[0];
        EXPECT_EQ(v[0], d);
    }

    TEST(xvector_variant, storage)
    {
        auto v = build_svector();
        variant_type vt(v);
        const variant_type& vt2 = vt;

        xtl::xget<xt::svector<int>>(vt.storage())[0] = 4;
        int d = xtl::xget<xt::svector<int>>(vt2.storage())[0];
        EXPECT_EQ(d, 4);
    }

    TEST(xvector_variant_ref, storage)
    {
        auto v = build_svector();
        variant_ref_type vt(v);
        const variant_ref_type& vt2 = vt;

        xtl::xget<xt::svector<int>&>(vt.storage())[0] = 4;
        int d = xtl::xget<const xt::svector<int>&>(vt2.storage())[0];
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v[0], 4);
    }

    TEST(xvector_variant_cref, storage)
    {
        auto v = build_svector();
        variant_cref_type vt(v);

        int d = xtl::xget<const xt::svector<int>&>(vt.storage())[0];
        EXPECT_EQ(v[0], d);
    }

    /************
     * Iterator *
     ************/

    TEST(xvector_variant_ref, iterator)
    {
        // 1, 3, 4, 6, 7
        auto v = build_svector();
        variant_ref_type vt(v);

        auto iter = vt.begin();
        xtl::xget<int&>(*iter) = 2;
        EXPECT_EQ(v[0], 2);
        ++iter;
        xtl::xget<int&>(*iter) = 4;
        EXPECT_EQ(v[1], 4);
        iter += 2;
        xtl::xget<int&>(*iter) = 21;
        EXPECT_EQ(v[3], 21);
        iter++;
        iter -= 2;
        xtl::xget<int&>(*iter) = 35;
        EXPECT_EQ(v[2], 35);
        --iter;
        iter--;
        iter += 5;
        EXPECT_EQ(iter, vt.end());

        EXPECT_EQ(vt.end() - vt.begin(), 5);
        EXPECT_TRUE(vt.begin() < vt.end());
    }

    TEST(xvector_variant_ref, const_iterator)
    {
        // 1, 3, 4, 6, 7
        auto v = build_svector();
        variant_ref_type vt(v);

        auto iter = vt.cbegin();
        auto d0 = xtl::xget<const int&>(*iter);
        EXPECT_EQ(d0, 1);
        ++iter;
        auto d1 = xtl::xget<const int&>(*iter);
        EXPECT_EQ(d1, 3);
        iter += 2;
        auto d2 = xtl::xget<const int&>(*iter);
        EXPECT_EQ(d2, 6);
        iter++;
        iter -= 2;
        auto d3 = xtl::xget<const int&>(*iter);
        EXPECT_EQ(d3, 4);
        --iter;
        iter--;
        iter += 5;
        EXPECT_EQ(iter, vt.cend());

        EXPECT_EQ(vt.cend() - vt.cbegin(), 5);
        EXPECT_TRUE(vt.cbegin() < vt.cend());
    }

    TEST(xvector_variant_cref, const_iterator)
    {
        // 1, 3, 4, 6, 7
        auto v = build_svector();
        variant_cref_type vt(v);

        auto iter = vt.cbegin();
        auto d0 = xtl::xget<const int&>(*iter);
        EXPECT_EQ(d0, 1);
        ++iter;
        auto d1 = xtl::xget<const int&>(*iter);
        EXPECT_EQ(d1, 3);
        iter += 2;
        auto d2 = xtl::xget<const int&>(*iter);
        EXPECT_EQ(d2, 6);
        iter++;
        iter -= 2;
        auto d3 = xtl::xget<const int&>(*iter);
        EXPECT_EQ(d3, 4);
        --iter;
        iter--;
        iter += 5;
        EXPECT_EQ(iter, vt.cend());

        EXPECT_EQ(vt.cend() - vt.cbegin(), 5);
        EXPECT_TRUE(vt.cbegin() < vt.cend());
    }

    /********
     * swap *
     ********/

    TEST(xvector_variant, swap)
    {
        auto v1 = build_svector();
        auto v2 = xt::svector<int>();

        variant_type vt1(v1);
        variant_type vt2(v2);

        variant_type vt1_bu(v1);
        variant_type vt2_bu(v2);

        using std::swap;
        swap(vt1, vt2);

        EXPECT_EQ(vt1, vt2_bu);
        EXPECT_EQ(vt2, vt1_bu);
    }

    TEST(xvector_variant_ref, swap)
    {
        auto v1 = build_svector();
        auto v2 = xt::svector<int>();

        auto v1_bu = v1;
        auto v2_bu = v2;

        variant_ref_type vt1(v1);
        variant_ref_type vt2(v2);

        using std::swap;
        swap(vt1, vt2);
        
        EXPECT_EQ(v1, v2_bu);
        EXPECT_EQ(v2, v1_bu);
    }

    /***************
     * xget_vector *
     ***************/

    TEST(xvector_variant, xget_vector)
    {
        auto v = build_svector();
        variant_type vt(v);
        const variant_type& vt2(vt);

        auto& vl = xget_vector<xt::svector<int>>(vt);
        vl[0] = 4;
        const auto& cvl = xget_vector<xt::svector<int>>(vt2);
        EXPECT_EQ(cvl[0], 4);

        xt::svector<int>&& rvl = xget_vector<xt::svector<int>>(std::move(vt));
        EXPECT_EQ(rvl[0], 4);

        const xt::svector<int>&& crvl = xget_vector<xt::svector<int>>(std::move(vt2));
        EXPECT_EQ(crvl[0], 4);
    }

    TEST(xvector_variant_ref, xget_vector)
    {
        auto v = build_svector();
        variant_ref_type vt(v);
        const variant_ref_type& vt2(vt);

        auto& vl = xget_vector<xt::svector<int>>(vt);
        vl[0] = 4;
        const auto& cvl = xget_vector<xt::svector<int>>(vt2);
        EXPECT_EQ(cvl[0], 4);
        EXPECT_EQ(v[0], 4);

        auto& rvl = xget_vector<xt::svector<int>>(std::move(vt));
        EXPECT_EQ(rvl[0], 4);

        const auto& crvl = xget_vector<xt::svector<int>>(std::move(vt2));
        EXPECT_EQ(crvl[0], 4);
    }

    TEST(xvector_variant_cref, xget_vector)
    {
        auto v = build_svector();
        variant_cref_type vt(v);
        const variant_cref_type& vt2(vt);

        auto& vl = xget_vector<xt::svector<int>>(vt);
        EXPECT_EQ(vl, v);

        const auto& cvl = xget_vector<xt::svector<int>>(vt2);
        EXPECT_EQ(cvl, v);

        auto& rvl = xget_vector<xt::svector<int>>(std::move(vt));
        EXPECT_EQ(rvl, v);

        const auto& crvl = xget_vector<xt::svector<int>>(std::move(vt2));
        EXPECT_EQ(crvl, v);
    }
}

