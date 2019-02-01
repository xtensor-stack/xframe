/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVECTOR_LIKE_VARIANT_HPP
#define XFRAME_XVECTOR_LIKE_VARIANT_HPP

#include "xtl/xclosure.hpp"
#include "xtl/xiterator_base.hpp"
#include "xtl/xmeta_utils.hpp"
#include "xtl/xvariant.hpp"

//TODO: move in xtl:
namespace xtl
{
    namespace mpl
    {
        namespace detail
        {
            template <class L>
            struct unique_impl;

            template <template <class...> class L, class... T>
            struct unique_impl<L<T...>>
            {
                using type = merge_set_t<L<>, L<T...>>;
            };
        }

        template <class L>
        struct unique : detail::unique_impl<L>
        {
        };

        template <class L>
        using unique_t = typename unique<L>::type;
    }
}

namespace xf
{

    namespace mpl = xtl::mpl;

    namespace detail
    {
        template <class L0, class...>
        struct xvlv_size_traits
        {
            using size_type = typename std::vector<L0>::size_type;
            using difference_type = typename std::vector<L0>::difference_type;
        };

        template <bool is_const, class L>
        struct xvlv_value_traits;

        template <bool is_const, class T>
        struct xvlv_value_traits<is_const, mpl::vector<T>>
        {
            using value_type = T;
            using const_reference = const T&;
            using reference = std::conditional_t<is_const, const_reference, T&>;
            using const_pointer = const T*;
            using pointer = std::conditional_t<is_const, const_pointer, T*>;
        };

        template <bool is_const, class... T>
        struct xvlv_value_traits<is_const, mpl::vector<T...>>
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
        struct xvector_like_variant_traits
        {
            using storage_type = 
                std::conditional_t<is_ref,
                                   std::conditional_t<is_const,
                                                      xtl::variant<xtl::xclosure_wrapper<const V&>...>,
                                                      xtl::variant<xtl::xclosure_wrapper<V&>...>>,
                                   xtl::variant<V...>>;
            using value_type_list = mpl::unique_t<mpl::vector<typename V::value_type...>>;
            using value_traits = xvlv_value_traits<is_const, value_type_list>;
            using size_traits = xvlv_size_traits<V...>;

            using value_type = typename value_traits::value_type;
            using reference = typename value_traits::reference;
            using const_reference = typename value_traits::const_reference;
            using pointer = typename value_traits::pointer;
            using const_pointer = typename value_traits::const_pointer;
            using size_type = typename size_traits::size_type;
            using difference_type = typename size_traits::difference_type;
        };

        template <class T1, class T2>
        using is_unlike = xtl::negation<std::is_same<std::decay_t<T1>, std::decay_t<T2>>>;
    }

    /*****************************
     * xvector_like_variant_base *
     *****************************/

    template <class traits>
    class xvector_like_variant_base
    {
    public:

        using self_type = xvector_like_variant_base;
        using value_type = typename traits::value_type;
        using reference = typename traits::reference;
        using const_reference = typename traits::const_reference;
        using pointer = typename traits::pointer;
        using const_pointer = typename traits::const_pointer;
        using size_type = typename traits::size_type;
        using difference_type = typename traits::difference_type;
        using storage_type = typename traits::storage_type;

        // Size and capacity

        bool empty() const;
        size_type size() const;
        void resize(size_type new_size);
        // Needs fix in xtensor
        /*size_type max_size() const;
        size_type capacity() const;
        void reserve(size_type new_cap);
        void shrink_to_fit();
        void clear();*/

        // Element access

        reference operator[](size_type i);
        const_reference operator[](size_type i) const;

        // Needs fix in xtensor
        /*reference at(size_type i);
        const_reference at(size_type i) const;*/

        reference front();
        const_reference front() const;

        reference back();
        const_reference back() const;

        pointer data();
        const_pointer data() const;

        storage_type& storage();
        const storage_type& storage() const;

        // Comparison

        bool equal(const self_type& rhs) const;
        bool less_than(const self_type& rhs) const;

    protected:

        template <class T,
                  XTL_REQUIRES(detail::is_unlike<T, self_type>)>
        xvector_like_variant_base(T&& v)
            : m_storage(std::forward<T>(v))
        {
        }

        ~xvector_like_variant_base() = default;

        xvector_like_variant_base(const self_type&) = default;
        xvector_like_variant_base(self_type&&) = default;

        self_type& operator=(const self_type& rhs) = default;
        self_type& operator=(self_type&& rhs) = default;


    private:

        storage_type m_storage;
    };

    template <class T>
    bool operator==(const xvector_like_variant_base<T>& lhs, const xvector_like_variant_base<T>& rhs);

    template <class T>
    bool operator!=(const xvector_like_variant_base<T>& lhs, const xvector_like_variant_base<T>& rhs);

    template <class T>
    bool operator<(const xvector_like_variant_base<T>& lhs, const xvector_like_variant_base<T>& rhs);

    template <class T>
    bool operator<=(const xvector_like_variant_base<T>& lhs, const xvector_like_variant_base<T>& rhs);

    template <class T>
    bool operator>(const xvector_like_variant_base<T>& lhs, const xvector_like_variant_base<T>& rhs);

    template <class T>
    bool operator>=(const xvector_like_variant_base<T>& lhs, const xvector_like_variant_base<T>& rhs);

    /************************
     * xvector_like_variant *
     ************************/

    template <class... V>
    class xvector_like_variant : public xvector_like_variant_base<detail::xvector_like_variant_traits<false, false, V...>>
    {
    public:

        using traits_type = detail::xvector_like_variant_traits<false, false, V...>;
        using base_type = xvector_like_variant_base<traits_type>;
        using self_type = xvector_like_variant<V...>;

        template <class OV,
                  XTL_REQUIRES(detail::is_unlike<OV, self_type>)>
        inline xvector_like_variant(OV&& v)
            : base_type(std::forward<OV>(v))
        {
        }

        ~xvector_like_variant() = default;

        xvector_like_variant(const self_type&) = default;
        self_type& operator=(const self_type&) = default;

        xvector_like_variant(self_type&&) = default;
        self_type& operator=(self_type&&) = default;
    };

    /****************************
     * xvector_like_variant_ref *
     ****************************/

    template <class ...V>
    class xvector_like_variant_ref : public xvector_like_variant_base<detail::xvector_like_variant_traits<false, true, V...>>
    {
    public:

        using traits_type = detail::xvector_like_variant_traits<false, true, V...>;
        using base_type = xvector_like_variant_base<traits_type>;
        using self_type = xvector_like_variant_ref<V...>;

        template <class OV,
                  XTL_REQUIRES(detail::is_unlike<OV, self_type>)>
        inline xvector_like_variant_ref(OV& v)
            : base_type(v)
        {
        }

        ~xvector_like_variant_ref() = default;

        xvector_like_variant_ref(const self_type&) = default;
        self_type& operator=(const self_type& rhs) = default;

        xvector_like_variant_ref(self_type&&) = default;
        self_type& operator=(self_type&&) = default;
    };

    /****************************
     * xvector_like_variant_cref *
     ****************************/

    template <class ...V>
    class xvector_like_variant_cref : public xvector_like_variant_base<detail::xvector_like_variant_traits<true, true, V...>>
    {
    public:

        using traits_type = detail::xvector_like_variant_traits<true, true, V...>;
        using base_type = xvector_like_variant_base<traits_type>;
        using self_type = xvector_like_variant_cref<V...>;

        template <class OV,
                  XTL_REQUIRES(detail::is_unlike<OV, self_type>)>
        inline xvector_like_variant_cref(OV& v)
            : base_type(v)
        {
        }

        ~xvector_like_variant_cref() = default;

        xvector_like_variant_cref(const self_type&) = default;
        xvector_like_variant_cref(self_type&&) = default;
    };

    /********************************************
     * xvector_like_variant_base implementation *
     ********************************************/

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
    inline bool xvector_like_variant_base<T>::empty() const
    {
        return xtl::visit([](const auto& arg) { return detail::unwrap(arg).empty(); }, m_storage);
    }

    template <class T>
    inline auto xvector_like_variant_base<T>::size() const -> size_type
    {
        return xtl::visit([](const auto& arg) { return detail::unwrap(arg).size(); }, m_storage);
    }

    template <class T>
    inline void xvector_like_variant_base<T>::resize(size_type size)
    {
        xtl::visit([size](auto& arg) { detail::unwrap(arg).resize(size); }, m_storage);
    }

    // Needs fix in xtensor
    /*template <class T>
    inline auto xvector_like_variant_base<T>::max_size() const -> size_type
    {
        return xtl::visit([](const auto& arg) { return detail::unwrap(arg).max_size(); }, m_storage);
    }

    template <class T>
    inline auto xvector_like_variant_base<T>::capacity() const -> size_type
    {
        return xtl::visit([](const auto& arg) { return detail::unwrap(arg).capacity(); }, m_storage);
    }

    template <class T>
    inline void xvector_like_variant_base<T>::reserve(size_type new_cap)
    {
        xtl::visit([new_cap](auto& arg) { detail::unwrap(arg).reserve(new_cap); }, m_storage);
    }

    template <class T>
    inline void xvector_like_variant_base<T>::shrink_to_fit()
    {
        xtl::visit([](auto& arg) { detail::unwrap(arg).shrink_to_fit(); }, m_storage);
    }

    template <class T>
    inline void xvector_like_variant_base<T>::clear()
    {
        xtl::visit([](auto& arg) { detail::unwrap(arg).clear(); }, m_storage);
    }*/

    template <class T>
    inline auto xvector_like_variant_base<T>::operator[](size_type i) -> reference
    {
        return xtl::visit([i](auto& arg)
        {
            return reference(xtl::closure(detail::unwrap(arg)[i]));
        },
        m_storage);
    }

    template <class T>
    inline auto xvector_like_variant_base<T>::operator[](size_type i) const -> const_reference
    {
        return xtl::visit([i](const auto& arg)
        {
            return const_reference(xtl::closure(detail::unwrap(arg)[i]));
        },
        m_storage);
    }

    // Needs a fix in xtensor
    /*
    template <class T>
    inline auto xvector_like_variant_base<T>::at(size_type i) -> reference
    {
        return xtl::visit([i](auto& arg)
        {
            return reference(xtl::closure(detail::unwrap(arg).at(i)));
        },
        m_storage);
    }

    template <class T>
    inline auto xvector_like_variant_base<T>::at(size_type i) const -> const_reference
    {
        return xtl::visit([i](const auto& arg)
        {
            return const_reference(xtl::closure(detail::unwrap(arg)[i]));
        },
        m_storage);
    }
    */

    template <class T>
    inline auto xvector_like_variant_base<T>::front() -> reference
    {
        return xtl::visit([](auto& arg)
        {
            return reference(xtl::closure(detail::unwrap(arg).front()));
        },
        m_storage);
    }

    template <class T>
    inline auto xvector_like_variant_base<T>::front() const -> const_reference
    {
        return xtl::visit([](const auto& arg)
        {
            return const_reference(xtl::closure(detail::unwrap(arg).front()));
        },
        m_storage);
    }

    template <class T>
    inline auto xvector_like_variant_base<T>::back() -> reference
    {
        return xtl::visit([](auto& arg)
        {
            return reference(xtl::closure(detail::unwrap(arg).back()));
        },
        m_storage);
    }

    template <class T>
    inline auto xvector_like_variant_base<T>::back() const -> const_reference
    {
        return xtl::visit([](const auto& arg)
        {
            return const_reference(xtl::closure(detail::unwrap(arg).back()));
        },
        m_storage);
    }

    template <class T>
    inline auto xvector_like_variant_base<T>::data() -> pointer
    {
        return xtl::visit([](auto& arg) { return pointer(detail::unwrap(arg).data()); }, m_storage);
    }

    template <class T>
    inline auto xvector_like_variant_base<T>::data() const -> const_pointer
    {
        return xtl::visit([](const auto& arg) { return const_pointer(detail::unwrap(arg).data()); }, m_storage);
    }

    template <class T>
    inline auto xvector_like_variant_base<T>::storage() -> storage_type&
    {
        return m_storage;
    }

    template <class T>
    inline auto xvector_like_variant_base<T>::storage() const -> const storage_type&
    {
        return m_storage;
    }

    template <class T>
    inline bool xvector_like_variant_base<T>::equal(const self_type& rhs) const
    {
        return m_storage == rhs.m_storage;
    }

    template <class T>
    inline bool xvector_like_variant_base<T>::less_than(const self_type& rhs) const
    {
        return m_storage < rhs.m_storage;
    }

    template <class T>
    inline bool operator==(const xvector_like_variant_base<T>& lhs, const xvector_like_variant_base<T>& rhs)
    {
        return lhs.equal(rhs);
    }

    template <class T>
    inline bool operator!=(const xvector_like_variant_base<T>& lhs, const xvector_like_variant_base<T>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class T>
    inline bool operator<(const xvector_like_variant_base<T>& lhs, const xvector_like_variant_base<T>& rhs)
    {
        return lhs.less_than(rhs);
    }

    template <class T>
    inline bool operator<=(const xvector_like_variant_base<T>& lhs, const xvector_like_variant_base<T>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class T>
    inline bool operator>(const xvector_like_variant_base<T>& lhs, const xvector_like_variant_base<T>& rhs)
    {
        return rhs < lhs;
    }

    template <class T>
    inline bool operator>=(const xvector_like_variant_base<T>& lhs, const xvector_like_variant_base<T>& rhs)
    {
        return !(lhs < rhs);
    }
    
    /***************************************
     * xvector_like_variant implementation *
     ***************************************/
}

#endif

