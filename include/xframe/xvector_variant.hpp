/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVECTOR_VARIANT_HPP
#define XFRAME_XVECTOR_VARIANT_HPP

#include "xtl/xclosure.hpp"
#include "xtl/xiterator_base.hpp"
#include "xtl/xmeta_utils.hpp"
#include "xtl/xvariant.hpp"

namespace xf
{

    namespace detail
    {
        template <class L0, class...>
        struct xvector_variant_size_type
        {
            using size_type = typename std::vector<L0>::size_type;
            using difference_type = typename std::vector<L0>::difference_type;
        };

        template <bool is_const, bool is_ref, class... L>
        struct xvector_variant_traits
        {
            using storage_type =
                std::conditional_t<is_ref,
                                   std::conditional_t<is_const,
                                                      xtl::variant<xtl::xclosure_wrapper<const std::vector<L>&>...>,
                                                      xtl::variant<xtl::xclosure_wrapper<std::vector<L>&>...>>,
                                   xtl::variant<std::vector<L>...>>;
            using value_type = xtl::variant<L...>;
            using const_reference = xtl::variant<xtl::xclosure_wrapper<const L&>...>;
            using reference = std::conditional_t<is_const,
                                                 const_reference,
                                                 xtl::variant<xtl::xclosure_wrapper<L&>...>>;
            using const_pointer = xtl::variant<const L*...>;
            using pointer = std::conditional_t<is_const,
                                               const_pointer,
                                               xtl::variant<L*...>>;
            using size_type = typename xvector_variant_size_type<L...>::size_type;
            using difference_type = typename xvector_variant_size_type<L...>::difference_type;

            using const_iterator = xtl::variant<typename std::vector<L>::const_iterator...>;
            using iterator = std::conditional_t<is_const,
                                                const_iterator,
                                                xtl::variant<typename std::vector<L>::iterator...>>;
        };

        template <bool is_const, class traits>
        struct xvector_variant_iterator_traits
        {
            using value_type = typename traits::value_type;
            using reference = std::conditional_t<is_const, typename traits::const_reference, typename traits::reference>;
            using pointer = std::conditional_t<is_const, typename traits::const_pointer, typename traits::pointer>;
            using difference_type = typename traits::difference_type;
            using iterator = std::conditional_t<is_const, typename traits::const_iterator, typename traits::iterator>;
        };
    }

    /************************
     * xvector_variant_base *
     ************************/

    template <class traits>
    class xvector_variant_iterator;

    template <class traits>
    class xvector_variant_base
    {
    public:

        using self_type = xvector_variant_base<traits>;
        using value_type = typename traits::value_type;
        using reference = typename traits::reference;
        using const_reference = typename traits::const_reference;
        using pointer = typename traits::pointer;
        using const_pointer = typename traits::const_pointer;
        using size_type = typename traits::size_type;
        using difference_type = typename traits::difference_type;
        using storage_type = typename traits::storage_type;

        using iterator_traits = detail::xvector_variant_iterator_traits<false, traits>;
        using const_iterator_traits = detail::xvector_variant_iterator_traits<true, traits>;
        
        using iterator = xvector_variant_iterator<iterator_traits>;
        using const_iterator = xvector_variant_iterator<const_iterator_traits>;

        // Capacity

        bool empty() const;
        size_type size() const;
        size_type max_size() const;

        // Element access

        reference operator[](size_type i);
        const_reference operator[](size_type i) const;

        reference at(size_type i);
        const_reference at(size_type i) const;

        reference front();
        const_reference front() const;

        reference back();
        const_reference back() const;

        pointer data();
        const_pointer data() const;

        // Iterators

        iterator begin();
        iterator end();

        const_iterator begin() const;
        const_iterator end() const;

        const_iterator cbegin() const;
        const_iterator cend() const;

        // Modifiers

        void clear();
        void resize(size_type size);

        // Comparison

        bool equal(const self_type& rhs) const;
        bool less_than(const self_type& rhs) const;

    protected:

        template <class T, 
            std::enable_if_t<
                !std::is_same<std::decay_t<T>, self_type>::value,
                bool
            > = true>
        xvector_variant_base(T&& v)
            : m_storage(std::forward<T>(v))
        {
        }

        ~xvector_variant_base() = default;

        xvector_variant_base(const self_type&) = default;
        xvector_variant_base(self_type&&) = default;

        self_type& operator=(const self_type& rhs) = default;
        self_type& operator=(self_type&& rhs) = default;

    private:

        storage_type m_storage;
    };

    template <class T>
    bool operator==(const xvector_variant_base<T>& lhs, const xvector_variant_base<T>& rhs);

    template <class T>
    bool operator!=(const xvector_variant_base<T>& lhs, const xvector_variant_base<T>& rhs);

    template <class T>
    bool operator<(const xvector_variant_base<T>& lhs, const xvector_variant_base<T>& rhs);

    template <class T>
    bool operator<=(const xvector_variant_base<T>& lhs, const xvector_variant_base<T>& rhs);

    template <class T>
    bool operator>(const xvector_variant_base<T>& lhs, const xvector_variant_base<T>& rhs);

    template <class T>
    bool operator>=(const xvector_variant_base<T>& lhs, const xvector_variant_base<T>& rhs);

    /****************************
     * xvector_variant_iterator *
     ****************************/

    template <class traits>
    class xvector_variant_iterator : public xtl::xrandom_access_iterator_base<xvector_variant_iterator<traits>,
                                                                              typename traits::value_type,
                                                                              typename traits::difference_type,
                                                                              typename traits::pointer,
                                                                              typename traits::reference>
    {
    public:

        using self_type = xvector_variant_iterator<traits>;
        using value_type = typename traits::value_type;
        using reference = typename traits::reference;
        using pointer = typename traits::pointer;
        using difference_type = typename traits::difference_type;
        using iterator_category = std::random_access_iterator_tag;
        using subiterator = typename traits::iterator;

        xvector_variant_iterator() = default;
        xvector_variant_iterator(subiterator it);

        self_type& operator++();
        self_type& operator--();

        self_type& operator+=(difference_type n);
        self_type& operator-=(difference_type n);

        difference_type operator-(const self_type& rhs) const;

        reference operator*() const;
        //pointer operator->() const;
        
        bool equal(const self_type& rhs) const;
        bool less_than(const self_type& rhs) const;

    private:

        subiterator m_it;
    };

    template <class T>
    bool operator==(const xvector_variant_iterator<T>& lhs, const xvector_variant_iterator<T>& rhs);

    template <class T>
    bool operator<(const xvector_variant_iterator<T>& lhs, const xvector_variant_iterator<T>& rhs);

    /*******************
     * xvector_variant *
     *******************/

    template <class... L>
    class xvector_variant : public xvector_variant_base<detail::xvector_variant_traits<false, false, L...>>
    {
    public:

        using traits_type = detail::xvector_variant_traits<false, false, L...>;
        using base_type = xvector_variant_base<traits_type>;
        using self_type = xvector_variant<L...>;

        template <class T>
        xvector_variant(const std::vector<T>& v);

        template <class T>
        xvector_variant(std::vector<T>&& v);

        ~xvector_variant() = default;

        xvector_variant(const self_type&) = default;
        self_type& operator=(const self_type&) = default;

        xvector_variant(self_type&&) = default;
        self_type& operator=(self_type&&) = default;
    };

    /************************
     *  xvector_variant_ref *
     ************************/

    template <class... L>
    class xvector_variant_ref : public xvector_variant_base<detail::xvector_variant_traits<false, true, L...>>
    {
    public:

        using traits_type = detail::xvector_variant_traits<false, true, L...>;
        using base_type = xvector_variant_base<traits_type>;
        using self_type = xvector_variant_ref<L...>;

        template <class T>
        xvector_variant_ref(std::vector<T>& v);

        ~xvector_variant_ref() = default;

        xvector_variant_ref(const self_type&) = default;
        self_type& operator=(const self_type& rhs) = default;

        xvector_variant_ref(self_type&&) = default;
        self_type& operator=(self_type&&) = default;
    };

    /************************
     * xvector_variant_cref *
     ************************/

    template <class... L>
    class xvector_variant_cref : public xvector_variant_base<detail::xvector_variant_traits<true, true, L...>>
    {
    public:

        using traits_type = detail::xvector_variant_traits<true, true, L...>;
        using base_type = xvector_variant_base<traits_type>;
        using self_type = xvector_variant_cref<L...>;

        template <class T>
        xvector_variant_cref(const std::vector<T>& v);

        ~xvector_variant_cref() = default;

        xvector_variant_cref(const self_type&) = default;
        xvector_variant_cref(self_type&&) = default;
    };

    /***************************************
     * xvector_variant_base implementation *
     ***************************************/

    namespace detail
    {
        template <class T>
        inline T& unwrap(T& arg)
        {
            return arg;
        }

        template <class T>
        inline const T& unwrap(const T& arg)
        {
            return arg;
        }

        template <class T>
        inline T& unwrap(xtl::xclosure_wrapper<T&>& arg)
        {
            return arg.get();
        }

        template <class T>
        inline const T& unwrap(const xtl::xclosure_wrapper<T&>& arg)
        {
            return arg.get();
        }
    }

    template <class T>
    inline bool xvector_variant_base<T>::empty() const
    {
        return xtl::visit([](auto&& arg) { return detail::unwrap(arg).empty(); }, m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::size() const -> size_type
    {
        return xtl::visit([](auto&& arg) { return detail::unwrap(arg).size(); }, m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::max_size() const -> size_type
    {
        return xtl::visit([](auto&& arg) { return detail::unwrap(arg).max_size(); }, m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::operator[](size_type i) -> reference
    {
        return xtl::visit([i](auto& arg) { return reference(detail::unwrap(arg)[i]); }, m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::operator[](size_type i) const -> const_reference
    {
        return xtl::visit([i](const auto& arg) { return const_reference(detail::unwrap(arg).at(i)); }, m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::at(size_type i) -> reference
    {
        return xtl::visit([i](auto& arg) { return reference(detail::unwrap(arg).at(i)); }, m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::at(size_type i) const -> const_reference
    {
        return xtl::visit([i](const auto& arg) { return const_reference(detail::unwrap(arg)[i]); }, m_storage);
    }
    
    template <class T>
    inline auto xvector_variant_base<T>::front() -> reference
    {
        return xtl::visit([](auto& arg) { return reference(detail::unwrap(arg).front()); }, m_storage);
    }
    
    template <class T>
    inline auto xvector_variant_base<T>::front() const -> const_reference
    {
        return xtl::visit([](const auto& arg) { return const_reference(detail::unwrap(arg).front()); }, m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::back() -> reference
    {
        return xtl::visit([](auto& arg) { return reference(detail::unwrap(arg).back()); }, m_storage);
    }
    
    template <class T>
    inline auto xvector_variant_base<T>::back() const -> const_reference
    {
        return xtl::visit([](const auto& arg) { return const_reference(detail::unwrap(arg).back()); }, m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::data() -> pointer
    {
        return xtl::visit([](auto& arg) { return pointer(detail::unwrap(arg).data()); }, m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::data() const -> const_pointer
    {
        return xtl::visit([](const auto& arg) { return const_pointer(detail::unwrap(arg).data()); }, m_storage);
    }
    
    template <class T>
    inline auto xvector_variant_base<T>::begin() -> iterator
    {
        return xtl::visit([](auto& arg) { return iterator(detail::unwrap(arg).begin()); }, m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::end() -> iterator
    {
        return xtl::visit([](auto& arg) { return iterator(detail::unwrap(arg).end()); }, m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::begin() const -> const_iterator
    {
        return cbegin();
    }

    template <class T>
    inline auto xvector_variant_base<T>::end() const -> const_iterator
    {
        return cend();
    }

    template <class T>
    inline auto xvector_variant_base<T>::cbegin() const -> const_iterator
    {
        return xtl::visit([](const auto& arg) { return const_iterator(detail::unwrap(arg).cbegin()); }, m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::cend() const -> const_iterator
    {
        return xtl::visit([](const auto& arg) { return const_iterator(detail::unwrap(arg).cend()); }, m_storage);
    }

    template <class T>
    inline void xvector_variant_base<T>::clear()
    {
        xtl::visit([](auto& arg) { detail::unwrap(arg).clear(); }, m_storage);
    }

    template <class T>
    inline void xvector_variant_base<T>::resize(size_type size)
    {
        xtl::visit([size](auto& arg) { detail::unwrap(arg).resize(size); }, m_storage);
    }

    template <class T>
    inline bool xvector_variant_base<T>::equal(const self_type& rhs) const
    {
        return m_storage == rhs.m_storage;
    }

    template <class T>
    inline bool xvector_variant_base<T>::less_than(const self_type& rhs) const
    {
        return m_storage < rhs.m_storage;
    }
    
    template <class T>
    inline bool operator==(const xvector_variant_base<T>& lhs, const xvector_variant_base<T>& rhs)
    {
        return lhs.equal(rhs);
    }

    template <class T>
    inline bool operator!=(const xvector_variant_base<T>& lhs, const xvector_variant_base<T>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class T>
    inline bool operator<(const xvector_variant_base<T>& lhs, const xvector_variant_base<T>& rhs)
    {
        return lhs.less_than(rhs);
    }

    template <class T>
    inline bool operator<=(const xvector_variant_base<T>& lhs, const xvector_variant_base<T>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class T>
    inline bool operator>(const xvector_variant_base<T>& lhs, const xvector_variant_base<T>& rhs)
    {
        return rhs < lhs;
    }

    template <class T>
    inline bool operator>=(const xvector_variant_base<T>& lhs, const xvector_variant_base<T>& rhs)
    {
        return !(lhs < rhs);
    }

    /****************************
     * xvector_variant_iterator *
     ****************************/

    template <class T>
    inline xvector_variant_iterator<T>::xvector_variant_iterator(subiterator it)
        : m_it(it)
    {
    }

    template <class T>
    inline auto xvector_variant_iterator<T>::operator++() -> self_type&
    {
        xtl::visit([](auto& arg) { ++arg; }, m_it);
        return *this;
    }

    template <class T>
    inline auto xvector_variant_iterator<T>::operator--() -> self_type&
    {
        xtl::visit([](auto& arg) { --arg; }, m_it);
        return *this;
    }

    template <class T>
    inline auto xvector_variant_iterator<T>::operator+=(difference_type n) -> self_type&
    {
        xtl::visit([n](auto& arg) { arg += n; }, m_it);
        return *this;
    }

    template <class T>
    inline auto xvector_variant_iterator<T>::operator-=(difference_type n) -> self_type&
    {
        xtl::visit([n](auto& arg) { arg -= n; }, m_it);
        return *this;
    }

    template <class T>
    inline auto xvector_variant_iterator<T>::operator-(const self_type& rhs) const -> difference_type
    {
        return xtl::visit([&rhs](auto& arg) { return arg - xtl::xget<std::decay_t<decltype(arg)>>(rhs); }, m_it);
    }

    template <class T>
    inline auto xvector_variant_iterator<T>::operator*() const -> reference
    {
        return xtl::visit([](auto& arg) { return reference(*arg); }, m_it);
    }

    template <class T>
    inline bool xvector_variant_iterator<T>::equal(const self_type& rhs) const
    {
        return m_it == rhs.m_it;
    }

    template <class T>
    inline bool xvector_variant_iterator<T>::less_than(const self_type& rhs) const
    {
        return m_it < rhs.m_it;
    }

    template <class T>
    inline bool operator==(const xvector_variant_iterator<T>& lhs, const xvector_variant_iterator<T>& rhs)
    {
        return lhs.equal(rhs);
    }

    template <class T>
    inline bool operator<(const xvector_variant_iterator<T>& lhs, const xvector_variant_iterator<T>& rhs)
    {
        return lhs.less_than(rhs);
    }

    /**********************************
     * xvector_variant implementation *
     **********************************/

    template <class... L>
    template <class T>
    xvector_variant<L...>::xvector_variant(const std::vector<T>& v)
        : base_type(v)
    {
    }

    template <class... L>
    template <class T>
    xvector_variant<L...>::xvector_variant(std::vector<T>&& v)
        : base_type(std::move(v))
    {
    }

    /**************************************
     * xvector_variant_ref implementation *
     **************************************/

    template <class... L>
    template <class T>
    inline xvector_variant_ref<L...>::xvector_variant_ref(std::vector<T>& v)
        : base_type(v)
    {
    }

    /***************************************
     * xvector_variant_cref implementation *
     ***************************************/

    template <class... L>
    template <class T>
    inline xvector_variant_cref<L...>::xvector_variant_cref(const std::vector<T>& v)
        : base_type(v)
    {
    }
}

#endif
