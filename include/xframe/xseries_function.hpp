/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XSERIES_FUNCTION_HPP
#define XFRAME_XSERIES_FUNCTION_HPP

#include "xtensor/xfunction.hpp"
#include "xframe_utils.hpp"

namespace xf
{
    template <class F, class R, class... CT>
    class xseries_function : public xt::xexpression<xseries_function<F, R, CT...>>
    {
    public:

        using self_type = xseries_function<F, R, CT...>;
        using functor_type = std::remove_reference_t<F>;
        using label_list = xt::promote_shape_t<typename std::decay_t<CT>::label_list...>;
        using key_type = typename label_list::value_type;
        using mapped_type = R;
        using size_type = xt::detail::common_size_type_t<std::decay_t<CT>...>;
        using difference_type = xt::detail::common_difference_type_t<std::decay_t<CT>...>;

        using expression_tag = xframe_expression_tag;

        template <class Func>
        xseries_function(Func&& func, CT... e) noexcept;

        mapped_type operator[](const key_type& key) const;

    private:

        template <std::size_t... I>
        mapped_type access_impl(std::index_sequence<I...>, const key_type& key) const;

        std::tuple<CT...> m_e;
        functor_type m_f;
    };

    template <class F, class R, class... CT>
    template <class Func>
    inline xseries_function<F, R, CT...>::xseries_function(Func&& func, CT... e) noexcept
        : m_e(e...), m_f(std::forward<Func>(func))
    {
    }

    template <class F, class R, class... CT>
    inline auto xseries_function<F, R, CT...>::operator[](const key_type& key) const -> mapped_type
    {
        return access_impl(std::make_index_sequence<sizeof...(CT)>(), key);
    }

    template <class F, class R, class... CT>
    template <std::size_t... I>
    inline auto xseries_function<F, R, CT...>::access_impl(std::index_sequence<I...>, const key_type& key) const -> mapped_type
    {
        return m_f(std::get<I>(m_e).operator[](key)...);
    }
}

#endif
