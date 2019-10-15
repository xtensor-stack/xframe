/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XWHERE_VIEW_HPP
#define XFRAME_XWHERE_VIEW_HPP

#include "xtensor/xgenerator.hpp"
#include "xtensor/xmasked_view.hpp"

#include "xframe_utils.hpp"
#include "xselecting.hpp"
#include "xframe_config.hpp"
#include "xvariable_base.hpp"

namespace xf
{

    /**************************
     * xvariable_masked_view  *
     **************************/

    template <class CTV, class CTAX>
    class xvariable_masked_view;

    template <class CTV, class CTAX>
    struct xvariable_inner_types<xvariable_masked_view<CTV, CTAX>>
    {
        using xvariable_expression_type = std::remove_reference_t<CTV>;
        using xaxis_expression_type = std::remove_reference_t<CTAX>;

        using coordinate_type = typename xvariable_expression_type::coordinate_type;
        using coordinate_closure_type = coordinate_type;
        using dimension_type = typename xvariable_expression_type::dimension_type;
        using dimension_closure_type = dimension_type;

        using inner_data_type = typename xvariable_expression_type::data_type;

        using axis_func_impl = detail::axis_function_mask_impl<xaxis_expression_type&, const dimension_type&>;
        using mask_type = xt::xgenerator<axis_func_impl, typename axis_func_impl::value_type, typename xvariable_expression_type::shape_type>;
        using data_type = xt::xmasked_view<inner_data_type&, mask_type>;
        using data_closure_type = data_type;

        using key_type = typename coordinate_type::key_type;
        using size_type = typename coordinate_type::size_type;
        using label_list = typename coordinate_type::label_list;
    };

}

namespace xt
{
    template <class CTV, class CTAX>
    struct xcontainer_inner_types<xf::xvariable_masked_view<CTV, CTAX>>
        : xf::xvariable_inner_types<xf::xvariable_masked_view<CTV, CTAX>>
    {
        using base_type = xf::xvariable_inner_types<xf::xvariable_masked_view<CTV, CTAX>>;

        using xvariable_expression_type = typename base_type::xvariable_expression_type;

        using data_type = typename base_type::data_type;
        using inner_data_type = typename base_type::inner_data_type;

        using xaxis_expression_type = typename base_type::xaxis_expression_type;
        using temporary_coordinate_type = xf::xcoordinate<typename base_type::key_type,
                                                          typename base_type::label_list,
                                                          typename base_type::size_type>;
        using temporary_data_type = typename xvariable_expression_type::temporary_type::data_type;
        using temporary_type = xf::xvariable_container<temporary_coordinate_type, temporary_data_type>;
    };
}

namespace xf
{

    /************************************
     * xvariable_masked_view definition *
     ************************************/

    /**
     * @class xvariable_masked_view
     * @brief View on a variable which will apply a mask on the variable, given
     * an expression on the axes.
     *
     * The xvariable_masked_view class is used for applying a mask on a variable,
     * avoiding assignment to masked values when assigning a scalar or an other
     * variable to the view. The mask is created given an expression on the axes.
     *
     * @tparam CTV the closure type on the underlying variable.
     * @tparam CTAX the closure type on the axes function.
     */
    template <class CTV, class CTAX>
    class xvariable_masked_view : public xt::xview_semantic<xvariable_masked_view<CTV, CTAX>>,
                                  private xvariable_base<xvariable_masked_view<CTV, CTAX>>
    {
    public:

        using self_type = xvariable_masked_view<CTV, CTAX>;
        using base_type = xvariable_base<self_type>;
        using semantic_base = xt::xview_semantic<self_type>;

        using inner_types = xt::xcontainer_inner_types<self_type>;
        using xvariable_expression_type = typename inner_types::xvariable_expression_type;
        using xaxis_expression_type = typename inner_types::xaxis_expression_type;
        using temporary_type = typename inner_types::temporary_type;

        using coordinate_type = typename inner_types::coordinate_type;
        using dimension_type = typename inner_types::dimension_type;

        using data_type = typename inner_types::data_type;
        using value_type = typename data_type::value_type;
        using reference = typename data_type::reference;
        using const_reference = typename data_type::const_reference;
        using pointer = typename data_type::pointer;
        using const_pointer = typename data_type::const_pointer;

        using size_type = typename xvariable_expression_type::size_type;
        using difference_type = typename xvariable_expression_type::difference_type;

        using dimension_list = typename dimension_type::label_list;
        using shape_type = typename data_type::shape_type;
        using squeeze_map = std::map<typename dimension_type::mapped_type, typename coordinate_type::index_type>;

        using expression_tag = xvariable_expression_tag;

        template <std::size_t N = dynamic()>
        using selector_traits = xselector_traits<coordinate_type, dimension_type, N>;
        template <std::size_t N = dynamic()>
        using index_type = typename selector_traits<N>::index_type;
        template <std::size_t N = dynamic()>
        using selector_type = typename selector_traits<N>::selector_type;
        template <std::size_t N = dynamic()>
        using selector_sequence_type = typename selector_traits<N>::selector_sequence_type;
        template <std::size_t N = dynamic()>
        using iselector_type = typename selector_traits<N>::iselector_type;
        template <std::size_t N = dynamic()>
        using iselector_sequence_type = typename selector_traits<N>::iselector_sequence_type;
        template <std::size_t N = dynamic()>
        using locator_type = typename selector_traits<N>::locator_type;
        template <std::size_t N = dynamic()>
        using locator_sequence_type = typename selector_traits<N>::locator_sequence_type;

        template <class V, class AX>
        xvariable_masked_view(V&& variable_expr, AX&& axis_expr);

        xvariable_masked_view(const xvariable_masked_view&) = default;
        xvariable_masked_view& operator=(const xvariable_masked_view&);

        template <class E>
        xvariable_masked_view& operator=(const xt::xexpression<E>& e);

        template <class E>
        xt::disable_xexpression<E, xvariable_masked_view>& operator=(const E& e);

        using base_type::missing;
        using base_type::shape;
        using base_type::size;
        using base_type::dimension;
        using base_type::data;
        using base_type::coordinates;
        using base_type::dimension_labels;
        using base_type::dimension_mapping;
        using base_type::broadcast_coordinates;
        using base_type::broadcast_dimensions;
        using base_type::operator();
        using base_type::element;
        using base_type::locate;
        using base_type::locate_element;
        using base_type::select;
        using base_type::iselect;

    private:

        void assign_temporary_impl(temporary_type&& tmp);

        data_type& data_impl() noexcept;
        const data_type& data_impl() const noexcept;

        CTV m_expr;
        CTAX m_axis_expr;
        data_type m_data;

        friend class xt::xview_semantic<self_type>;
        friend class xvariable_base<self_type>;
    };

    /****************************************
     * xvariable_masked_view implementation *
     ****************************************/

    /**
     * Builds an xvariable_masked_view.
     * @param variable_expr the underlying variable.
     * @param axis_expr the axis expression.
     */
    template <class CTV, class CTAX>
    template <class V, class AX>
    inline xvariable_masked_view<CTV, CTAX>::xvariable_masked_view(V&& variable_expr, AX&& axis_expr)
        : base_type(variable_expr.coordinates(), variable_expr.dimension_mapping()),
          m_expr(std::forward<V>(variable_expr)),
          m_axis_expr(std::forward<AX>(axis_expr)),
          m_data(m_expr.data(), axis_function_mask(
              m_axis_expr,
              m_expr.dimension_mapping(),
              m_expr.shape()
          ))
    {
    }

    template <class CTV, class CTAX>
    inline xvariable_masked_view<CTV, CTAX>& xvariable_masked_view<CTV, CTAX>::operator=(const xvariable_masked_view& rhs)
    {
        temporary_type tmp(rhs);
        return this->assign_temporary(std::move(tmp));
    }

    template <class CTV, class CTAX>
    template <class E>
    inline xvariable_masked_view<CTV, CTAX>& xvariable_masked_view<CTV, CTAX>::operator=(const xt::xexpression<E>& e)
    {
        using root_semantic = typename semantic_base::base_type;
        return root_semantic::operator=(e);
    }

    template <class CTV, class CTAX>
    template <class E>
    inline xt::disable_xexpression<E, xvariable_masked_view<CTV, CTAX>>& xvariable_masked_view<CTV, CTAX>::operator=(const E& e)
    {
        this->data().fill(e);
        return *this;
    }

    template <class CTV, class CTAX>
    inline void xvariable_masked_view<CTV, CTAX>::assign_temporary_impl(temporary_type&& tmp)
    {
        // TODO: improve this with iterators when they are available
        const temporary_type& tmp2 = tmp;
        const auto& dim_label = dimension_labels();
        const auto& coords = coordinates();
        std::vector<size_type> index(dim_label.size(), size_type(0));
        selector_sequence_type<> selector(index.size());
        bool end = false;
        do
        {
            for (size_type i = 0; i < index.size(); ++i)
            {
                selector[i] = std::make_pair(dim_label[i], coords[dim_label[i]].label(index[i]));
            }
            this->select(selector) = tmp2.select(selector);
            end = xt::detail::increment_index(tmp2.data().shape(), index);
        } while (!end);
    }

    template <class CTV, class CTAX>
    inline auto xvariable_masked_view<CTV, CTAX>::data_impl() noexcept -> data_type&
    {
        return m_data;
    }

    template <class CTV, class CTAX>
    inline auto xvariable_masked_view<CTV, CTAX>::data_impl() const noexcept -> const data_type&
    {
        return m_data;
    }

    /**
     * Apply a mask on a variable where the axis expression is false. e.g.
     * ```
     * // Will only assign 36 to values where the ordinate label is lower than 6
     * where(var, var.axis<int>("ordinate") < 6) = 36;
     *
     * // Will only add 2.4 to values where the abscissa is not equal to 'm' and the ordinate is not equal to 1
     * where(
     *      var,
     *      not_equal(var.axis<char>("abscissa"), 'm')) && not_equal(var.axis<int>("ordinate"), 1)
     * ) += 2.4;
     * ```
     * @param variable_expr the variable.
     * @param axis_expr the axis expression.
     * @return an xvariable_masked_view.
     */
    template <class EV, class EAX>
    inline auto where(EV&& variable_expr, EAX&& axis_expr)
    {
        return xvariable_masked_view<EV, EAX>(std::forward<EV>(variable_expr), std::forward<EAX>(axis_expr));
    }

    template <class EV, class EAX>
    inline std::ostream& operator<<(std::ostream& out, const xvariable_masked_view<EV, EAX>& v)
    {
        return print_variable_expression(out, v);
    }
}

#endif
