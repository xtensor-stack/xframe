/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_TEST_FIXTURE_HPP
#define XFRAME_TEST_FIXTURE_HPP

#include <cstddef>
#include "xtl/xbasic_fixed_string.hpp"
#include "xtensor/xmasked_view.hpp"
#include "xframe/xvariable.hpp"
#include "xframe/xnamed_axis.hpp"

namespace xf
{
    using saxis_type = xaxis<fstring, std::size_t>;
    using iaxis_type = xaxis<int, std::size_t>;
    using daxis_type = xaxis_default<int, std::size_t>;
    using dimension_type = xdimension<fstring, std::size_t>;
    using data_type = xt::xoptional_assembly<xt::xarray<double>, xt::xarray<bool>>;
    using int_data_type = xt::xoptional_assembly<xt::xarray<int>, xt::xarray<bool>>;
    using bool_data_type = xt::xoptional_assembly<xt::xarray<bool>, xt::xarray<bool>>;
    using coordinate_type = xcoordinate<fstring>;
    using variable_type = xvariable_container<coordinate_type, data_type>;
    using int_variable_type = xvariable_container<coordinate_type, int_data_type>;
    using bool_variable_type = xvariable_container<coordinate_type, bool_data_type>;

    /********
     * axes *
     ********/

    // { "a", "c", "d" }
    inline saxis_type make_test_saxis()
    {
        return saxis_type({"a", "c", "d"});
    }

    // { "a", "d", "e" }
    inline saxis_type make_test_saxis2()
    {
        return saxis_type({"a", "d", "e"});
    }

    // { "a", "b", "c", "d" }
    inline saxis_type make_test_saxis3()
    {
        return saxis_type({"a", "b", "c", "d"});
    }

    // { 1, 2, 4 }
    inline iaxis_type make_test_iaxis()
    {
        return iaxis_type({1, 2, 4});
    }

    // { 1, 4, 5 }
    inline iaxis_type make_test_iaxis2()
    {
        return iaxis_type({1, 4, 5});
    }

    // { 0, 1, 2 }
    inline daxis_type make_test_daxis()
    {
        return daxis_type(3);
    }

    /***************
     * coordinates *
     ***************/

    // abscissa: { "a", "c", "d" }
    // ordinate: { 1, 2, 4 }
    inline coordinate_type make_test_coordinate()
    {
        fstring s1 = "abscissa";
        fstring s2 = "ordinate";
        return coordinate<fstring>({
            {s1, make_test_saxis()},
            {s2, make_test_iaxis()}
        });
    }

    // abscissa: { "a", "d", "e" }
    // ordinate: { 1, 4, 5 }
    inline coordinate_type make_test_coordinate2()
    {
        const fstring s1 = "abscissa";
        return coordinate<fstring>(
            named_axis(s1, make_test_saxis2()),
            named_axis("ordinate", make_test_iaxis2())
        );
    }

    // abscissa: { "a", "d", "e" }
    // ordinate: { 1, 4, 5 }
    // altitude: { 1, 2, 4 }
    inline coordinate_type make_test_coordinate3()
    {
        fstring s1 = "abscissa";
        fstring s2 = "ordinate";
        fstring s3 = "altitude";
        return coordinate<fstring>({
            {s1, make_test_saxis2()},
            {s2, make_test_iaxis2()},
            {s3, make_test_iaxis()}
        });
    }

    // abscissa: { "a", "d", "e" }
    // ordinate: { 1, 4, 5 }
    // altitude: { 0, 1, 2 }
    inline coordinate_type make_test_coordinate4()
    {
        fstring s1 = "abscissa";
        fstring s2 = "ordinate";
        fstring s3 = "altitude";
        return coordinate<fstring>({
            {s1, make_test_saxis2()},
            {s2, make_test_iaxis2()},
            {s3, make_test_daxis()}
        });
    }

    // abscissa: { "a", "c", "d", "e" }
    // ordinate: { 1, 2, 4, 5 }
    // altitude: { 1, 2, 4 }
    inline coordinate_type make_merge_coordinate()
    {
        saxis_type sres = { "a", "c", "d", "e" };
        iaxis_type ires = { 1, 2, 4, 5 };
        return coordinate<fstring>({
            {fstring("abscissa"), std::move(sres)},
            {fstring("ordinate"), std::move(ires)},
            {fstring("altitude"), make_test_iaxis()},
        });
    }

    // abscissa: { "a", "d", "e" }
    // ordinate: { 1, 4, 5 }
    // altitude: { 0, 1, 2, 4 }
    inline coordinate_type make_merge_coordinate2()
    {
        iaxis_type ires = { 0, 1, 2, 4 };
        return coordinate<fstring>({
            {fstring("abscissa"), make_test_saxis2()},
            {fstring("ordinate"), make_test_iaxis2()},
            {fstring("altitude"), std::move(ires)},
        });
    }

    // abscissa: { "a", "d" }
    // ordinate: { 1, 4 }
    // altitude: { 1, 2, 4 }
    inline coordinate_type make_intersect_coordinate()
    {
        saxis_type sres = { "a", "d" };
        iaxis_type ires = { 1, 4 };
        return coordinate<fstring>({
            {fstring("abscissa"), std::move(sres)},
            {fstring("ordinate"), std::move(ires)},
            {fstring("altitude"), make_test_iaxis()}
        });
    }

    // abscissa: { "a", "d", "e" }
    // ordinate: { 1, 4, 5 }
    // altitude: { 1, 2 }
    inline coordinate_type make_intersect_coordinate2()
    {
        saxis_type sres = { "a", "d", "e" };
        iaxis_type ires = { 1, 4, 5 };
        iaxis_type ires2 = { 1, 2 };
        return coordinate<fstring>({
            {fstring("abscissa"), std::move(sres)},
            {fstring("ordinate"), std::move(ires)},
            {fstring("altitude"), std::move(ires2)}
        });
    }

    /*************
     * variables *
     *************/

    // data = {{ 1. ,  2., N/A },
    //         { N/A,  5.,  6. },
    //         { 7. ,  8.,  9. }}
    inline data_type make_test_data()
    {
        data_type d = {{ 1., 2., 3.},
                       { 4., 5., 6.},
                       { 7., 8., 9.}};
        d(0, 2).has_value() = false;
        d(1, 0).has_value() = false;
        return d;
    }

    inline data_type make_test_data2()
    {
        data_type d2 = {{{ 1., 2., 3.},
                         { 4., 5., 6.},
                         { 7., 8., 9.}},
                        {{ 11., 12., 13.},
                         { 14., 15., 16.},
                         { 17., 18., 19.}},
                        {{ 21., 22., 23.},
                         { 24., 25., 26.},
                         { 27., 28., 29.}}};
        d2(0, 0, 2).has_value() = false;
        d2(1, 1, 0).has_value() = false;
        return d2;
    }

    // data = {{ 1 ,  2, N/A },
    //         { N/A, 5,  6 },
    //         { 7 ,  8,  9 }}
    inline int_data_type make_test_int_data()
    {
        int_data_type d = {{ 1, 2, 3 },
                           { 4, 5, 6 },
                           { 7, 8, 9 }};
        d(0, 2).has_value() = false;
        d(1, 0).has_value() = false;
        return d;
    }

    // data = {{ true ,  true, N/A },
    //         { N/A, true,  true },
    //         { true ,  true,  true }}
    inline bool_data_type make_test_bool_data()
    {
        bool_data_type d = {{ true, true, true },
                            { true, true, true },
                            { true, true, true }};
        d(0, 2).has_value() = false;
        d(1, 0).has_value() = false;
        return d;
    }

    // masked_data = {{ 1. ,  2., N/A },
    //                { N/A, N/A, N/A },
    //                { 7. ,  8.,  9. }}
    inline auto make_masked_data(data_type& data)
    {
        auto axis1 = named_axis(fstring("abs"), axis({'x', 'y', 'z'}));

        auto func = axis_function_mask(
            not_equal(std::move(axis1), 'y'),
            dimension_type({"abs", "ord"}),
            std::array<int, 2>({3, 3})
        );

        return xt::masked_view(data, std::move(func));
    }

    // abscissa: { "a", "c", "d" }
    // ordinate: { 1, 2, 4 }
    // dims: {{ "abscissa", 0 }, { "ordinate", 1 }}
    // data = {{ 1. ,  2., N/A },
    //         { N/A,  5.,  6. },
    //         { 7. ,  8.,  9. }}
    inline variable_type make_test_variable()
    {
        return variable_type(make_test_data(), make_test_coordinate(), dimension_type({"abscissa", "ordinate"}));
    }

    // abscissa: { "a", "d", "e" }
    // ordinate: { 1, 4, 5 }
    // altitude: { 1, 2, 4 }
    // dims: {{ "abscissa", 0 }, { "ordinate", 1 }, {"altitude", 2}}
    // data = make_test_data2
    inline variable_type make_test_variable2()
    {
        return variable_type(make_test_data2(), make_test_coordinate3(), dimension_type({"abscissa", "ordinate", "altitude"}));
    }

    // abscissa: { "a", "d", "e" }
    // ordinate: { 1, 4, 5 }
    // dims: {{ "abscissa", 0 }, { "ordinate", 1 }}
    // data = {{ 1. ,  2., N/A },
    //         { N/A,  5.,  6. },
    //         { 7. ,  8.,  9. }}
    inline variable_type make_test_variable3()
    {
        return variable_type(make_test_data(), make_test_coordinate2(), dimension_type({"abscissa", "ordinate"}));
    }

    // altitude: { 1, 2, 4 }
    // abscissa: { "a", "d", "e" }
    // ordinate: { 1, 4, 5 }
    // dims: {{"altitude", 0}, { "abscissa", 1 }, { "ordinate", 2 }}
    // data = make_test_data2
    inline variable_type make_test_variable4()
    {
        return variable_type(make_test_data2(), make_test_coordinate3(), dimension_type({"altitude", "abscissa", "ordinate"}));
    }

    // abscissa: { "a", "c", "d" }
    // ordinate: { 1, 2, 4 }
    // dims: {{ "abscissa", 0 }, { "ordinate", 1 }}
    // data = {{ 1 ,  2, N/A },
    //         { N/A, 5,  6 },
    //         { 7 ,  8,  9 }}
    inline int_variable_type make_test_int_variable()
    {
        return int_variable_type(make_test_int_data(), make_test_coordinate(), dimension_type({ "abscissa", "ordinate" }));
    }

    inline bool_variable_type make_test_bool_variable()
    {
        return bool_variable_type(make_test_bool_data(), make_test_coordinate(), dimension_type({ "abscissa", "ordinate" }));
    }

    /*************
     * selectors *
     *************/

    using dict_type = typename variable_type::selector_sequence_type<>;
    using selector_list = std::vector<dict_type>;

    inline dict_type make_selector_aa()
    {
        return { {"abscissa", "a"}, {"ordinate", 1} };
    }

    inline selector_list make_selector_list_aa()
    {
        selector_list sl(9);
        sl[0] = { { "abscissa", "a" },{ "ordinate", 1 } };
        sl[1] = { { "abscissa", "a" },{ "ordinate", 2 } };
        sl[2] = { { "abscissa", "a" },{ "ordinate", 4 } };
        sl[3] = { { "abscissa", "c" },{ "ordinate", 1 } };
        sl[4] = { { "abscissa", "c" },{ "ordinate", 2 } };
        sl[5] = { { "abscissa", "c" },{ "ordinate", 4 } };
        sl[6] = { { "abscissa", "d" },{ "ordinate", 1 } };
        sl[7] = { { "abscissa", "d" },{ "ordinate", 2 } };
        sl[8] = { { "abscissa", "d" },{ "ordinate", 4 } };
        return sl;
    }

    inline selector_list make_selector_list_ab()
    {
        selector_list sl(12);
        sl[0] = { { "abscissa", "a" },{ "ordinate", 1 },{ "altitude", 1 } };
        sl[1] = { { "abscissa", "a" },{ "ordinate", 1 },{ "altitude", 2 } };
        sl[2] = { { "abscissa", "a" },{ "ordinate", 1 },{ "altitude", 4 } };
        sl[3] = { { "abscissa", "a" },{ "ordinate", 4 },{ "altitude", 1 } };
        sl[4] = { { "abscissa", "a" },{ "ordinate", 4 },{ "altitude", 2 } };
        sl[5] = { { "abscissa", "a" },{ "ordinate", 4 },{ "altitude", 4 } };
        sl[6] = { { "abscissa", "d" },{ "ordinate", 1 },{ "altitude", 1 } };
        sl[7] = { { "abscissa", "d" },{ "ordinate", 1 },{ "altitude", 2 } };
        sl[8] = { { "abscissa", "d" },{ "ordinate", 1 },{ "altitude", 4 } };
        sl[9] = { { "abscissa", "d" },{ "ordinate", 4 },{ "altitude", 1 } };
        sl[10] = { { "abscissa", "d" },{ "ordinate", 4 },{ "altitude", 2 } };
        sl[11] = { { "abscissa", "d" },{ "ordinate", 4 },{ "altitude", 4 } };
        return sl;
    }

    inline selector_list make_selector_list_cd()
    {
        selector_list sl(27);
        sl[0] = { { "altitude", 1 },{ "abscissa", "a" },{ "ordinate", 1 } };
        sl[1] = { { "altitude", 1 },{ "abscissa", "a" },{ "ordinate", 4 } };
        sl[2] = { { "altitude", 1 },{ "abscissa", "a" },{ "ordinate", 5 } };
        sl[3] = { { "altitude", 1 },{ "abscissa", "d" },{ "ordinate", 1 } };
        sl[4] = { { "altitude", 1 },{ "abscissa", "d" },{ "ordinate", 4 } };
        sl[5] = { { "altitude", 1 },{ "abscissa", "d" },{ "ordinate", 5 } };
        sl[6] = { { "altitude", 1 },{ "abscissa", "e" },{ "ordinate", 1 } };
        sl[7] = { { "altitude", 1 },{ "abscissa", "e" },{ "ordinate", 4 } };
        sl[8] = { { "altitude", 1 },{ "abscissa", "e" },{ "ordinate", 5 } };
        sl[9] = { { "altitude", 2 },{ "abscissa", "a" },{ "ordinate", 1 } };
        sl[10] = { { "altitude", 2 },{ "abscissa", "a" },{ "ordinate", 4 } };
        sl[11] = { { "altitude", 2 },{ "abscissa", "a" },{ "ordinate", 5 } };
        sl[12] = { { "altitude", 2 },{ "abscissa", "d" },{ "ordinate", 1 } };
        sl[13] = { { "altitude", 2 },{ "abscissa", "d" },{ "ordinate", 4 } };
        sl[14] = { { "altitude", 2 },{ "abscissa", "d" },{ "ordinate", 5 } };
        sl[15] = { { "altitude", 2 },{ "abscissa", "e" },{ "ordinate", 1 } };
        sl[16] = { { "altitude", 2 },{ "abscissa", "e" },{ "ordinate", 4 } };
        sl[17] = { { "altitude", 2 },{ "abscissa", "e" },{ "ordinate", 5 } };
        sl[18] = { { "altitude", 4 },{ "abscissa", "a" },{ "ordinate", 1 } };
        sl[19] = { { "altitude", 4 },{ "abscissa", "a" },{ "ordinate", 4 } };
        sl[20] = { { "altitude", 4 },{ "abscissa", "a" },{ "ordinate", 5 } };
        sl[21] = { { "altitude", 4 },{ "abscissa", "d" },{ "ordinate", 1 } };
        sl[22] = { { "altitude", 4 },{ "abscissa", "d" },{ "ordinate", 4 } };
        sl[23] = { { "altitude", 4 },{ "abscissa", "d" },{ "ordinate", 5 } };
        sl[24] = { { "altitude", 4 },{ "abscissa", "e" },{ "ordinate", 1 } };
        sl[25] = { { "altitude", 4 },{ "abscissa", "e" },{ "ordinate", 4 } };
        sl[26] = { { "altitude", 4 },{ "abscissa", "e" },{ "ordinate", 5 } };
        return sl;
    }

#define DEFINE_TEST_VARIABLES()                                           \
        variable_type a = make_test_variable();                           \
        variable_type b = make_test_variable2();                          \
        variable_type c = make_test_variable3();                          \
        variable_type d = make_test_variable4()

#define CHECK_EQUALITY(RES, A, B, SL, OP)                                 \
    for(std::size_t i = 0; i < sl.size(); ++i)                            \
    {                                                                     \
        EXPECT_EQ(RES.select(SL[i]), A.select(SL[i]) OP B.select(SL[i])); \
    }

#define CHECK_SCALAR2_EQUALITY(RES, A, B, SL, OP)                         \
    for(std::size_t i = 0; i < sl.size(); ++i)                            \
    {                                                                     \
        EXPECT_EQ(RES.select(SL[i]), A.select(SL[i]) OP B); \
    }

#define CHECK_SCALAR1_EQUALITY(RES, A, B, SL, OP)                         \
    for(std::size_t i = 0; i < sl.size(); ++i)                            \
    {                                                                     \
        EXPECT_EQ(RES.select(SL[i]), A OP B.select(SL[i])); \
    }
}

#endif
