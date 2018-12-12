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

#include "xframe/xvector_variant.hpp"

namespace xf
{
    using variant_type = xvector_variant<int, std::string>;
    using variant_ref_type = xvector_variant_ref<int, std::string>;
    using variant_cref_type = xvector_variant_cref<int, std::string>;

    std::vector<int> build_test_ivector()
    {
        return {1, 3, 4, 6, 7};
    }

    /*************************
     * xvector_variant tests *
     *************************/

    TEST(xvector_variant, constructor)
    {
        auto v = build_test_ivector();
        
        // Copy
        variant_type vt(v);
        variant_type vt2(vt);
        EXPECT_EQ(vt, vt2);

        // Move
        variant_type vt3(v);
        variant_type vt4(std::move(vt3));
        EXPECT_EQ(vt, vt4);
    }

    TEST(xvector_variant, assign)
    {
        auto v = build_test_ivector();
        variant_type vt(v);

        // Copy assign
        std::vector<int> v2 = {1, 2, 3};
        variant_type vt2(v2);
        vt2 = vt;
        EXPECT_EQ(vt, vt2);
        EXPECT_FALSE(std::equal(v.cbegin(), v.cend(), v2.cbegin()));

        // Move assign
        std::vector<int> v3 = {0, 3, 4};
        variant_type vt3(v3);
        vt3 = std::move(vt2);
        EXPECT_EQ(vt3, vt);
        EXPECT_NE(v3, v);
    }

    TEST(xvector_variant, empty)
    {
        auto v = build_test_ivector();
        variant_type vt(v);
        EXPECT_FALSE(vt.empty());

        std::vector<int> v2;
        variant_type vt2(v2);
        EXPECT_TRUE(vt2.empty());
    }

    TEST(xvector_variant, size)
    {
        auto v = build_test_ivector();
        variant_type vt(v);
        variant_type vt2(build_test_ivector());

        EXPECT_EQ(vt.size(), v.size());
        EXPECT_EQ(vt2.size(), v.size());
    }

    TEST(xvector_variant, max_size)
    {
        auto v = build_test_ivector();
        variant_type vt(v);
        variant_type vt2(build_test_ivector());

        EXPECT_EQ(vt.max_size(), v.max_size());
        EXPECT_EQ(vt2.max_size(), v.max_size());
    }

    TEST(xvector_variant, capacity)
    {
        auto v = build_test_ivector();
        variant_type vt(v);
        variant_type vt2(build_test_ivector());

        EXPECT_EQ(vt.capacity(), v.capacity());
        EXPECT_EQ(vt2.capacity(), v.capacity());
    }

    TEST(xvector_variant, reserve)
    {
        auto v = build_test_ivector();
        variant_type vt(v);
        vt.reserve(20u);
        EXPECT_EQ(vt.capacity(), 20u);
    }

    TEST(xvector_variant, shrink_to_fit)
    {
        auto v = build_test_ivector();
        variant_type vt(v);
        vt.resize(2);
        vt.shrink_to_fit();
        EXPECT_EQ(vt.size(), vt.capacity());
    }

    TEST(xvector_variant, access)
    {
        auto v = build_test_ivector();
        variant_type vt(v);

        vt[0] = 4;
        int d = xtl::xget<const int&>(vt[0]);
        EXPECT_EQ(d, 4);
    }

    TEST(xvector_variant, at)
    {
        auto v = build_test_ivector();
        variant_type vt(v);

        vt.at(0) = 4;
        int d = xtl::xget<const int&>(vt.at(0));
        EXPECT_EQ(d, 4);

        EXPECT_ANY_THROW(vt.at(6));
    }

    TEST(xvector_variant, front)
    {
        auto v = build_test_ivector();
        variant_type vt(v);

        vt.front() = 4;
        int d = xtl::xget<const int&>(vt.front());
        EXPECT_EQ(d, 4);
    }

    TEST(xvector_variant, back)
    {
        auto v = build_test_ivector();
        variant_type vt(v);

        vt.back() = 4;
        int d = xtl::xget<const int&>(vt.back());
        EXPECT_EQ(d, 4);
    }

    TEST(xvector_variant, data)
    {
        auto v = build_test_ivector();
        variant_type vt(v);
        const variant_type& vt2 = vt;

        xtl::xget<int*>(vt.data())[0] = 4;
        int d = xtl::xget<const int*>(vt2.data())[0];
        EXPECT_EQ(d, 4);
    }

    TEST(xvector_variant, storage)
    {
        auto v = build_test_ivector();
        variant_type vt(v);
        const variant_type& vt2 = vt;

        xtl::xget<std::vector<int>>(vt.storage())[0] = 4;
        int d = xtl::xget<std::vector<int>>(vt2.storage())[0];
        EXPECT_EQ(d, 4);
    }

    TEST(xvector_variant, clear)
    {
        auto v = build_test_ivector();
        variant_type vt(v);

        vt.clear();
        EXPECT_TRUE(vt.empty());
    }

    TEST(xvector_variant, resize)
    {
        auto v = build_test_ivector();
        variant_type vt(v);

        vt.resize(8u);
        EXPECT_EQ(vt.size(), 8u);
    }

    TEST(xvector_variant, swap)
    {
        auto v1 = build_test_ivector();
        auto v2 = std::vector<int>();

        variant_type vt1(v1);
        variant_type vt2(v2);

        variant_type vt1_bu(v1);
        variant_type vt2_bu(v2);

        using std::swap;
        swap(vt1, vt2);

        EXPECT_EQ(vt1, vt2_bu);
        EXPECT_EQ(vt2, vt1_bu);
    }

    TEST(xvector_variant, push_back)
    {
        auto v = build_test_ivector();
        variant_type vt(v);

        int i = 2;
        vt.push_back(i);
        int j = 4;
        vt.push_back(std::move(j));
        int d0 = xtl::xget<int&>(vt[5]);
        int d1 = xtl::xget<int&>(vt[6]);
        EXPECT_EQ(d0, 2);
        EXPECT_EQ(d1, 4);
    }

    TEST(xvector_variant, pop_back)
    {
        auto v = build_test_ivector();
        variant_type vt(v);
        vt.pop_back();
        int d = xtl::xget<int&>(vt.back());
        EXPECT_EQ(d, 6);
    }

    TEST(xvector_variant, xget_vector)
    {
        auto v = build_test_ivector();
        variant_type vt(v);
        const variant_type& vt2(vt);

        auto& vl = xget_vector<int>(vt);
        vl[0] = 4;
        const auto& cvl = xget_vector<int>(vt2);
        EXPECT_EQ(cvl[0], 4);

        std::vector<int>&& rvl = xget_vector<int>(std::move(vt));
        EXPECT_EQ(rvl[0], 4);

        const std::vector<int>&& crvl = xget_vector<int>(std::move(vt2));
        EXPECT_EQ(crvl[0], 4);
    }

    /*****************************
     * xvector_variant_ref tests *
     *****************************/

    TEST(xvector_variant_ref, constructor)
    {
        auto v = build_test_ivector();
        
        // Copy
        variant_ref_type vrt(v);
        variant_ref_type vrt2(vrt);
        EXPECT_EQ(vrt, vrt2);

        // Move
        variant_ref_type vrt3(v);
        variant_ref_type vrt4(std::move(vrt3));
        EXPECT_EQ(vrt, vrt4);
    }

    TEST(xvector_variant_ref, assign)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);

        // Copy assign
        std::vector<int> v2 = {1, 2, 3};
        variant_ref_type vt2(v2);
        vt2 = vt;
        EXPECT_EQ(vt, vt2);
        EXPECT_EQ(v, v2);

        // Move assign
        std::vector<int> v3 = {0, 3, 4};
        variant_ref_type vt3(v3);
        vt3 = std::move(vt2);
        EXPECT_EQ(vt3, vt);
        EXPECT_EQ(v3, v);
    }

    TEST(xvector_variant_ref, empty)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);
        EXPECT_FALSE(v.empty());

        std::vector<int> v2;
        variant_ref_type vt2(v2);
        EXPECT_TRUE(vt2.empty());
    }

    TEST(xvector_variant_ref, size)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);

        EXPECT_EQ(vt.size(), v.size());
    }

    TEST(xvector_variant_ref, max_size)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);

        EXPECT_EQ(vt.max_size(), v.max_size());
    }

    TEST(xvector_variant_ref, capacity)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);
        EXPECT_EQ(vt.capacity(), v.capacity());
    }

    TEST(xvector_variant_ref, reserve)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);
        vt.reserve(20u);
        EXPECT_EQ(v.capacity(), 20u);
    }

    TEST(xvector_variant_ref, shrink_to_fit)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);
        vt.resize(2u);
        vt.shrink_to_fit();
        EXPECT_EQ(vt.capacity(), v.capacity());
    }

    TEST(xvector_variant_ref, access)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);

        vt[0] = 4;
        int d = xtl::xget<const int&>(vt[0]);
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v[0], 4);
    }

    TEST(xvector_variant_ref, at)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);

        vt.at(0) = 4;
        int d = xtl::xget<const int&>(vt.at(0));
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v[0], 4);

        EXPECT_ANY_THROW(vt.at(6));
    }

    TEST(xvector_variant_ref, front)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);

        vt.front() = 4;
        int d = xtl::xget<const int&>(vt.front());
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v.front(), 4);
    }

    TEST(xvector_variant_ref, back)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);

        vt.back() = 4;
        int d = xtl::xget<const int&>(vt.back());
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v.back(), 4);
    }

    TEST(xvector_variant_ref, data)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);
        const variant_ref_type& vt2 = vt;

        xtl::xget<int*>(vt.data())[0] = 4;
        int d = xtl::xget<const int*>(vt2.data())[0];
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v[0], 4);
    }

    TEST(xvector_variant_ref, storage)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);
        const variant_ref_type& vt2 = vt;

        xtl::xget<std::vector<int>&>(vt.storage())[0] = 4;
        int d = xtl::xget<const std::vector<int>&>(vt2.storage())[0];
        EXPECT_EQ(d, 4);
        EXPECT_EQ(v[0], 4);
    }

    TEST(xvector_variant_ref, clear)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);

        vt.clear();
        EXPECT_TRUE(vt.empty());
        EXPECT_TRUE(v.empty());
    }

    TEST(xvector_variant_ref, resize)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);

        vt.resize(8u);
        EXPECT_EQ(vt.size(), 8u);
        EXPECT_EQ(v.size(), 8u);
    }

    TEST(xvector_variant_ref, iterator)
    {
        // 1, 3, 4, 6, 7
        auto v = build_test_ivector();
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
        auto v = build_test_ivector();
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

    TEST(xvector_variant_ref, swap)
    {
        auto v1 = build_test_ivector();
        auto v2 = std::vector<int>();

        auto v1_bu = v1;
        auto v2_bu = v2;

        variant_ref_type vt1(v1);
        variant_ref_type vt2(v2);

        using std::swap;
        swap(vt1, vt2);
        
        EXPECT_EQ(v1, v2_bu);
        EXPECT_EQ(v2, v1_bu);
    }

    TEST(xvector_variant_ref, push_back)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);

        int i = 2;
        vt.push_back(i);
        int j = 4;
        vt.push_back(std::move(j));

        EXPECT_EQ(v[5], 2);
        EXPECT_EQ(v[6], 4);
    }

    TEST(xvector_variant_ref, pop_back)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);
        vt.pop_back();
        EXPECT_EQ(v.back(), 6);
    }

    TEST(xvector_variant_ref, xget_vector)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);
        const variant_ref_type& vt2(vt);

        auto& vl = xget_vector<int>(vt);
        vl[0] = 4;
        const auto& cvl = xget_vector<int>(vt2);
        EXPECT_EQ(cvl[0], 4);
        EXPECT_EQ(v[0], 4);

        auto& rvl = xget_vector<int>(std::move(vt));
        EXPECT_EQ(rvl[0], 4);

        const auto& crvl = xget_vector<int>(std::move(vt2));
        EXPECT_EQ(crvl[0], 4);
    }

    /******************************
     * xvector_variant_cref tests *
     ******************************/

    TEST(xvector_variant_cref, constructor)
    {
        auto v = build_test_ivector();
        
        // Copy
        variant_cref_type vrt(v);
        variant_cref_type vrt2(vrt);
        EXPECT_EQ(vrt, vrt2);

        // Move
        variant_cref_type vrt3(v);
        variant_cref_type vrt4(std::move(vrt3));
        EXPECT_EQ(vrt, vrt4);
    }

    TEST(xvector_variant_cref, empty)
    {
        auto v = build_test_ivector();
        variant_cref_type vt(v);
        EXPECT_FALSE(v.empty());

        std::vector<int> v2;
        variant_cref_type vt2(v2);
        EXPECT_TRUE(vt2.empty());
    }

    TEST(xvector_variant_cref, size)
    {
        auto v = build_test_ivector();
        variant_cref_type vt(v);

        EXPECT_EQ(vt.size(), v.size());
    }

    TEST(xvector_variant_cref, max_size)
    {
        auto v = build_test_ivector();
        variant_cref_type vt(v);

        EXPECT_EQ(vt.max_size(), v.max_size());
    }

    TEST(xvector_variant_cref, capacity)
    {
        auto v = build_test_ivector();
        variant_cref_type vt(v);
        EXPECT_EQ(vt.capacity(), v.capacity());
    }
    
    TEST(xvector_variant_cref, access)
    {
        auto v = build_test_ivector();
        variant_cref_type vt(v);

        int d = xtl::xget<const int&>(vt[0]);
        EXPECT_EQ(v[0], d);
    }

    TEST(xvector_variant_cref, at)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);

        int d = xtl::xget<const int&>(vt.at(0));
        EXPECT_EQ(v[0], d);

        EXPECT_ANY_THROW(vt.at(6));
    }

    TEST(xvector_variant_cref, front)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);

        int d = xtl::xget<const int&>(vt.front());
        EXPECT_EQ(v.front(), d);
    }

    TEST(xvector_variant_cref, back)
    {
        auto v = build_test_ivector();
        variant_ref_type vt(v);

        int d = xtl::xget<const int&>(vt.back());
        EXPECT_EQ(v.back(), d);
    }

    TEST(xvector_variant_cref, data)
    {
        auto v = build_test_ivector();
        variant_cref_type vt(v);

        int d = xtl::xget<const int*>(vt.data())[0];
        EXPECT_EQ(v[0], d);
    }

    TEST(xvector_variant_cref, storage)
    {
        auto v = build_test_ivector();
        variant_cref_type vt(v);

        int d = xtl::xget<const std::vector<int>&>(vt.storage())[0];
        EXPECT_EQ(v[0], d);
    }
    
    TEST(xvector_variant_cref, const_iterator)
    {
        // 1, 3, 4, 6, 7
        auto v = build_test_ivector();
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
    
    TEST(xvector_variant_cref, xget_vector)
    {
        auto v = build_test_ivector();
        variant_cref_type vt(v);
        const variant_cref_type& vt2(vt);

        auto& vl = xget_vector<int>(vt);
        EXPECT_EQ(vl, v);

        const auto& cvl = xget_vector<int>(vt2);
        EXPECT_EQ(cvl, v);

        auto& rvl = xget_vector<int>(std::move(vt));
        EXPECT_EQ(rvl, v);

        const auto& crvl = xget_vector<int>(std::move(vt2));
        EXPECT_EQ(crvl, v);
    }   
}

