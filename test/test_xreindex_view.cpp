/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"
#include "test_fixture.hpp"
#include "xframe/xreindex_view.hpp"

namespace xf
{
    using coordinate_map = typename coordinate_type::map_type;

    coordinate_map make_new_coordinate()
    {
        coordinate_map m;
        m["abscissa"] = make_test_saxis3();
        return m;
    }

    TEST(xreindex_view, builders)
    {
        auto var = make_test_variable();
        coordinate_map new_coord = make_new_coordinate();
        coordinate_type coordinate_res = {{"abscissa", make_test_saxis3()}, {"ordinate", make_test_iaxis()}};

        auto view = reindex(var, new_coord);
        EXPECT_TRUE(view.coordinates() == coordinate_res);
        EXPECT_EQ(view.dimension_mapping(), var.dimension_mapping());
    }

    TEST(xreindex_view, size)
    {
        auto var = make_test_variable();
        coordinate_map new_coord = make_new_coordinate();
        auto view = reindex(var, new_coord);

        EXPECT_EQ(12u, view.size());
    }
}
