/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay, Wolf Vollprecht and   *
* Martin Renou                                                             *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_FUNCTION_HPP
#define XFRAME_XAXIS_FUNCTION_HPP

#include "xtensor/xoptional.hpp"

#include "xframe_expression.hpp"
#include "xframe_utils.hpp"
#include "xaxis_meta.hpp"
#include "xaxis_expression_leaf.hpp"

namespace xf
{
    /******************
     * xaxis_function *
     ******************/

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
        using size_type = xt::detail::common_size_type_t<std::decay_t<xaxis_expression_closure_t<CT>>...>;

        template <std::size_t N = dynamic()>
        using selector_sequence_type = detail::xselector_sequence_t<std::pair<name_type, size_type>, N>;

        using expression_tag = xaxis_expression_tag;

        template <class Func, class U = std::enable_if<!std::is_base_of<Func, self_type>::value>>
        xaxis_function(Func&& f, CT... e) noexcept;

        template <std::size_t N = std::numeric_limits<size_type>::max()>
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

    template <class F, class R, class... CT>
    template <class Func, class>
    inline xaxis_function<F, R, CT...>::xaxis_function(Func&& f, CT... e) noexcept
        : m_e(detail::get_axis_closure(std::forward<CT>(e))...),
          m_f(std::forward<Func>(f))
    {
    }

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
        return m_f(std::get<I>(m_e)(selector)...);
    }
}

#endif
