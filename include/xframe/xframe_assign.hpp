/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_ASSIGN_HPP
#define XFRAME_ASSIGN_HPP

#include "xtensor/xassign.hpp"
#include "xframe_utils.hpp"

namespace xt
{
    using xframe_expression_tag = xf::xframe_expression_tag;

    template <>
    class xexpression_assigner<xframe_expression_tag>
    {
    public:

        template <class E1, class E2>
        static void assign_data(xexpression<E1>& e1, const xexpression<E2>& e2, bool trivial);

        template <class E1, class E2>
        static void assign_xexpression(xexpression<E1>& e1, const xexpression<E2>& e2);

        template <class E1, class E2>
        static void computed_assign(xexpression<E1>& e1, const xexpression<E2>& e2);

        template <class E1, class E2, class F>
        static void scalar_computed_assign(xexpression<E1>& e1, const E2& e2, F&& f);

        template <class E1, class E2>
        static void assert_compatible_shape(const xexpression<E1>& e1, const xexpression<E2>& e2);
    };

    /***************************************
     * xexpression_assigner implementation *
     ***************************************/

    template <class E1, class E2>
    inline void xexpression_assigner<xframe_expression_tag>::assign_data(xexpression<E1>& e1,
                                                                         const xexpression<E2>& e2,
                                                                         bool trivial)
    {
        E1& de1 = e1.derived_cast();
        const E2& de2 = e2.derived_cast();

        using label_list = typename E1::label_list;
        const label_list& ll = de1.labels();

        using expression_type = typename E1::expression_type;
        expression_type& et = de1.values();

        auto lit = ll.cbegin();
        auto etit = et.storage_begin();
        for(; lit != ll.cend(); ++lit, ++etit)
        {
            *etit = de2[*lit];
        }
    }

    template <class E1, class E2>
    inline void xexpression_assigner<xframe_expression_tag>::assign_xexpression(xexpression<E1>& e1,
                                                                                const xexpression<E2>& e2)
    {
        // TODO: check trivial assign
        assign_data(e1, e2, true);
    }

    template <class E1, class E2>
    inline void xexpression_assigner<xframe_expression_tag>::computed_assign(xexpression<E1>& e1,
                                                                             const xexpression<E2>& e2)
    {
        throw std::runtime_error("computed_assign not implemented");
    }

    template <class E1, class E2, class F>
    inline void xexpression_assigner<xframe_expression_tag>::scalar_computed_assign(xexpression<E1>& e1,
                                                                                    const E2& e2,
                                                                                    F&& f)
    {
        throw std::runtime_error("scalar_computed_assign not implemented");
    }

    template <class E1, class E2>
    inline void xexpression_assigner<xframe_expression_tag>::assert_compatible_shape(const xexpression<E1>& e1,
                                                                                     const xexpression<E2>& e2)
    {
        throw std::runtime_error("assert_compatible_shape not implemented");
    }
}

#endif
