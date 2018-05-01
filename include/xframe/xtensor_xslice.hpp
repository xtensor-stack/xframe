/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

// The content of this file should be moved to xtensor / xslice.hpp

#ifndef XFRAME_XTENSOR_XSLICE_HPP
#define XFRAME_XTENSOR_XSLICE_HPP

#include <cstddef>
#include <type_traits>
#include "xtl/xvariant.hpp"
#include "xtensor/xslice.hpp"

namespace xt
{

    /******************
     * xslice_variant *
     ******************/

    template <class T>
    class xslice_variant;

    namespace detail
    {
        template <class T>
        struct is_xslice_variant : std::false_type
        {
        };

        template <class T>
        struct is_xslice_variant<xslice_variant<T>> : std::true_type
        {
        };

        template <class S>
        using disable_xslice_variant_t = std::enable_if_t<!is_xslice_variant<std::decay_t<S>>::value, void>;
    }

    template <class T>
    class xslice_variant
    {
    public:

        using slice_type = xtl::variant<xrange<T>, xstepped_range<T>, xall<T>>;
        using size_type = T;
        using self_type = xslice_variant<T>;

        xslice_variant() = default;
        template <class S, typename = detail::disable_xslice_variant_t<S>>
        xslice_variant(S&& slice) noexcept;

        template <class S, typename = std::enable_if_t<std::is_convertible<T, S>::value, void>>
        operator xslice_variant<S>() const noexcept;

        size_type size() const noexcept;
        bool contains(size_type i) const noexcept;

        size_type operator()(size_type i) const noexcept;
        size_type step_size(size_type i, size_type n = 1) const noexcept;

        size_type revert_index(size_type i) const noexcept;

        bool operator==(const self_type& rhs) const noexcept;
        bool operator!=(const self_type& rhs) const noexcept;

    private:

        slice_type m_slice;
    };

    /*******************
     * xslice_extended *
     *******************/

    template <class T>
    class xslice_extended;

    namespace detail
    {
        template <class T>
        struct is_xslice_extended : std::false_type
        {
        };

        template <class T>
        struct is_xslice_extended<xslice_extended<T>> : std::true_type
        {
        };

        template <class S>
        using disable_xslice_extended_t = std::enable_if_t<!is_xslice_extended<std::decay_t<S>>::value, void>;
    }

    template <class T>
    class xslice_extended
    {
    public:

        using self_type = xslice_extended<T>;
        using all_type = xall_tag;
        using squeeze_type = T;
        using slice_type = xslice_variant<T>;
        using storage_type = xtl::variant<all_type, squeeze_type, slice_type>;

        xslice_extended() = default;
        template <class S, typename = detail::disable_xslice_extended_t<S>>
        xslice_extended(S&& slice);

        template <class S, typename = std::enable_if_t<std::is_convertible<S, T>::value, void>>
        operator xslice_extended<S>() const noexcept;

        const all_type* get_all() const noexcept;
        const squeeze_type* get_squeeze() const noexcept;
        const slice_type* get_slice() const noexcept;

    private:

        storage_type m_data;
    };

    /********************
     * helper functions *
     ********************/

    // TODO: check if these functions are still required

    template <class T>
    xslice_variant<T> variant_range(T first, T last);

    template <class T>
    xslice_variant<T> variant_range(T first, T last, T step);

    /*************************
    * conversions functions *
    *************************/

    namespace detail
    {
        template <class S, class T>
        inline xslice_variant<S> convert_slice(const xt::xrange<T>& rhs)
        {
            S smin = static_cast<S>(rhs(T(0)));
            S smax = smin + static_cast<S>(rhs.size());
            return xt::xrange<S>(smin, smax);
        }

        template <class S, class T>
        inline xslice_variant<S> convert_slice(const xt::xstepped_range<T>& rhs)
        {
            S smin = static_cast<S>(rhs(T(0)));
            S sstep = static_cast<S>(rhs.step_size());
            S smax = smin + static_cast<S>(rhs.size()) * sstep;
            return xt::xstepped_range<S>(smin, smax, sstep);
        }

        template <class S, class T>
        inline xslice_variant<S> convert_slice(const xt::xall<T>& rhs)
        {
            return xt::xall<S>(static_cast<S>(rhs.size()));
        }
    }

    /*********************************
     * xslice_variant implementation *
     *********************************/

    template <class T>
    template <class S, typename>
    inline xslice_variant<T>::xslice_variant(S&& slice) noexcept
        : m_slice(std::forward<S>(slice))
    {
    }

    template <class T>
    template <class S, typename>
    inline xslice_variant<T>::operator xslice_variant<S>() const noexcept
    {
        return xtl::visit([](const auto& arg) { return detail::convert_slice<S>(arg); }, m_slice);
    }

    template <class T>
    inline auto xslice_variant<T>::size() const noexcept -> size_type
    {
        return xtl::visit([](auto&& arg) { return arg.size(); }, m_slice);
    }

    template <class T>
    inline bool xslice_variant<T>::contains(size_type i) const noexcept
    {
        return xtl::visit([i](auto&& arg) { return arg.contains(i); }, m_slice);
    }

    template <class T>
    inline auto xslice_variant<T>::operator()(size_type i) const noexcept -> size_type
    {
        return xtl::visit([i](auto&& arg) { return arg(i); }, m_slice);
    }

    template <class T>
    inline auto xslice_variant<T>::step_size(size_type i, size_type n) const noexcept -> size_type
    {
        return xtl::visit([i, n](auto&& arg) { return arg.step_size(i, n); }, m_slice);
    }

    template <class T>
    inline auto xslice_variant<T>::revert_index(size_type i) const noexcept -> size_type
    {
        return xtl::visit([i](auto&& arg) { return arg.revert_index(i); }, m_slice);
    }

    template <class T>
    inline bool xslice_variant<T>::operator==(const self_type& rhs) const noexcept
    {
        return m_slice == rhs.m_slice;
    }

    template <class T>
    inline bool xslice_variant<T>::operator!=(const self_type& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    /**********************************
     * xslice_extended implementation *
     **********************************/

    namespace detail
    {
        template <class S, class T>
        inline xslice_extended<S> convert_slice(typename xslice_extended<T>::all_type)
        {
            return xall_tag();
        }

        template <class S, class T>
        inline xslice_extended<S> convert_slice(const typename xslice_extended<T>::squeeze_type& squeeze)
        {
            return static_cast<S>(squeeze);
        }

        template <class S, class T>
        inline xslice_extended<S> convert_slice(const xt::xslice_variant<T>& slice)
        {
            return xslice_variant<S>(slice);
        }
    }

    template <class T>
    template <class S, typename>
    inline xslice_extended<T>::xslice_extended(S&& slice)
        : m_data(std::forward<S>(slice))
    {
    }

    template <class T>
    template <class S, typename>
    inline xslice_extended<T>::operator xslice_extended<S>() const noexcept
    {
        return xtl::visit([](const auto& arg) { return detail::convert_slice<S, T>(arg); }, m_data);
    }

    template <class T>
    inline auto xslice_extended<T>::get_all() const noexcept -> const all_type*
    {
        return xtl::get_if<all_type>(&m_data);
    }

    template <class T>
    inline auto xslice_extended<T>::get_squeeze() const noexcept -> const squeeze_type*
    {
        return xtl::get_if<squeeze_type>(&m_data);
    }

        template <class T>
    inline auto xslice_extended<T>::get_slice() const noexcept -> const slice_type*
    {
        return xtl::get_if<slice_type>(&m_data);
    }

    /***********************************
     * helper functions implementation *
     ***********************************/

    template <class T>
    inline xslice_variant<T> variant_range(T first, T last)
    {
        return xslice_variant<T>(xrange<T>(first, last));
    }

    template <class T>
    inline xslice_variant<T> variant_range(T first, T last, T step)
    {
        return xslice_variant<T>(xstepped_range<T>(first, last, step));
    }
}

#endif
