/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_VARIANT_HPP
#define XFRAME_XAXIS_VARIANT_HPP

#include <functional>
#include "xtl/xclosure.hpp"
#include "xtl/xmeta_utils.hpp"
#include "xtl/xvariant.hpp"
#include "xaxis.hpp"
#include "xaxis_default.hpp"

namespace xf
{

    namespace detail
    {
        template <class V, class S, class... L>
        struct add_default_axis;

        template <class... A, class S>
        struct add_default_axis<xtl::variant<A...>, S>
        {
            using type = xtl::variant<A...>;
        };

        template <class... A, class S, class L1, class... L>
        struct add_default_axis<xtl::variant<A...>, S, L1, L...>
        {
            using variant_type = xtl::variant<A...>;
            using type = typename xtl::mpl::if_t<std::is_integral<L1>,
                add_default_axis<xtl::variant<A..., xaxis_default<L1, S>>, S, L...>,
                add_default_axis<xtl::variant<A...>, S, L...>>::type;
        };

        template <class V, class S, class... L>
        using add_default_axis_t = typename add_default_axis<V, S, L...>::type;

        template <class V>
        struct get_axis_variant_iterator;

        template <class... A>
        struct get_axis_variant_iterator<xtl::variant<A...>>
        {
            using type = xtl::variant<typename A::const_iterator...>;
        };

        template <class V>
        using get_axis_variant_iterator_t = typename get_axis_variant_iterator<V>::type;

        template <class S, class MT, class TL>
        struct xaxis_variant_traits;

        template <class S, class MT, template <class...> class TL, class... L>
        struct xaxis_variant_traits<S, MT, TL<L...>>
        {
            using tmp_storage_type = xtl::variant<xaxis<L, S, MT>...>;
            using storage_type = add_default_axis_t<tmp_storage_type, S, L...>;
            // Convenient for defining other types, but do not use it
            using label_list = std::vector<xtl::variant<L...>>;
            using key_type = xtl::variant<typename xaxis<L, S, MT>::key_type...>;
            using key_reference = xtl::variant<xtl::xclosure_wrapper<const typename xaxis<L, S, MT>::key_type&>...>;
            using mapped_type = S;
            using value_type = std::pair<key_type, mapped_type>;
            using reference = std::pair<key_reference, mapped_type&>;
            using const_reference = std::pair<key_reference, const mapped_type&>;
            using pointer = xtl::xclosure_pointer<reference>;
            using const_pointer = xtl::xclosure_pointer<const_reference>;
            using size_type = typename label_list::size_type;
            using difference_type = typename label_list::difference_type;
            using subiterator = get_axis_variant_iterator_t<storage_type>;
        };
    }

    template <class L, class T, class MT>
    class xaxis_variant_iterator;

    /*****************
     * xaxis_variant *
     *****************/

    template <class L, class T, class MT = hash_map_tag>
    class xaxis_variant
    {
    public:

        static_assert(std::is_integral<T>::value, "index_type must be an integral type");

        using self_type = xaxis_variant<L, T, MT>;
        using map_container_tag = MT;
        using traits_type = detail::xaxis_variant_traits<T, MT, L>;
        using storage_type = typename traits_type::storage_type;
        using key_type = typename traits_type::key_type;
        using key_reference = typename traits_type::key_reference;
        using mapped_type = T;
        using value_type = typename traits_type::value_type;
        using reference = typename traits_type::reference;
        using const_reference = typename traits_type::const_reference;
        using pointer = typename traits_type::pointer;
        using const_pointer = typename traits_type::const_pointer;
        using size_type = typename traits_type::size_type;
        using difference_type = typename traits_type::difference_type;
        using iterator = xaxis_variant_iterator<L, T, MT>;
        using const_iterator = iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using subiterator = typename traits_type::subiterator;

        template <class LB>
        using label_list = typename xaxis<LB, T, MT>::label_list;

        xaxis_variant() = default;
        template <class LB>
        xaxis_variant(const xaxis<LB, T, MT>& axis);
        template <class LB>
        xaxis_variant(xaxis<LB, T, MT>&& axis);

        template <class LB>
        const label_list<LB>& labels() const;
        key_type label(size_type i) const;

        bool empty() const;
        size_type size() const;

        bool contains(const key_type& key) const;
        mapped_type operator[](const key_type& key) const;

        template <class F>
        self_type filter(const F& f) const;

        template <class F>
        self_type filter(const F& f, size_type size) const;

        const_iterator find(const key_type& key) const;

        const_iterator begin() const;
        const_iterator end() const;

        const_iterator cbegin() const;
        const_iterator cend() const;

        const_reverse_iterator rbegin() const;
        const_reverse_iterator rend() const;

        const_reverse_iterator crbegin() const;
        const_reverse_iterator crend() const;

        template <class... Args>
        bool merge(const Args&... axes);

        template <class... Args>
        bool intersect(const Args&... axes);

        bool operator==(const self_type& rhs) const;
        bool operator!=(const self_type& rhs) const;

    private:

        storage_type m_data;

        template <class OS, class L1, class T1, class MT1>
        friend OS& operator<<(OS&, const xaxis_variant<L1, T1, MT1>&);

    };

    template <class OS, class L, class T, class MT>
    OS& operator<<(OS& out, const xaxis_variant<L, T, MT>& axis);

    /**************************
     * xaxis_variant_iterator *
     **************************/

    template <class L, class T, class MT>
    class xaxis_variant_iterator : public xtl::xrandom_access_iterator_base<xaxis_variant_iterator<L, T, MT>,
                                                                            typename xaxis_variant<L, T, MT>::value_type,
                                                                            typename xaxis_variant<L, T, MT>::difference_type,
                                                                            typename xaxis_variant<L, T, MT>::const_pointer,
                                                                            typename xaxis_variant<L, T, MT>::const_reference>
    {
    public:

        using self_type = xaxis_variant_iterator<L, T, MT>;
        using container_type = xaxis_variant<L, T, MT>;
        using key_reference = typename container_type::key_reference;
        using value_type = typename container_type::value_type;
        using reference = typename container_type::const_reference;
        using pointer = typename container_type::const_pointer;
        using difference_type = typename container_type::difference_type;
        using iterator_category = std::random_access_iterator_tag;
        using subiterator = typename container_type::subiterator;

        xaxis_variant_iterator() = default;
        xaxis_variant_iterator(subiterator it);

        self_type& operator++();
        self_type& operator--();

        self_type& operator+=(difference_type n);
        self_type& operator-=(difference_type n);

        difference_type operator-(const self_type& rhs) const;

        reference operator*() const;
        pointer operator->() const;

        bool equal(const self_type& rhs) const;
        bool less_than(const self_type& rhs) const;

    private:

        subiterator m_it;
    };

    template <class L, class T, class MT>
    typename xaxis_variant_iterator<L, T, MT>::difference_type operator-(const xaxis_variant_iterator<L, T, MT>& lhs,
                                                                         const xaxis_variant_iterator<L, T, MT>& rhs);

    template <class L, class T, class MT>
    bool operator==(const xaxis_variant_iterator<L, T, MT>& lhs, const xaxis_variant_iterator<L, T, MT>& rhs);

    template <class L, class T, class MT>
    bool operator<(const xaxis_variant_iterator<L, T, MT>& lhs, const xaxis_variant_iterator<L, T, MT>& rhs);

    /********************************
     * xaxis_variant implementation *
     ********************************/

    template <class L, class T, class MT>
    template <class LB>
    inline xaxis_variant<L, T, MT>::xaxis_variant(const xaxis<LB, T, MT>& axis)
        : m_data(axis)
    {
    }

    template <class L, class T, class MT>
    template <class LB>
    inline xaxis_variant<L, T, MT>::xaxis_variant(xaxis<LB, T, MT>&& axis)
        : m_data(std::move(axis))
    {
    }

    template <class L, class T, class MT>
    template <class LB>
    inline auto xaxis_variant<L, T, MT>::labels() const -> const label_list<LB>&
    {
        return xtl::get<xaxis<LB, T>>(m_data).labels();
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::label(size_type i) const -> key_type
    {
        return xtl::visit([i](auto&& arg) -> key_type { return arg.labels()[i]; }, m_data);
    }

    template <class L, class T, class MT>
    inline bool xaxis_variant<L, T, MT>::empty() const
    {
        return xtl::visit([](auto&& arg) { return arg.empty(); }, m_data);
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::size() const -> size_type
    {
        return xtl::visit([](auto&& arg) { return arg.size(); }, m_data);
    }

    template <class L, class T, class MT>
    inline bool xaxis_variant<L, T, MT>::contains(const key_type& key) const
    {
        auto lambda = [&key](auto&& arg) -> bool
        {
            using type = typename std::decay_t<decltype(arg)>::key_type;
            return arg.contains(xtl::get<type>(key));
        };
        return xtl::visit(lambda, m_data);
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::operator[](const key_type& key) const -> mapped_type
    {
        auto lambda = [&key](auto&& arg) -> mapped_type
        {
            using type = typename std::decay_t<decltype(arg)>::key_type;
            return arg[xtl::get<type>(key)];
        };
        return xtl::visit(lambda, m_data);
    }

    template <class L, class T, class MT>
    template <class F>
    inline auto xaxis_variant<L, T, MT>::filter(const F& f) const -> self_type
    {
        return xtl::visit([&f](const auto& arg) { return self_type(arg.filter(f)); }, m_data);
    }

    template <class L, class T, class MT>
    template <class F>
    inline auto xaxis_variant<L, T, MT>::filter(const F& f, size_type size) const -> self_type
    {
        return xtl::visit([&f, size](const auto& arg) { return self_type(arg.filter(f, size)); }, m_data);
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::find(const key_type& key) const -> const_iterator
    {
        auto lambda = [&key](auto&& arg) -> const_iterator
        {
            using type = typename std::decay_t<decltype(arg)>::key_type;
            return subiterator(arg.find(xtl::get<type>(key)));
        };
        return xtl::visit(lambda, m_data);
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::begin() const -> const_iterator
    {
        return cbegin();
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::end() const -> const_iterator
    {
        return cend();
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::cbegin() const -> const_iterator
    {
        return xtl::visit([](auto&& arg) { return subiterator(arg.cbegin()); }, m_data);
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::cend() const -> const_iterator
    {
        return xtl::visit([](auto&& arg) { return subiterator(arg.cend()); }, m_data);
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::rbegin() const -> const_reverse_iterator
    {
        return crbegin();
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::rend() const -> const_reverse_iterator
    {
        return crend();
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::crbegin() const -> const_reverse_iterator
    {
        return xtl::visit([](auto&& arg) { return subiterator(arg.cend()); }, m_data);
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::crend() const -> const_reverse_iterator
    {
        return xtl::visit([](auto&& arg) { return subiterator(arg.cbegin()); }, m_data);
    }

    template <class L, class T, class MT>
    template <class... Args>
    inline bool xaxis_variant<L, T, MT>::merge(const Args&... axes)
    {
        auto lambda = [&axes...](auto&& arg) -> bool
        {
            using type = std::decay_t<decltype(arg)>;
            return arg.merge(xtl::get<type>(axes.m_data)...);
        };
        return xtl::visit(lambda, m_data);
    }

    template <class L, class T, class MT>
    template <class... Args>
    inline bool xaxis_variant<L, T, MT>::intersect(const Args&... axes)
    {
        auto lambda = [&axes...](auto&& arg) -> bool
        {
            using type = std::decay_t<decltype(arg)>;
            return arg.intersect(xtl::get<type>(axes.m_data)...);
        };
        return xtl::visit(lambda, m_data);
    }

    template <class L, class T, class MT>
    inline bool xaxis_variant<L, T, MT>::operator==(const self_type& rhs) const
    {
        return m_data == rhs.m_data;
    }

    template <class L, class T, class MT>
    inline bool xaxis_variant<L, T, MT>::operator!=(const self_type& rhs) const
    {
        return m_data != rhs.m_data;
    }

    template <class OS, class L, class T, class MT>
    inline OS& operator<<(OS& out, const xaxis_variant<L, T, MT>& axis)
    {
        xtl::visit([&out](auto&& arg) { out << arg; }, axis.m_data);
        return out;
    }

    /*****************************************
     * xaxis_variant_iterator implementation *
     *****************************************/

    template<class L, class T, class MT>
    inline xaxis_variant_iterator<L, T, MT>::xaxis_variant_iterator(subiterator it)
        : m_it(it)
    {
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant_iterator<L, T, MT>::operator++() -> self_type&
    {
        xtl::visit([](auto&& arg) { ++arg; }, m_it);
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant_iterator<L, T, MT>::operator--() -> self_type&
    {
        xtl::visit([](auto&& arg) { --arg; }, m_it);
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant_iterator<L, T, MT>::operator+=(difference_type n) -> self_type&
    {
        xtl::visit([n](auto&& arg) { arg += n; }, m_it);
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant_iterator<L, T, MT>::operator-=(difference_type n) -> self_type&
    {
        xtl::visit([n](auto&& arg) { arg -= n; }, m_it);
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant_iterator<L, T, MT>::operator-(const self_type& rhs) const -> difference_type
    {
        xtl::visit([&rhs](auto&& arg) { return arg - std::get<std::decay_t<decltype(arg)>>(rhs); }, m_it);
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant_iterator<L, T, MT>::operator*() const -> reference
    {
        return xtl::visit([](auto&& arg)
        {
            return reference(key_reference(xtl::closure(arg->first)), arg->second);
        }, m_it);
    }

    template <class T>
    struct DEBUG;

    template <class L, class T, class MT>
    inline auto xaxis_variant_iterator<L, T, MT>::operator->() const -> pointer
    {
        return xtl::visit([](auto&& arg)
        {
            return pointer(reference(key_reference(xtl::closure(arg->first)), arg->second));
        }, m_it);
    }

    template <class L, class T, class MT>
    inline bool xaxis_variant_iterator<L, T, MT>::equal(const self_type& rhs) const
    {
        return m_it == rhs.m_it;
    }

    template <class L, class T, class MT>
    inline bool xaxis_variant_iterator<L, T, MT>::less_than(const self_type& rhs) const
    {
        return m_it < rhs.m_it;
    }

    template <class L, class T, class MT>
    inline auto operator-(const xaxis_variant_iterator<L, T, MT>& lhs, const xaxis_variant_iterator<L, T, MT>& rhs)
        -> typename xaxis_variant_iterator<L, T, MT>::difference_type
    {
        return lhs.operator-(rhs);
    }

    template <class L, class T, class MT>
    inline bool operator==(const xaxis_variant_iterator<L, T, MT>& lhs, const xaxis_variant_iterator<L, T, MT>& rhs)
    {
        return lhs.equal(rhs);
    }

    template <class L, class T, class MT>
    inline bool operator<(const xaxis_variant_iterator<L, T, MT>& lhs, const xaxis_variant_iterator<L, T, MT>& rhs)
    {
        return lhs.less_than(rhs);
    }
}

#endif
