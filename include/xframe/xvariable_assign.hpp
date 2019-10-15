/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVARIABLE_ASSIGN_HPP
#define XFRAME_XVARIABLE_ASSIGN_HPP

#include "xtensor/xassign.hpp"
#include "xcoordinate.hpp"
#include "xframe_expression.hpp"

namespace xt
{
    using xvariable_expression_tag = xf::xvariable_expression_tag;

    template <>
    class xexpression_assigner<xvariable_expression_tag>
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

    private:

        template <class E1, class E2>
        static xf::xtrivial_broadcast resize(xexpression<E1>& e1, const xexpression<E2>& e2);

        template <class E1, class E2>
        static void assign_optional_tensor(xexpression<E1>& e1, const xexpression<E2>& e2, bool trivial);

        template <class E1, class E2>
        static void assign_resized_xexpression(xexpression<E1>& e1, const xexpression<E2>& e2,
                                               xf::xtrivial_broadcast trivial);
    };

    /***************************************
     * xexpression_assigner implementation *
     ***************************************/

    namespace detail
    {
        template <class IT, class ST>
        bool increment_index(const ST& shape, IT& index)
        {
            using size_type = typename ST::size_type;
            size_type i = index.size();
            while(i != 0)
            {
                --i;
                if(index[i] != shape[i] - 1)
                {
                    ++index[i];
                    return false;
                }
                else
                {
                    index[i] = 0;
                }
            }
            return true;
        }
    }

    template <class E1, class E2>
    inline void xexpression_assigner<xvariable_expression_tag>::assign_data(xexpression<E1>& e1,
                                                                            const xexpression<E2>& e2,
                                                                            bool /*trivial*/)
    {
        const auto& dim_label = e1.derived_cast().dimension_mapping().labels();
        const auto& coords = e1.derived_cast().coordinates();
        using size_type = typename E1::size_type;
        std::vector<size_type> index(dim_label.size(), size_type(0));
        using selector_sequence_type = typename E1::template selector_sequence_type<>;
        selector_sequence_type selector(index.size());
        bool end = false;
        do
        {
            for(size_type i = 0; i < index.size(); ++i)
            {
                selector[i] = std::make_pair(dim_label[i], coords[dim_label[i]].label(index[i]));
            }
            e1.derived_cast().select(selector) = e2.derived_cast().select(selector);
            end = detail::increment_index(e1.derived_cast().shape(), index);
        }
        while(!end);
    }

    template <class E1, class E2>
    inline void xexpression_assigner<xvariable_expression_tag>::assign_xexpression(xexpression<E1>& e1,
                                                                                   const xexpression<E2>& e2)
    {
        XFRAME_TRACE("ASSIGN EXPRESSION - BEGIN");
        xf::xtrivial_broadcast trivial = resize(e1, e2);
        assign_resized_xexpression(e1, e2, trivial);
        XFRAME_TRACE("ASSIGN EXPRESSION - END" << std::endl);
    }

    template <class E1, class E2>
    inline void xexpression_assigner<xvariable_expression_tag>::computed_assign(xexpression<E1>& e1,
                                                                                const xexpression<E2>& e2)
    {
        using coordinate_type = typename E1::coordinate_type;
        using dimension_type = typename E1::dimension_type;
        coordinate_type c;
        dimension_type d;
        xf::xtrivial_broadcast trivial = e2.derived_cast().broadcast_coordinates(c);
        bool dim_trivial = e2.derived_cast().broadcast_dimensions(d, trivial.m_same_dimensions);
        trivial.m_same_labels &= dim_trivial;
        if (d.size() > e1.derived_cast().dimension_mapping().size() || !trivial.m_same_labels)
        {
            typename E1::temporary_type tmp(std::move(c), std::move(d));
            assign_resized_xexpression(tmp, e2, trivial);
            e1.derived_cast().assign_temporary(std::move(tmp));
        }
        else
        {
            assign_resized_xexpression(e1, e2, trivial);
        }
    }

    template <class E1, class E2, class F>
    inline void xexpression_assigner<xvariable_expression_tag>::scalar_computed_assign(xexpression<E1>& e1,
                                                                                       const E2& e2,
                                                                                       F&& f)
    {
        E1& d = e1.derived_cast();
        std::transform(d.data().cbegin(), d.data().cend(), d.data().begin(),
            [e2, &f](const auto& v) { return f(v, e2); });
    }

    template <class E1, class E2>
    inline void xexpression_assigner<xvariable_expression_tag>::assert_compatible_shape(const xexpression<E1>& e1,
                                                                                        const xexpression<E2>& e2)
    {
        const auto& c1 = e1.derived_cast().coordinates();
        const auto& l2 = e2.derived_cast().dimension_labels();
        bool res = std::accumulate(l2.cbegin(), l2.cend(), true,
            [&c1](bool res, const auto& arg) { return res && c1.contains(arg); });
        if (!res)
        {
            throw std::runtime_error("Incompatible dimensions of expressions");
        }
    }

    template <class E1, class E2>
    inline xf::xtrivial_broadcast xexpression_assigner<xvariable_expression_tag>::resize(xexpression<E1>& e1,
                                                                                         const xexpression<E2>& e2)
    {
        using coordinate_type = typename E1::coordinate_type;
        using dimension_type = typename E1::dimension_type;
        coordinate_type c;
        dimension_type d;
        xf::xtrivial_broadcast res = e2.derived_cast().broadcast_coordinates(c);
        bool dim_trivial = e2.derived_cast().broadcast_dimensions(d, res.m_same_dimensions);
        res.m_same_labels &= dim_trivial;
        e1.derived_cast().resize(c, d);
        return res;
    }

    template <class E1, class E2>
    inline void xexpression_assigner<xvariable_expression_tag>::assign_optional_tensor(xexpression<E1>& e1,
                                                                                       const xexpression<E2>& e2,
                                                                                       bool trivial)
    {
        xexpression_assigner<xoptional_expression_tag>::assign_data(e1.derived_cast().data(),
                                                                    e2.derived_cast().data(),
                                                                    trivial);
    }

    template <class E1, class E2>
    inline void xexpression_assigner<xvariable_expression_tag>::assign_resized_xexpression(xexpression<E1>& e1,
                                                                                           const xexpression<E2>& e2,
                                                                                           xf::xtrivial_broadcast trivial)
    {
        if (trivial.m_same_labels)
        {
            assign_optional_tensor(e1, e2, trivial.m_same_dimensions);
        }
        else
        {
            assign_data(e1, e2, false);
        }
    }
}

#endif
