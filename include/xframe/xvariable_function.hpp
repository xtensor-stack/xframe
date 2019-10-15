/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
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
#include "xvariable_meta.hpp"
#include "xvariable_scalar.hpp"

namespace xf
{

    /**********************
     * xvariable_function *
     **********************/

    namespace detail
    {
        template <class CT>
        struct xvariable_data_closure
        {
            using data_type = decltype(std::declval<xvariable_closure_t<CT>>().data());
            using type = xt::const_xclosure_t<data_type>;
        };

        template <class CT>
        using xvariable_data_closure_t = typename xvariable_data_closure<CT>::type;
    }

    template <class CCT, class ECT>
    class xvariable_container;

    template <class F, class R, class... CT>
    class xvariable_function : public xt::xexpression<xvariable_function<F, R, CT...>>
    {
    public:

        using self_type = xvariable_function<F, R, CT...>;
        using functor_type = std::remove_reference_t<F>;
        using data_type = xt::xfunction<F, detail::xvariable_data_closure_t<CT>...>;
        using value_type = R;
        using reference = value_type;
        using const_reference = value_type;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using size_type = xt::common_size_type_t<std::decay_t<CT>...>;
        using difference_type = xt::common_difference_type_t<std::decay_t<CT>...>;

        using shape_type = typename data_type::shape_type;

        using coordinate_type = xcommon_coordinate_type_t<CT...>;
        using dimension_type = xcommon_dimension_type_t<CT...>;
        using dimension_list = typename dimension_type::label_list;

        using temporary_type = xvariable_container<coordinate_type, XFRAME_DEFAULT_DATA_CONTAINER(value_type)>;

        template <std::size_t N = dynamic()>
        using selector_type = xselector<coordinate_type, dimension_type, N>;
        template <std::size_t N = dynamic()>
        using selector_sequence_type = typename selector_type<N>::sequence_type;

        using expression_tag = xvariable_expression_tag;

        template <class Func, class U = std::enable_if<!std::is_base_of<Func, self_type>::value>>
        xvariable_function(Func&& f, CT... e) noexcept;

        template <class Join = XFRAME_DEFAULT_JOIN>
        size_type size() const noexcept;

        template <class Join = XFRAME_DEFAULT_JOIN>
        size_type dimension() const noexcept;

        template <class Join = XFRAME_DEFAULT_JOIN>
        const dimension_list& dimension_labels() const;

        template <class Join = XFRAME_DEFAULT_JOIN>
        const coordinate_type& coordinates() const;

        template <class Join = XFRAME_DEFAULT_JOIN>
        const dimension_type& dimension_mapping() const;

        template <class Join = XFRAME_DEFAULT_JOIN>
        xtrivial_broadcast broadcast_coordinates(coordinate_type& coords) const;
        bool broadcast_dimensions(dimension_type& dims, bool trivial_bc = false) const;

        shape_type shape() const noexcept;
        data_type data() const noexcept;

        template <class... Args>
        const_reference operator()(Args... args) const;

        template <class Join = XFRAME_DEFAULT_JOIN, std::size_t N = std::numeric_limits<size_type>::max()>
        const_reference select(const selector_sequence_type<N>& selector) const;

        template <class Join = XFRAME_DEFAULT_JOIN, std::size_t N = dynamic()>
        const_reference select(selector_sequence_type<N>&& selector) const;

        const std::tuple<xvariable_closure_t<CT>...>& arguments() const { return m_e; }

    private:

        template <class Join>
        void compute_coordinates() const;

        template <std::size_t... I, class... Args>
        const_reference access_impl(std::index_sequence<I...>, Args... args) const;

        template <std::size_t... I>
        data_type data_impl(std::index_sequence<I...>) const noexcept;

        template <class Join, std::size_t... I, class S>
        const_reference select_impl(std::index_sequence<I...>, S&& selector) const;

        template <std::size_t...I>
        bool merge_dimension_mapping(std::index_sequence<I...>, dimension_type& dims) const;

        std::tuple<xvariable_closure_t<CT>...> m_e;
        functor_type m_f;
        mutable coordinate_type m_coordinate;
        mutable dimension_type m_dimension_mapping;
        mutable join::join_id m_join_id;
        mutable bool m_coordinate_computed;
        mutable xtrivial_broadcast m_trivial_broadcast;
    };

    template <class F, class R, class... CT>
    std::ostream& operator<<(std::ostream& out, const xvariable_function<F, R, CT...>& f);

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
          m_join_id(join::inner::id()),
          m_coordinate_computed(false)
    {
    }

    template <class F, class R, class... CT>
    template <class Join>
    inline auto xvariable_function<F, R, CT...>::size() const noexcept -> size_type
    {
        const coordinate_type& coords = coordinates<Join>();
        return std::accumulate(coords.begin(), coords.end(), size_type(1),
                [](size_type init, auto&& arg) { return init * arg.second.size(); });
    }

    template <class F, class R, class... CT>
    template <class Join>
    inline auto xvariable_function<F, R, CT...>::dimension() const noexcept -> size_type
    {
        const coordinate_type& coords = coordinates<Join>();
        return coords.size();
    }

    template <class F, class R, class... CT>
    template <class Join>
    inline auto xvariable_function<F, R, CT...>::dimension_labels() const-> const dimension_list&
    {
        return dimension_mapping<Join>().labels();
    }

    template <class F, class R, class... CT>
    template <class Join>
    inline auto xvariable_function<F, R, CT...>::coordinates() const -> const coordinate_type&
    {
        compute_coordinates<Join>();
        return m_coordinate;
    }

    template <class F, class R, class... CT>
    template <class Join>
    inline auto xvariable_function<F, R, CT...>::dimension_mapping() const -> const dimension_type&
    {
        compute_coordinates<Join>();
        return m_dimension_mapping;
    }

    template <class F, class R, class... CT>
    template <class Join>
    inline xtrivial_broadcast xvariable_function<F, R, CT...>::broadcast_coordinates(coordinate_type& coords) const
    {
        auto func = [&coords](xtrivial_broadcast trivial, const auto& arg) {
            return arg.template broadcast_coordinates<Join>(coords) && trivial;
        };
        return xt::accumulate(func, xtrivial_broadcast(true, true), m_e);
    }

    namespace detail
    {
        template <class T, std::size_t I, bool C>
        struct first_non_scalar_impl
        {
            using elem_type = std::tuple_element_t<I, T>;

            static const elem_type& get(const T& t) noexcept
            {
                return std::get<I>(t);
            }
        };

        template <class T, std::size_t I>
        struct first_non_scalar_impl<T, I, false>
            : first_non_scalar_impl<T, I + 1, !is_xvariable_scalar<std::tuple_element_t<I+1, T>>::value>
        {
        };

        template <class T>
        struct first_non_scalar
            : first_non_scalar_impl<T, 0, !is_xvariable_scalar<std::tuple_element_t<0, T>>::value>
        {
        };

        template <class T>
        inline auto get_first_non_scalar(const T& t) noexcept
        {
            return first_non_scalar<T>::get(t);
        }
    }

    template <class F, class R, class... CT>
    inline bool xvariable_function<F, R, CT...>::broadcast_dimensions(dimension_type& dims, bool trivial_bc) const
    {
        bool ret = true;
        if(trivial_bc)
        {
            dims = detail::get_first_non_scalar(m_e).dimension_mapping();
        }
        else
        {
            ret = merge_dimension_mapping(std::make_index_sequence<sizeof...(CT)>(), dims);
        }
        return ret;
    }

    template <class F, class R, class... CT>
    inline auto xvariable_function<F, R, CT...>::shape() const noexcept -> shape_type
    {
        return data().shape();
    }

    template <class F, class R, class... CT>
    inline auto xvariable_function<F, R, CT...>::data() const noexcept -> data_type
    {
        return data_impl(std::make_index_sequence<sizeof...(CT)>());
    }

    template <class F, class R, class... CT>
    template <class... Args>
    inline auto xvariable_function<F, R, CT...>::operator()(Args... args) const -> const_reference
    {
        return access_impl(std::make_index_sequence<sizeof...(CT)>(), static_cast<size_type>(args)...);
    }

    template <class F, class R, class... CT>
    template <class Join, std::size_t N>
    inline auto xvariable_function<F, R, CT...>::select(const selector_sequence_type<N>& selector) const -> const_reference
    {
        return select_impl<Join>(std::make_index_sequence<sizeof...(CT)>(), selector);
    }

    template <class F, class R, class... CT>
    template <class Join, std::size_t N>
    inline auto xvariable_function<F, R, CT...>::select(selector_sequence_type<N>&& selector) const -> const_reference
    {
        return select_impl<Join>(std::make_index_sequence<sizeof...(CT)>(), std::move(selector));
    }

    template <class F, class R, class... CT>
    template <class Join>
    inline void xvariable_function<F, R, CT...>::compute_coordinates() const
    {
        if(!m_coordinate_computed || m_join_id != Join::id())
        {
            m_coordinate.clear();
            m_trivial_broadcast = broadcast_coordinates<Join>(m_coordinate);
            broadcast_dimensions(m_dimension_mapping, m_trivial_broadcast.m_same_dimensions);
            m_coordinate_computed = true;
            m_join_id = Join::id();
        }
    }

    template <class F, class R, class... CT>
    template <std::size_t... I, class... Args>
    inline auto xvariable_function<F, R, CT...>::access_impl(std::index_sequence<I...>, Args... args) const -> const_reference
    {
        return m_f(std::get<I>(m_e)(args...)...);
    }

    template <class F, class R, class... CT>
    template <std::size_t... I>
    inline auto xvariable_function<F, R, CT...>::data_impl(std::index_sequence<I...>) const noexcept -> data_type
    {
        return data_type(m_f, std::get<I>(m_e).data()...);
    }

    template <class F, class R, class... CT>
    template <class Join, std::size_t... I, class S>
    inline auto xvariable_function<F, R, CT...>::select_impl(std::index_sequence<I...>, S&& selector) const -> const_reference
    {
        return m_f(std::get<I>(m_e).template select<Join>(selector)...);
    }

    template <class F, class R, class... CT>
    template <std::size_t...I>
    bool xvariable_function<F, R, CT...>::merge_dimension_mapping(std::index_sequence<I...>, dimension_type& dims) const
    {
        return xf::broadcast_dimensions(dims, std::get<I>(m_e).dimension_mapping()...);
    }

    template <class F, class R, class... CT>
    inline std::ostream& operator<<(std::ostream& out, const xvariable_function<F, R, CT...>& f)
    {
        using temporary_type = typename xvariable_function<F, R, CT...>::temporary_type;
        temporary_type tmp(f);
        return print_variable_expression(out, tmp);
    }
}

#endif
