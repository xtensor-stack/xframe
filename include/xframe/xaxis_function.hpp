/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_FUNCTION_HPP
#define XFRAME_XAXIS_FUNCTION_HPP

#include "xtensor/xoptional.hpp"
#include "xtensor/xgenerator.hpp"

#include "xframe_expression.hpp"
#include "xframe_utils.hpp"
#include "xaxis_meta.hpp"
#include "xaxis_expression_leaf.hpp"

namespace xf
{
    /******************
     * xaxis_function *
     ******************/

    /**
     * @class xaxis_function
     * @brief An expression of xaxis
     *
     * The xaxis_function class is used for creating an expression on named axis, e.g.
     * `auto expr = not_equal(axis1, 2) && axis2 < 2`.
     *
     * @tparam F the function type.
     * @tparam R the result type.
     * @tparam CT the function argument types.
     * @sa xaxis_expression_leaf
     * @sa xnamed_axis
     */
    template <class F, class R, class... CT>
    class xaxis_function : public xt::xexpression<xaxis_function<F, R, CT...>>
    {
    public:

        using self_type = xaxis_function<F, R, CT...>;
        using functor_type = std::remove_reference_t<F>;

        using value_type = R;
        using reference = value_type;
        using const_reference = value_type;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using name_type = detail::common_name_type_t<std::decay_t<xaxis_expression_closure_t<CT>>...>;
        using size_type = xt::common_size_type_t<std::decay_t<xaxis_expression_closure_t<CT>>...>;

        template <std::size_t N = dynamic()>
        using selector_sequence_type = detail::xselector_sequence_t<std::pair<name_type, size_type>, N>;

        using expression_tag = xaxis_expression_tag;

        template <class Func, class U = std::enable_if<!std::is_base_of<Func, self_type>::value>>
        xaxis_function(Func&& f, CT... e) noexcept;

        template <std::size_t N = dynamic()>
        const_reference operator()(const selector_sequence_type<N>& selector) const;

    private:

        template <std::size_t N, std::size_t... I>
        const_reference evaluate(std::index_sequence<I...>, const selector_sequence_type<N>& selector) const;

        std::tuple<xaxis_expression_closure_t<CT>...> m_e;
        functor_type m_f;
    };

    /*********************************
     * xaxis_function implementation *
     *********************************/

    /**
     * Builds an axis function.
     * @param f the function to apply.
     * @param e the function arguments.
     */
    template <class F, class R, class... CT>
    template <class Func, class>
    inline xaxis_function<F, R, CT...>::xaxis_function(Func&& f, CT... e) noexcept
        : m_e(detail::get_axis_closure(std::forward<CT>(e))...),
          m_f(std::forward<Func>(f))
    {
    }

    /**
     * Returns an evaluation of the xaxis_function.
     * Example:
     * \code{.cpp}
     * auto axis1 = named_axis("abs", axis(16));
     * auto axis2 = named_axis("ord", axis({'a', 'c', 'i'}));
     *
     * auto func1 = axis1 < 5 && not_equal(axis2, 'i');
     *
     * // This will evaluate the xaxis_function for `axis1.label(10) == 9` and `axis2.label(1) == 'c'``
     * func1({{"abs", 10}, {"ord", 1}});
     * \endcode
     *
     * @param selector a selector_sequence_type for selecting the position
     *                 where you want to evaluate the function.
     * @return the evaluation of the xaxis_function
     */
    template <class F, class R, class... CT>
    template <std::size_t N>
    inline auto xaxis_function<F, R, CT...>::operator()(const selector_sequence_type<N>& selector) const -> const_reference
    {
        return evaluate<N>(std::make_index_sequence<sizeof...(CT)>(), selector);
    }

    template <class F, class R, class... CT>
    template <std::size_t N, std::size_t... I>
    inline auto xaxis_function<F, R, CT...>::evaluate(std::index_sequence<I...>, const selector_sequence_type<N>& selector) const -> const_reference
    {
#ifdef _MSC_VER
        return m_f(std::get<I>(m_e).operator()<N>(selector)...);
#else
        return m_f(std::get<I>(m_e).template operator()<N>(selector)...);
#endif
    }

    /**********************
     * axis_function_mask *
     **********************/

    namespace detail
    {
        template <class AF, class DM>
        class axis_function_mask_impl
        {
        public:

            using axis_function_type = std::remove_reference_t<AF>;

            using value_type = typename axis_function_type::value_type;
            using name_type = typename axis_function_type::name_type;
            using size_type = typename axis_function_type::size_type;

            template <std::size_t N = dynamic()>
            using selector_sequence_type = detail::xselector_sequence_t<std::pair<name_type, size_type>, N>;

            axis_function_mask_impl(AF&& axis_function, DM&& dim_mapping)
                : m_axis_function(std::forward<AF>(axis_function)),
                  m_dimension_mapping(std::forward<DM>(dim_mapping))
            {
            }

            template <class... Args>
            inline value_type operator()(Args... args) const
            {
                auto selector = make_selector(std::make_index_sequence<sizeof...(Args)>(), args...);
#ifdef _MSC_VER
                return m_axis_function.operator()<sizeof...(Args)>(selector);
#else
                return m_axis_function.template operator()<sizeof...(Args)>(selector);
#endif
            }

            template <class It>
            inline value_type element(It first, It last) const
            {
                // TODO avoid dynamic allocation
                auto selector = selector_sequence_type<dynamic()>();
                std::size_t i = 0;
                for (It it = first; it != last; ++it)
                {
                    selector.push_back(std::make_pair(m_dimension_mapping.label(i++), static_cast<size_type>(*it)));
                }
#ifdef _MSC_VER
                return m_axis_function.operator()<dynamic()>(selector);
#else
                return m_axis_function.template operator()<dynamic()>(selector);
#endif
            }

        private:

            AF m_axis_function;
            DM m_dimension_mapping;

            template <class... Args, std::size_t... I>
            inline selector_sequence_type<sizeof...(Args)> make_selector(std::index_sequence<I...>, Args&&... args) const
            {
                return {std::make_pair(m_dimension_mapping.label(I), static_cast<size_type>(args))...};
            }
        };
    }

    template <class AF, class DM, class S>
    inline auto axis_function_mask(AF&& axis_function, DM&& dim_mapping, const S& shape) noexcept
    {
        return xt::detail::make_xgenerator(
            detail::axis_function_mask_impl<AF, DM>(std::forward<AF>(axis_function), std::forward<DM>(dim_mapping)),
            shape
        );
    }
}

#endif
