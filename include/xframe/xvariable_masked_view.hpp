/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay, Wolf Vollprecht and   *
* Martin Renou                                                             *
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
#include "xcoordinate_system.hpp"

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

        using data_type = typename xvariable_expression_type::data_type;

        using coordinate_type = typename xvariable_expression_type::coordinate_type;
        using coordinate_closure_type = coordinate_type;
        using dimension_type = typename xvariable_expression_type::dimension_type;
        using dimension_closure_type = dimension_type;
        using key_type = typename coordinate_type::key_type;
        using size_type = typename coordinate_type::size_type;
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
        using optional_type = typename data_type::value_type;

        using xaxis_expression_type = typename base_type::xaxis_expression_type;
        using temporary_coordinate_type = xf::xcoordinate<typename base_type::key_type, typename base_type::size_type>;
        using temporary_data_type = xt::xmasked_view<xarray<typename optional_type::value_type>, xarray<bool>>;
        using temporary_type = xf::xvariable<temporary_coordinate_type, temporary_data_type>;
    };
}

namespace xf
{

    /************************************
     * xvariable_masked_view definition *
     ************************************/

    template <class CTV, class CTAX>
    class xvariable_masked_view : public xt::xview_semantic<xvariable_masked_view<CTV, CTAX>>,
                                  private xcoordinate_system<xvariable_masked_view<CTV, CTAX>>
    {
    public:

        using self_type = xvariable_masked_view<CTV, CTAX>;
        using coordinate_base = xcoordinate_system<xvariable_masked_view<CTV, CTAX>>;
        using semantic_base = xt::xview_semantic<self_type>;

        using inner_types = xt::xcontainer_inner_types<self_type>;
        using xvariable_expression_type = typename inner_types::xvariable_expression_type;
        using xaxis_expression_type = typename inner_types::xaxis_expression_type;
        using temporary_type = typename inner_types::temporary_type;

        using coordinate_type = typename coordinate_base::coordinate_type;
        using dimension_type = typename coordinate_base::dimension_type;

        using inner_data_type = typename xvariable_expression_type::data_type;
        using axis_func_impl = detail::axis_function_mask_impl<xaxis_expression_type&, const dimension_type&>;
        using mask_type = xt::xgenerator<axis_func_impl, typename axis_func_impl::value_type, typename xvariable_expression_type::shape_type>;
        using data_type = xt::xmasked_view<inner_data_type&, mask_type>;
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

        using coordinate_base::size;
        using coordinate_base::dimension;
        using coordinate_base::dimension_labels;
        using coordinate_base::coordinates;
        using coordinate_base::dimension_mapping;
        using coordinate_base::broadcast_coordinates;
        using coordinate_base::broadcast_dimensions;

        const shape_type& shape() const;

        data_type& data() noexcept;
        const data_type& data() const noexcept;

        template <class... Args>
        reference operator()(Args... args);

        template <class... Args>
        const_reference operator()(Args... args) const;

        template <std::size_t N = dynamic()>
        reference element(const index_type<N>& index);

        template <std::size_t N = dynamic()>
        const_reference element(const index_type<N>& index) const;

        template <std::size_t N = dynamic()>
        reference element(index_type<N>&& index);

        template <std::size_t N = dynamic()>
        const_reference element(index_type<N>&& index) const;

        template <class... Args>
        reference locate(Args&&... args);

        template <class... Args>
        const_reference locate(Args&&... args) const;

        template <std::size_t N = dynamic()>
        reference locate_element(const locator_sequence_type<N>& locator);

        template <std::size_t N = dynamic()>
        const_reference locate_element(const locator_sequence_type<N>& locator) const;

        template <std::size_t N = dynamic()>
        reference locate_element(locator_sequence_type<N>&& locator);

        template <std::size_t N = dynamic()>
        const_reference locate_element(locator_sequence_type<N>&& locator) const;

        template <std::size_t N = dynamic()>
        reference select(const selector_sequence_type<N>& selector);

        template <class Join = XFRAME_DEFAULT_JOIN, std::size_t N = dynamic()>
        const_reference select(const selector_sequence_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        reference select(selector_sequence_type<N>&& selector);

        template <class Join = XFRAME_DEFAULT_JOIN, std::size_t N = dynamic()>
        const_reference select(selector_sequence_type<N>&& selector) const;

        template <std::size_t N = dynamic()>
        reference iselect(const iselector_sequence_type<N>& selector);

        template <std::size_t N = dynamic()>
        const_reference iselect(const iselector_sequence_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        reference iselect(iselector_sequence_type<N>&& selector);

        template <std::size_t N = dynamic()>
        const_reference iselect(iselector_sequence_type<N>&& selector) const;

    private:

        template <class... Args, std::size_t... I>
        reference locate_impl(std::index_sequence<I...>, Args&&... args);

        template <class... Args, std::size_t... I>
        const_reference locate_impl(std::index_sequence<I...>, Args&&... args) const;

        template <std::size_t N = dynamic()>
        reference locate_element_impl(const locator_sequence_type<N>& locator);

        template <std::size_t N = dynamic()>
        const_reference locate_element_impl(const locator_sequence_type<N>& locator) const;

        template <class Join, std::size_t N>
        const_reference select_join(const selector_sequence_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        reference select_impl(const selector_sequence_type<N>& selector);

        template <std::size_t N = dynamic()>
        const_reference select_impl(const selector_sequence_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        const_reference select_outer(const selector_sequence_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        reference iselect_impl(const iselector_sequence_type<N>& selector);

        template <std::size_t N = dynamic()>
        const_reference iselect_impl(const iselector_sequence_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        iselector_sequence_type<N> to_iselector(const selector_sequence_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        iselector_sequence_type<N> to_iselector(const locator_sequence_type<N>& locator) const;

        template <class... Args, std::size_t... I>
        iselector_sequence_type<sizeof...(Args)> labels_to_iselector(std::index_sequence<I...>, Args&&... args) const;

        void assign_temporary_impl(temporary_type&& tmp);

        CTV m_expr;
        CTAX m_axis_expr;
        data_type m_data;

        friend class xt::xview_semantic<xvariable_masked_view<CTV, CTAX>>;
    };

    /****************************************
     * xvariable_masked_view implementation *
     ****************************************/

    template <class CTV, class CTAX>
    template <class V, class AX>
    inline xvariable_masked_view<CTV, CTAX>::xvariable_masked_view(V&& variable_expr, AX&& axis_expr)
        : coordinate_base(variable_expr.coordinates(), variable_expr.dimension_mapping()),
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
    inline auto xvariable_masked_view<CTV, CTAX>::shape() const -> const shape_type&
    {
        return m_expr.shape();
    }

    template <class CTV, class CTAX>
    inline auto xvariable_masked_view<CTV, CTAX>::data() noexcept -> data_type&
    {
        return m_data;
    }

    template <class CTV, class CTAX>
    inline auto xvariable_masked_view<CTV, CTAX>::data() const noexcept -> const data_type&
    {
        return m_data;
    }

    template <class CTV, class CTAX>
    template <class... Args>
    inline auto xvariable_masked_view<CTV, CTAX>::operator()(Args... args) -> reference
    {
        return m_data(args...);
    }

    template <class CTV, class CTAX>
    template <class... Args>
    inline auto xvariable_masked_view<CTV, CTAX>::operator()(Args... args) const -> const_reference
    {
        return m_data(args...);
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::element(const index_type<N>& index) -> reference
    {
        return m_data.element(index.cbegin(), index.cend());
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::element(const index_type<N>& index) const -> const_reference
    {
        return m_data.element(index.cbegin(), index.cend());
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::element(index_type<N>&& index) -> reference
    {
        return m_data.element(index.cbegin(), index.cend());
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::element(index_type<N>&& index) const -> const_reference
    {
        return m_data.element(index.cbegin(), index.cend());
    }

    template <class CTV, class CTAX>
    template <class... Args>
    inline auto xvariable_masked_view<CTV, CTAX>::locate(Args&&... args) -> reference
    {
        return locate_impl(std::make_index_sequence<sizeof...(Args)>(), std::forward<Args>(args)...);
    }

    template <class CTV, class CTAX>
    template <class... Args>
    inline auto xvariable_masked_view<CTV, CTAX>::locate(Args&&... args) const -> const_reference
    {
        return locate_impl(std::make_index_sequence<sizeof...(Args)>(), std::forward<Args>(args)...);
    }

    template <class CTV, class CTAX>
    template <class... Args, std::size_t... I>
    inline auto xvariable_masked_view<CTV, CTAX>::locate_impl(std::index_sequence<I...> idx_seq, Args&&... args) -> reference
    {
        return reference(iselect_impl<sizeof...(Args)>(labels_to_iselector(idx_seq, args...)));
    }

    template <class CTV, class CTAX>
    template <class... Args, std::size_t... I>
    inline auto xvariable_masked_view<CTV, CTAX>::locate_impl(std::index_sequence<I...> idx_seq, Args&&... args) const -> const_reference
    {
        return const_reference(iselect_impl<sizeof...(Args)>(labels_to_iselector(idx_seq, args...)));
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::locate_element(const locator_sequence_type<N>& locator) -> reference
    {
        return locate_element_impl<N>(locator);
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::locate_element(const locator_sequence_type<N>& locator) const -> const_reference
    {
        return locate_element_impl<N>(locator);
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::locate_element(locator_sequence_type<N>&& locator) -> reference
    {
        return locate_element_impl<N>(std::move(locator));
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::locate_element(locator_sequence_type<N>&& locator) const -> const_reference
    {
        return locate_element_impl<N>(std::move(locator));
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::locate_element_impl(const locator_sequence_type<N>& locator) -> reference
    {
        return reference(iselect_impl<N>(to_iselector<N>(locator)));
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::locate_element_impl(const locator_sequence_type<N>& locator) const -> const_reference
    {
        return const_reference(iselect_impl<N>(to_iselector<N>(locator)));
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::select(const selector_sequence_type<N>& selector) -> reference
    {
        return select_impl<N>(selector);
    }

    template <class CTV, class CTAX>
    template <class Join, std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::select(const selector_sequence_type<N>& selector) const -> const_reference
    {
        return select_join<Join, N>(selector);
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::select(selector_sequence_type<N>&& selector) -> reference
    {
        return select_impl<N>(std::move(selector));
    }

    template <class CTV, class CTAX>
    template <class Join, std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::select(selector_sequence_type<N>&& selector) const -> const_reference
    {
        return select_join<Join, N>(std::move(selector));
    }

    template <class CTV, class CTAX>
    template <class Join, std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::select_join(const selector_sequence_type<N>& selector) const -> const_reference
    {
        return xtl::mpl::static_if<Join::id() == join::inner::id()>([&](auto self)
        {
            return self(*this).template select_impl<N>(selector);
        }, /*else*/ [&](auto self)
        {
            return self(*this).template select_outer<N>(selector);
        });
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::select_impl(const selector_sequence_type<N>& selector) -> reference
    {
        auto iselector = to_iselector<N>(selector);

#ifdef _MSC_VER
        bool res = m_axis_expr.operator()<N>(iselector);
#else
        bool res = m_axis_expr.template operator()<N>(iselector);
#endif

        return reference(m_expr.select_impl(selector_type<N>(selector)), res);
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::select_impl(const selector_sequence_type<N>& selector) const -> const_reference
    {
        auto iselector = to_iselector<N>(selector);

#ifdef _MSC_VER
        bool res = m_axis_expr.operator()<N>(iselector);
#else
        bool res = m_axis_expr.template operator()<N>(iselector);
#endif

        return const_reference(m_expr.select_impl(selector_type<N>(selector)), res);
    }
#include <iostream>
    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::select_outer(const selector_sequence_type<N>& selector) const -> const_reference
    {
        // TODO get rid of the try-block
        try
        {
            auto iselector = xtl::make_sequence<iselector_sequence_type<N>>(selector.size());
            std::transform(selector.begin(), selector.end(), iselector.begin(), [this] (const auto& selector_pair) {
                return std::make_pair(selector_pair.first, m_expr.coordinates()[selector_pair]);
            });

#ifdef _MSC_VER
            bool res = m_axis_expr.operator()<N>(iselector);
#else
            bool res = m_axis_expr.template operator()<N>(iselector);
#endif

            return const_reference(m_expr.select_outer(selector_type<N>(selector)), res);
        }
        catch (const std::exception& /*exception*/)
        {
            return const_reference(m_expr.select_outer(selector_type<N>(selector)), false);
        }

    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::iselect(const iselector_sequence_type<N>& selector) -> reference
    {
        return iselect_impl<N>(selector);
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::iselect(const iselector_sequence_type<N>& selector) const -> const_reference
    {
        return iselect_impl<N>(selector);
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::iselect(iselector_sequence_type<N>&& selector) -> reference
    {
        return iselect_impl<N>(std::move(selector));
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::iselect(iselector_sequence_type<N>&& selector) const -> const_reference
    {
        return iselect_impl<N>(std::move(selector));
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::iselect_impl(const iselector_sequence_type<N>& selector) const -> const_reference
    {
#ifdef _MSC_VER
        bool res = m_axis_expr.operator()<N>(selector);
#else
        bool res = m_axis_expr.template operator()<N>(selector);
#endif

        return const_reference(m_expr.template iselect<N>(selector), res);
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::iselect_impl(const iselector_sequence_type<N>& selector) -> reference
    {
#ifdef _MSC_VER
        bool res = m_axis_expr.operator()<N>(selector);
#else
        bool res = m_axis_expr.template operator()<N>(selector);
#endif

        return reference(m_expr.template iselect<N>(selector), res);
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::to_iselector(const selector_sequence_type<N>& selector) const -> iselector_sequence_type<N>
    {
        auto iselector = xtl::make_sequence<iselector_sequence_type<N>>(selector.size());
        std::transform(selector.begin(), selector.end(), iselector.begin(), [this] (const auto& selector_pair) {
            return std::make_pair(selector_pair.first, m_expr.coordinates()[selector_pair]);
        });

        return iselector;
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::to_iselector(const locator_sequence_type<N>& locator) const -> iselector_sequence_type<N>
    {
        auto iselector = xtl::make_sequence<iselector_sequence_type<N>>(locator.size());
        for (std::size_t idx = 0; idx < locator.size(); ++idx)
        {
            iselector[idx] = std::make_pair(
                m_expr.dimension_mapping().label(idx),
                m_expr.coordinates()[std::make_pair(m_expr.dimension_mapping().label(idx), locator[idx])]
            );
        }

        return iselector;
    }

    template <class CTV, class CTAX>
    template <class... Args, std::size_t... I>
    inline auto xvariable_masked_view<CTV, CTAX>::labels_to_iselector(std::index_sequence<I...>, Args&&... args) const -> iselector_sequence_type<sizeof...(Args)>
    {
        return {
            std::make_pair(
                m_expr.dimension_mapping().label(I),
                m_expr.coordinates()[std::make_pair(m_expr.dimension_mapping().label(I), args)]
            )...
        };
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
