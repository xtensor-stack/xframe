/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
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
        static std::pair<bool, bool> reshape(xexpression<E1>& e1, const xexpression<E2>& e2);

        template <class E1, class E2>
        static void assign_optional_tensor(xexpression<E1>& e1, const xexpression<E2>& e2, bool trivial);

        template <class E1, class E2>
        static void assign_reshaped_xexpression(xexpression<E1>& e1, const xexpression<E2>& e2,
                                                std::pair<bool, bool> trivial);
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
        using selector_map_type = typename E1::template selector_map_type<>;
        selector_map_type selector;
        size_type i = index.size();
        bool end = false;
        do
        {
            for(size_type i = 0; i < index.size(); ++i)
            {
                selector[dim_label[i]] = coords[dim_label[i]].label(index[i]);
            }
            e1.derived_cast().select(selector) = e2.derived_cast().select(selector);
            end = detail::increment_index(e1.derived_cast().data().shape(), index);
        }
        while(!end);
    }

    template <class E1, class E2>
    inline void xexpression_assigner<xvariable_expression_tag>::assign_xexpression(xexpression<E1>& e1,
                                                                                   const xexpression<E2>& e2)
    {
        std::pair<bool, bool> trivial = reshape(e1, e2);
        assign_reshaped_xexpression(e1, e2, trivial);
    }

    template <class E1, class E2>
    inline void xexpression_assigner<xvariable_expression_tag>::computed_assign(xexpression<E1>& e1,
                                                                                const xexpression<E2>& e2)
    {
        using coordinate_type = typename E1::coordinate_type;
        using dimension_type = typename E1::dimension_type;
        coordinate_type c;
        dimension_type d;
        std::pair<bool, bool> trivial = e2.derived_cast().broadcast_coordinates(c);
        trivial.first = e2.derived_cast().broadcast_dimensions(d, trivial.first);
        if (d.size() > e1.derived_cast().dimension_mapping().size() || !trivial.second)
        {
            // TODO: add constructor overloads in xvariable to avoid reshape
            typename E1::temporary_type tmp;
            tmp.reshape(std::move(c), std::move(d));
            assign_reshaped_xexpression(tmp, e2, trivial);
            e1.derived_cast().assign_temporary(std::move(tmp));
        }
        else
        {
            assign_reshaped_xexpression(e1, e2, trivial);
        }
    }

    template <class E1, class E2, class F>
    inline void xexpression_assigner<xvariable_expression_tag>::scalar_computed_assign(xexpression<E1>& e1,
                                                                                       const E2& e2,
                                                                                       F&& f)
    {
        throw std::runtime_error("scalar_computed_assign not implemented");
    }

    template <class E1, class E2>
    inline void xexpression_assigner<xvariable_expression_tag>::assert_compatible_shape(const xexpression<E1>& e1,
                                                                                        const xexpression<E2>& e2)
    {
        throw std::runtime_error("assert_compatible_shape not implemented");
    }

    template <class E1, class E2>
    inline std::pair<bool, bool> xexpression_assigner<xvariable_expression_tag>::reshape(xexpression<E1>& e1,
                                                                                         const xexpression<E2>& e2)
    {
        using coordinate_type = typename E1::coordinate_type;
        using dimension_type = typename E1::dimension_type;
        coordinate_type c;
        dimension_type d;
        std::pair<bool, bool> res = e2.derived_cast().broadcast_coordinates(c);
        res.first = e2.derived_cast().broadcast_dimensions(d, res.first);
        e1.derived_cast().reshape(c, d);
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
    inline void xexpression_assigner<xvariable_expression_tag>::assign_reshaped_xexpression(xexpression<E1>& e1,
                                                                                            const xexpression<E2>& e2,
                                                                                            std::pair<bool, bool> trivial)
    {
        if (trivial.second)
        {
            assign_optional_tensor(e1, e2, trivial.first);
        }
        else
        {
            assign_data(e1, e2, false);
        }
    }
}

#endif
