/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
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
    namespace mpl = xtl::mpl;

    namespace detail
    {
        template <class L0, class...>
        struct xvector_variant_size_traits
        {
            using size_type = typename std::vector<L0>::size_type;
            using difference_type = typename std::vector<L0>::difference_type;
        };

        template <bool is_const, class L>
        struct xvector_variant_value_traits;

        template <bool is_const, class T>
        struct xvector_variant_value_traits<is_const, mpl::vector<T>>
        {
            using value_type = T;
            using const_reference = const T&;
            using reference = std::conditional_t<is_const, const_reference, T&>;
            using const_pointer = const T*;
            using pointer = std::conditional_t<is_const, const_pointer, T*>;
        };

        template <bool is_const, class... T>
        struct xvector_variant_value_traits<is_const, mpl::vector<T...>>
        {
            using value_type = xtl::variant<T...>;
            using const_reference = xtl::variant<xtl::xclosure_wrapper<const T&>...>;
            using reference = std::conditional_t<is_const,
                                                 const_reference,
                                                 xtl::variant<xtl::xclosure_wrapper<T&>...>>;
            using const_pointer = xtl::variant<const T*...>;
            using pointer = std::conditional_t<is_const,
                                               const_pointer,
                                               xtl::variant<T*...>>;
        };

        template <bool is_const, bool is_ref, class... V>
        struct xvector_variant_traits
        {
            using storage_type = 
                std::conditional_t<is_ref,
                                   std::conditional_t<is_const,
                                                      xtl::variant<xtl::xclosure_wrapper<const V&>...>,
                                                      xtl::variant<xtl::xclosure_wrapper<V&>...>>,
                                   xtl::variant<V...>>;
            using value_type_list = mpl::unique_t<mpl::vector<typename V::value_type...>>;
            using value_traits = xvector_variant_value_traits<is_const, value_type_list>;
            using size_traits = xvector_variant_size_traits<V...>;

            using value_type = typename value_traits::value_type;
            using reference = typename value_traits::reference;
            using const_reference = typename value_traits::const_reference;
            using pointer = typename value_traits::pointer;
            using const_pointer = typename value_traits::const_pointer;
            using size_type = typename size_traits::size_type;
            using difference_type = typename size_traits::difference_type;

            using const_iterator = xtl::variant<typename V::const_iterator...>;
            using iterator = std::conditional_t<is_const,
                                                const_iterator,
                                                xtl::variant<typename V::iterator...>>;
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

        template <class T1, class T2>
        using is_unlike = xtl::negation<std::is_same<std::decay_t<T1>, std::decay_t<T2>>>;
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

        using self_type = xvector_variant_base;
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

        // Size and capacity

        bool empty() const;
        size_type size() const;
        void resize(size_type new_size);
        size_type max_size() const;
        size_type capacity() const;
        void reserve(size_type new_cap);
        void shrink_to_fit();
        void clear();

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

        storage_type& storage();
        const storage_type& storage() const;

        // Iterators

        iterator begin();
        iterator end();

        const_iterator begin() const;
        const_iterator end() const;

        const_iterator cbegin() const;
        const_iterator cend() const;

        // swap

        void swap(self_type& rhs);

        // Comparison

        bool equal(const self_type& rhs) const;
        bool less_than(const self_type& rhs) const;

    protected:

        template <class T,
                  XTL_REQUIRES(detail::is_unlike<T, self_type>)>
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

    template <class... V>
    class xvector_variant : public xvector_variant_base<detail::xvector_variant_traits<false, false, V...>>
    {
    public:

        using traits_type = detail::xvector_variant_traits<false, false, V...>;
        using base_type = xvector_variant_base<traits_type>;
        using self_type = xvector_variant<V...>;

        template <class OV,
                  XTL_REQUIRES(detail::is_unlike<OV, self_type>)>
        inline xvector_variant(OV&& v)
            : base_type(std::forward<OV>(v))
        {
        }

        ~xvector_variant() = default;

        xvector_variant(const self_type&) = default;
        self_type& operator=(const self_type&) = default;

        xvector_variant(self_type&&) = default;
        self_type& operator=(self_type&&) = default;
    };

    template <class... V>
    void swap(xvector_variant<V...>& lhs, xvector_variant<V...>& rhs);

    template <class T, class... V>
    T& xget_vector(xvector_variant<V...>& v);

    template <class T, class... V>
    const T& xget_vector(const xvector_variant<V...>& v);

    template <class T, class... V>
    T&& xget_vector(xvector_variant<V...>&& v);

    template <class T, class... V>
    const T&& xget_vector(const xvector_variant<V...>&& v);

    /***********************
     * xvector_variant_ref *
     ***********************/

    template <class ...V>
    class xvector_variant_ref : public xvector_variant_base<detail::xvector_variant_traits<false, true, V...>>
    {
    public:

        using traits_type = detail::xvector_variant_traits<false, true, V...>;
        using base_type = xvector_variant_base<traits_type>;
        using self_type = xvector_variant_ref<V...>;

        template <class OV,
                  XTL_REQUIRES(detail::is_unlike<OV, self_type>)>
        inline xvector_variant_ref(OV& v)
            : base_type(v)
        {
        }

        ~xvector_variant_ref() = default;

        xvector_variant_ref(const self_type&) = default;
        self_type& operator=(const self_type& rhs) = default;

        xvector_variant_ref(self_type&&) = default;
        self_type& operator=(self_type&&) = default;
    };

    template <class... V>
    void swap(xvector_variant_ref<V...>& lhs, xvector_variant_ref<V...>& rhs);

    template <class T, class... V>
    T& xget_vector(xvector_variant_ref<V...>& v);

    template <class T, class... V>
    const T& xget_vector(const xvector_variant_ref<V...>& v);

    template <class T, class... V>
    T& xget_vector(xvector_variant_ref<V...>&& v);

    template <class T, class... V>
    const T& xget_vector(const xvector_variant_ref<V...>&& v);

    /************************
     * xvector_variant_cref *
     ************************/

    template <class ...V>
    class xvector_variant_cref : public xvector_variant_base<detail::xvector_variant_traits<true, true, V...>>
    {
    public:

        using traits_type = detail::xvector_variant_traits<true, true, V...>;
        using base_type = xvector_variant_base<traits_type>;
        using self_type = xvector_variant_cref<V...>;

        template <class OV,
                  XTL_REQUIRES(detail::is_unlike<OV, self_type>)>
        inline xvector_variant_cref(OV& v)
            : base_type(v)
        {
        }

        ~xvector_variant_cref() = default;

        xvector_variant_cref(const self_type&) = default;
        xvector_variant_cref(self_type&&) = default;
    };

    template <class T, class... V>
    const T& xget_vector(xvector_variant_cref<V...>& v);

    template <class T, class... V>
    const T& xget_vector(const xvector_variant_cref<V...>& v);

    template <class T, class... V>
    const T& xget_vector(xvector_variant_cref<V...>&& v);

    template <class T, class... V>
    const T& xget_vector(const xvector_variant_cref<V...>&& v);

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
        return xtl::visit([](const auto& arg) { return detail::unwrap(arg).empty(); }, m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::size() const -> size_type
    {
        return xtl::visit([](const auto& arg) { return detail::unwrap(arg).size(); }, m_storage);
    }

    template <class T>
    inline void xvector_variant_base<T>::resize(size_type size)
    {
        xtl::visit([size](auto& arg) { detail::unwrap(arg).resize(size); }, m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::max_size() const -> size_type
    {
        return xtl::visit([](const auto& arg) { return detail::unwrap(arg).max_size(); }, m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::capacity() const -> size_type
    {
        return xtl::visit([](const auto& arg) { return detail::unwrap(arg).capacity(); }, m_storage);
    }

    template <class T>
    inline void xvector_variant_base<T>::reserve(size_type new_cap)
    {
        xtl::visit([new_cap](auto& arg) { detail::unwrap(arg).reserve(new_cap); }, m_storage);
    }

    template <class T>
    inline void xvector_variant_base<T>::shrink_to_fit()
    {
        xtl::visit([](auto& arg) { detail::unwrap(arg).shrink_to_fit(); }, m_storage);
    }

    template <class T>
    inline void xvector_variant_base<T>::clear()
    {
        xtl::visit([](auto& arg) { detail::unwrap(arg).clear(); }, m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::operator[](size_type i) -> reference
    {
        return xtl::visit([i](auto& arg)
        {
            return reference(xtl::closure(detail::unwrap(arg)[i]));
        },
        m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::operator[](size_type i) const -> const_reference
    {
        return xtl::visit([i](const auto& arg)
        {
            return const_reference(xtl::closure(detail::unwrap(arg)[i]));
        },
        m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::at(size_type i) -> reference
    {
        return xtl::visit([i](auto& arg)
        {
            return reference(xtl::closure(detail::unwrap(arg).at(i)));
        },
        m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::at(size_type i) const -> const_reference
    {
        return xtl::visit([i](const auto& arg)
        {
            return const_reference(xtl::closure(detail::unwrap(arg)[i]));
        },
        m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::front() -> reference
    {
        return xtl::visit([](auto& arg)
        {
            return reference(xtl::closure(detail::unwrap(arg).front()));
        },
        m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::front() const -> const_reference
    {
        return xtl::visit([](const auto& arg)
        {
            return const_reference(xtl::closure(detail::unwrap(arg).front()));
        },
        m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::back() -> reference
    {
        return xtl::visit([](auto& arg)
        {
            return reference(xtl::closure(detail::unwrap(arg).back()));
        },
        m_storage);
    }

    template <class T>
    inline auto xvector_variant_base<T>::back() const -> const_reference
    {
        return xtl::visit([](const auto& arg)
        {
            return const_reference(xtl::closure(detail::unwrap(arg).back()));
        },
        m_storage);
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
    inline auto xvector_variant_base<T>::storage() -> storage_type&
    {
        return m_storage;
    }

    template <class T>
    inline auto xvector_variant_base<T>::storage() const -> const storage_type&
    {
        return m_storage;
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
    inline void xvector_variant_base<T>::swap(self_type& rhs)
    {
        m_storage.swap(rhs.m_storage);
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
        return xtl::visit([&rhs](auto& arg) { return arg - xtl::xget<std::decay_t<decltype(arg)>>(rhs.m_it); }, m_it);
    }

    template <class T>
    inline auto xvector_variant_iterator<T>::operator*() const -> reference
    {
        return xtl::visit([](auto& arg) { return reference(xtl::closure(*arg)); }, m_it);
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

    template <class... V>
    inline void swap(xvector_variant<V...>& lhs, xvector_variant<V...>& rhs)
    {
        lhs.swap(rhs);
    }

    template <class T, class... V>
    inline T& xget_vector(xvector_variant<V...>& v)
    {
        return xtl::xget<T>(v.storage());
    }

    template <class T, class... V>
    inline const T& xget_vector(const xvector_variant<V...>& v)
    {
        return xtl::xget<T>(v.storage());
    }

    template <class T, class... V>
    inline T&& xget_vector(xvector_variant<V...>&& v)
    {
        return xtl::xget<T>(std::move(v.storage()));
    }

    template <class T, class... V>
    inline const T&& xget_vector(const xvector_variant<V...>&& v)
    {
        return xtl::xget<T>(std::move(v.storage()));
    }

    /**************************************
     * xvector_variant_ref implementation *
     **************************************/

    template <class... L>
    inline void swap(xvector_variant_ref<L...>& lhs, xvector_variant_ref<L...>& rhs)
    {
        lhs.swap(rhs);
    }

    template <class T, class... V>
    inline T& xget_vector(xvector_variant_ref<V...>& v)
    {
        return xtl::xget<T&>(v.storage());
    }

    template <class T, class... V>
    inline const T& xget_vector(const xvector_variant_ref<V...>& v)
    {
        return xtl::xget<const T&>(v.storage());
    }

    template <class T, class... V>
    inline T& xget_vector(xvector_variant_ref<V...>&& v)
    {
        return xtl::xget<T&>(std::move(v.storage()));
    }

    template <class T, class... V>
    inline const T& xget_vector(const xvector_variant_ref<V...>&& v)
    {
        return xtl::xget<const T&>(std::move(v.storage()));
    }

    /***************************************
     * xvector_variant_cref implementation *
     ***************************************/

    template <class T, class... V>
    const T& xget_vector(xvector_variant_cref<V...>& v)
    {
        return xtl::xget<const T&>(v.storage());
    }

    template <class T, class... V>
    const T& xget_vector(const xvector_variant_cref<V...>& v)
    {
        return xtl::xget<const T&>(v.storage());
    }

    template <class T, class... V>
    const T& xget_vector(xvector_variant_cref<V...>&& v)
    {
        return xtl::xget<const T&>(std::move(v.storage()));
    }

    template <class T, class... V>
    const T& xget_vector(const xvector_variant_cref<V...>&& v)
    {
        return xtl::xget<const T&>(std::move(v.storage()));
    }
}

#endif

