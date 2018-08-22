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

#include "xframe_utils.hpp"
#include "xselecting.hpp"
#include "xframe_config.hpp"
#include "xcoordinate_system.hpp"
#include "xtensor/xmasked_view.hpp"

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
        using xaxis_expression_type = typename base_type::xaxis_expression_type;
    };
}

namespace xf
{

    /************************************
     * xvariable_masked_view definition *
     ************************************/

    template <class CTV, class CTAX>
    class xvariable_masked_view : /*public xt::xview_semantic<xvariable_masked_view<CTV, CTAX>>,*/
                                  private xcoordinate_system<xvariable_masked_view<CTV, CTAX>>
    {
    public:

        using self_type = xvariable_masked_view<CTV, CTAX>;
        using coordinate_base = xcoordinate_system<xvariable_masked_view<CTV, CTAX>>;
        // using semantic_base = xt::xview_semantic<self_type>;

        using inner_types = xt::xcontainer_inner_types<self_type>;
        using xvariable_expression_type = typename inner_types::xvariable_expression_type;
        using xaxis_expression_type = typename inner_types::xaxis_expression_type;

        using coordinate_type = typename coordinate_base::coordinate_type;
        using dimension_type = typename coordinate_base::dimension_type;

        using inner_data_type = typename xvariable_expression_type::data_type;
        using axis_func_impl = detail::axis_function_mask_impl<xaxis_expression_type&, const dimension_type&>;
        using mask_type = xt::xgenerator<axis_func_impl, typename axis_func_impl::value_type, typename xvariable_expression_type::shape_type>;
        using data_type = xt::xmasked_view<inner_data_type&, mask_type>;

        static constexpr bool is_const = std::is_const<std::remove_reference_t<CTV>>::value;
        using value_type = typename xvariable_expression_type::value_type;
        using inner_value_type = typename value_type::value_type;
        using reference = std::conditional_t<is_const,
                                             typename xvariable_expression_type::const_reference,
                                             typename xvariable_expression_type::reference>;
        using const_reference = typename xvariable_expression_type::const_reference;
        using pointer = std::conditional_t<is_const,
                                           typename xvariable_expression_type::const_pointer,
                                           typename xvariable_expression_type::pointer>;
        using const_pointer = typename xvariable_expression_type::const_pointer;
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

        const_reference missing() const;
        reference missing();

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

        template <class Join = DEFAULT_JOIN, std::size_t N = dynamic()>
        const_reference select(const selector_sequence_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        reference select(selector_sequence_type<N>&& selector);

        template <class Join = DEFAULT_JOIN, std::size_t N = dynamic()>
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

        template <std::size_t N = dynamic(), class T>
        T select_impl(T&& missing_val, const selector_sequence_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        const_reference select_outer(const selector_sequence_type<N>& selector) const;

        template <std::size_t N = dynamic(), class T>
        T iselect_impl(T&& missing_val, const iselector_sequence_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        iselector_sequence_type<N> to_iselector(const selector_sequence_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        iselector_sequence_type<N> to_iselector(const locator_sequence_type<N>& locator) const;

        template <class... Args, std::size_t... I>
        iselector_sequence_type<sizeof...(Args)> labels_to_iselector(std::index_sequence<I...>, Args&&... args) const;

        CTV m_expr;
        CTAX m_axis_expr;
        data_type m_data;
        thread_local static value_type m_missing_ref;

        // friend class xt::xview_semantic<xvariable_masked_view<CTV, CTAX>>;
    };

    /****************************************
     * xvariable_masked_view implementation *
     ****************************************/

    template <class CTV, class CTAX>
    thread_local typename xvariable_masked_view<CTV, CTAX>::value_type xvariable_masked_view<CTV, CTAX>::m_missing_ref = value_type(inner_value_type(0), false);

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
    inline auto xvariable_masked_view<CTV, CTAX>::missing() const -> const_reference
    {
        return detail::static_missing<const_reference>();
    }

    template <class CTV, class CTAX>
    inline auto xvariable_masked_view<CTV, CTAX>::missing() -> reference
    {
        m_missing_ref.has_value() = false;
        return reference(m_missing_ref.value(), m_missing_ref.has_value());
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
        return iselect_impl<sizeof...(Args)>(missing(), labels_to_iselector(idx_seq, args...));
    }

    template <class CTV, class CTAX>
    template <class... Args, std::size_t... I>
    inline auto xvariable_masked_view<CTV, CTAX>::locate_impl(std::index_sequence<I...> idx_seq, Args&&... args) const -> const_reference
    {
        return iselect_impl<sizeof...(Args)>(missing(), labels_to_iselector(idx_seq, args...));
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
        return iselect_impl<N>(missing(), to_iselector<N>(locator));
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::locate_element_impl(const locator_sequence_type<N>& locator) const -> const_reference
    {
        return iselect_impl<N>(missing(), to_iselector<N>(locator));
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::select(const selector_sequence_type<N>& selector) -> reference
    {
        return select_impl<N>(missing(), selector);
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
        return select_impl<N>(missing(), std::move(selector));
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
            return self(*this).template select_impl<N>(self(*this).missing(), selector);
        }, /*else*/ [&](auto self)
        {
            return self(*this).template select_outer<N>(selector);
        });
    }

    template <class CTV, class CTAX>
    template <std::size_t N, class T>
    inline auto xvariable_masked_view<CTV, CTAX>::select_impl(T&& missing_val, const selector_sequence_type<N>& selector) const -> T
    {
        auto iselector = to_iselector<N>(selector);

#ifdef _MSC_VER
        bool res = m_axis_expr.operator()<N>(iselector);
#else
        bool res = m_axis_expr.template operator()<N>(iselector);
#endif
        if (!res)
        {
            return missing_val;
        }

        return m_expr.select_impl(selector_type<N>(selector));
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::select_outer(const selector_sequence_type<N>& selector) const -> const_reference
    {
        auto val = m_expr.select_outer(selector_type<N>(selector));

        if (val != missing())
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
            if (!res)
            {
                return missing();
            }
        }

        return val;
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
        return iselect_impl<N>(missing(), selector);
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::iselect(iselector_sequence_type<N>&& selector) -> reference
    {
        return iselect_impl<N>(missing(), std::move(selector));
    }

    template <class CTV, class CTAX>
    template <std::size_t N>
    inline auto xvariable_masked_view<CTV, CTAX>::iselect(iselector_sequence_type<N>&& selector) const -> const_reference
    {
        return iselect_impl<N>(missing(), std::move(selector));
    }

    template <class CTV, class CTAX>
    template <std::size_t N, class T>
    inline auto xvariable_masked_view<CTV, CTAX>::iselect_impl(T&& missing_val, const iselector_sequence_type<N>& selector) const -> T
    {
#ifdef _MSC_VER
        bool res = m_axis_expr.operator()<N>(selector);
#else
        bool res = m_axis_expr.template operator()<N>(selector);
#endif
        if (!res)
        {
            return missing_val;
        }

        return m_expr.template iselect<N>(selector);
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
