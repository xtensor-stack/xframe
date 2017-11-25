/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVARIABLE_FUNCTION_HPP
#define XFRAME_XVARIABLE_FUNCTION_HPP

#include "xtensor/xoptional.hpp"

#include "xcoordinate.hpp"
#include "xselecting.hpp"

namespace xf
{

    /**********************
     * xvariable_function *
     **********************/

    template <class F, class R, class... CT>
    class xvariable_function : public xt::xexpression<xvariable_function<F, R, CT...>>
    {
    public:

        using self_type = xvariable_function<F, R, CT...>;
        using functor_type = std::remove_reference_t<F>;
        using value_type = R;
        using reference = value_type;
        using const_reference = value_type;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using size_type = xt::detail::common_size_type_t<std::decay_t<CT>...>;
        using difference_type = xt::detail::common_difference_type_t<std::decay_t<CT>...>;     

        using coordinate_type = std::common_type_t<typename std::decay_t<CT>::coordinate_type...>;
        using dimension_type = std::common_type_t<typename std::decay_t<CT>::dimension_type...>;
        using dimension_list = typename dimension_type::label_list;

        using expression_tag = xvariable_expression_tag;

        template <class Func, class U = std::enable_if<!std::is_base_of<Func, self_type>::value>>
        xvariable_function(Func&& f, CT... e) noexcept;

        template <class Policy = DEFAULT_BROADCAST_POLICY>
        size_type size() const noexcept;

        template <class Policy = DEFAULT_BROADCAST_POLICY>
        size_type dimension() const noexcept;

        template <class Policy = DEFAULT_BROADCAST_POLICY>
        const dimension_list& dimension_labels() const;

        template <class Policy = DEFAULT_BROADCAST_POLICY>
        const coordinate_type& coordinates() const;

        template <class Policy = DEFAULT_BROADCAST_POLICY>
        const dimension_type& dimension_mapping() const;

        template <class... Args>
        const_reference operator()(Args... args) const;
        
        template <class Policy = DEFAULT_BROADCAST_POLICY>
        std::pair<bool, bool> broadcast_coordinates(coordinate_type& coords) const;

        template <std::size_t N = dynamic()>
        using selector_type = xselector<coordinate_type, dimension_type, N>;
        template <std::size_t N = dynamic()>
        using selector_map_type = typename selector_type<N>::map_type;
        template <std::size_t N = dynamic()>
        using iselector_type = xiselector<coordinate_type, dimension_type, N>;
        template <std::size_t N = dynamic()>
        using iselector_map_type = typename iselector_type<N>::map_type;
        template <std::size_t N = dynamic()>
        using locator_type = xlocator<coordinate_type, dimension_type, N>;
        template <std::size_t N = dynamic()>
        using locator_map_type = typename locator_type<N>::map_type;

        template <std::size_t N = std::numeric_limits<size_type>::max()>
        const_reference select(const selector_map_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        const_reference select(selector_map_type<N>&& selector) const;

    private:

        template <class Policy>
        void compute_coordinates() const;

        template <std::size_t... I, class... Args>
        const_reference access_impl(std::index_sequence<I...>, Args... args) const;

        template <class Policy, std::size_t... I>
        std::pair<bool, bool> broadcast_coordinates_impl(std::index_sequence<I...>, coordinate_type& coords) const;

        template <std::size_t... I, class S>
        const_reference select_impl(std::index_sequence<I...>, S&& selector) const;

        template <std::size_t...I>
        bool merge_dimension_mapping(std::index_sequence<I...>, dimension_type& dims) const;

        std::tuple<CT...> m_e;
        functor_type m_f;
        mutable coordinate_type m_coordinate;
        mutable dimension_type m_dimension_mapping;
        mutable broadcast_policy::policy_id m_policy_id;
        mutable bool m_coordinate_computed;
    };

    /*************************************
     * xvariable_function implementation *
     *************************************/

    template <class F, class R, class... CT>
    template <class Func, class>
    inline xvariable_function<F, R, CT...>::xvariable_function(Func&& f, CT... e) noexcept
        : m_e(e...),
          m_f(std::forward<Func>(f)),
          m_coordinate(),
          m_dimension_mapping(),
          m_policy_id(broadcast_policy::merge_axes::id()),
          m_coordinate_computed(false)
    {
    }

    template <class F, class R, class... CT>
    template <class Policy>
    inline auto xvariable_function<F, R, CT...>::size() const noexcept -> size_type
    {
        const coordinate_type& coords = coordinates<Policy>();
        return std::accumulate(coords.begin(), coords.end(), size_type(1),
                [](size_type init, auto&& arg) { return init * arg.second.size(); });
    }

    template <class F, class R, class... CT>
    template <class Policy>
    inline auto xvariable_function<F, R, CT...>::dimension() const noexcept -> size_type
    {
        const coordinate_type& coords = coordinates<Policy>();
        return coords.size();
    }
    
    template <class F, class R, class... CT>
    template <class Policy>
    inline auto xvariable_function<F, R, CT...>::dimension_labels() const-> const dimension_list&
    {
        return dimension_mapping<Policy>().labels();
    }

    template <class F, class R, class... CT>
    template <class Policy>
    inline auto xvariable_function<F, R, CT...>::coordinates() const -> const coordinate_type&
    {
        compute_coordinates<Policy>();
        return m_coordinate;
    }

    template <class F, class R, class... CT>
    template <class Policy>
    inline auto xvariable_function<F, R, CT...>::dimension_mapping() const -> const dimension_type&
    {
        compute_coordinates<Policy>();
        return m_dimension_mapping;
    }
    
    template <class F, class R, class... CT>
    template <class... Args>
    inline auto xvariable_function<F, R, CT...>::operator()(Args... args) const -> const_reference
    {
        return access_impl(std::make_index_sequence<sizeof...(CT)>(), static_cast<size_type>(args)...);
    }

    template <class F, class R, class... CT>
    template <class Policy>
    inline std::pair<bool, bool> xvariable_function<F, R, CT...>::broadcast_coordinates(coordinate_type& coords) const
    {
        return broadcast_coordinates_impl<Policy>(std::make_index_sequence<sizeof...(CT)>(), coords);
    }

    template <class F, class R, class... CT>
    template <std::size_t N>
    inline auto xvariable_function<F, R, CT...>::select(const selector_map_type<N>& selector) const -> const_reference
    {
        return select_impl(std::make_index_sequence<sizeof...(CT)>(), selector);
    }

    template <class F, class R, class... CT>
    template <std::size_t N>
    inline auto xvariable_function<F, R, CT...>::select(selector_map_type<N>&& selector) const -> const_reference
    {
        return select_impl(std::make_index_sequence<sizeof...(CT)>(), std::move(selector));
    }

    template <class F, class R, class... CT>
    template <class Policy>
    inline void xvariable_function<F, R, CT...>::compute_coordinates() const
    {
        if(!m_coordinate_computed || m_policy_id != Policy::id())
        {
            m_coordinate.clear();
            auto res = broadcast_coordinates<Policy>(m_coordinate);
            if(res.first)
            {
                m_dimension_mapping = std::get<0>(m_e).dimension_mapping();
            }
            else
            {
                merge_dimension_mapping(std::make_index_sequence<sizeof...(CT)>(), m_dimension_mapping);
            }
            m_coordinate_computed = true;
            m_policy_id = Policy::id();
        }
    }

    template <class F, class R, class... CT>
    template <std::size_t... I, class... Args>
    inline auto xvariable_function<F, R, CT...>::access_impl(std::index_sequence<I...>, Args... args) const -> const_reference
    {
        return m_f(xt::detail::get_element(std::get<I>(m_e), args...)...);
    }
    
    template <class F, class R, class... CT>
    template <class Policy, std::size_t... I>
    inline std::pair<bool, bool>
    xvariable_function<F, R, CT...>::broadcast_coordinates_impl(std::index_sequence<I...>, coordinate_type& coords) const
    {
        return xf::broadcast_coordinates<Policy>(coords, std::get<I>(m_e).coordinates()...);
    }

    template <class F, class R, class... CT>
    template <std::size_t... I, class S>
    inline auto xvariable_function<F, R, CT...>::select_impl(std::index_sequence<I...>, S&& selector) const -> const_reference
    {
        return m_f(std::get<I>(m_e).select(selector)...);
    }

    template <class F, class R, class... CT>
    template <std::size_t...I>
    bool xvariable_function<F, R, CT...>::merge_dimension_mapping(std::index_sequence<I...>, dimension_type& dims) const
    {
        return xf::merge_axes(dims, std::get<I>(m_e).dimension_mapping()...);
    }
}

#endif
