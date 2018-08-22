/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_INDEX_SLICE_HPP
#define XFRAME_XAXIS_INDEX_SLICE_HPP

#include <cstddef>
#include <type_traits>
#include "xtl/xvariant.hpp"
#include "xtensor/xstorage.hpp"
#include "xtensor/xslice.hpp"

namespace xf
{

    /*********************
     * xaxis_index_slice *
     *********************/

    template <class T>
    class xaxis_index_slice;

    namespace detail
    {
        template <class T>
        struct is_xaxis_index_slice : std::false_type
        {
        };

        template <class T>
        struct is_xaxis_index_slice<xaxis_index_slice<T>> : std::true_type
        {
        };

        template <class S>
        using disable_xaxis_index_slice_t = std::enable_if_t<!is_xaxis_index_slice<std::decay_t<S>>::value, void>;
    }

    template <class T>
    class xaxis_index_slice
    {
    public:

        using self_type = xaxis_index_slice<T>;
        using size_type = T;
        using storage_type = xtl::variant<xt::xrange<T>,
                                          xt::xstepped_range<T>,
                                          xt::xkeep_slice<T>,
                                          xt::xdrop_slice<T>,
                                          xt::xall<T>>;

        xaxis_index_slice() = default;
        template <class S, typename = detail::disable_xaxis_index_slice_t<S>>
        xaxis_index_slice(S&& slice) noexcept;

        size_type size() const noexcept;
        bool contains(size_type i) const noexcept;

        size_type operator()(size_type i) const noexcept;
        size_type step_size(size_type i, size_type n = 1) const noexcept;

        size_type revert_index(size_type i) const noexcept;

        bool operator==(const self_type& rhs) const noexcept;
        bool operator!=(const self_type& rhs) const noexcept;

    private:

        storage_type m_slice;
    };

    /************************************
     * xaxis_index_slice implementation *
     ************************************/

    template <class T>
    template <class S, typename>
    inline xaxis_index_slice<T>::xaxis_index_slice(S&& slice) noexcept
        : m_slice(std::forward<S>(slice))
    {
    }

    template <class T>
    inline auto xaxis_index_slice<T>::size() const noexcept -> size_type
    {
        return xtl::visit([](auto&& arg) { return arg.size(); }, m_slice);
    }

    template <class T>
    inline bool xaxis_index_slice<T>::contains(size_type i) const noexcept
    {
        return xtl::visit([i](auto&& arg) { return arg.contains(i); }, m_slice);
    }

    template <class T>
    inline auto xaxis_index_slice<T>::operator()(size_type i) const noexcept -> size_type
    {
        return xtl::visit([i](auto&& arg) { return arg(i); }, m_slice);
    }

    template <class T>
    inline auto xaxis_index_slice<T>::step_size(size_type i, size_type n) const noexcept -> size_type
    {
        return xtl::visit([i, n](auto&& arg) { return arg.step_size(i, n); }, m_slice);
    }

    template <class T>
    inline auto xaxis_index_slice<T>::revert_index(size_type i) const noexcept -> size_type
    {
        return xtl::visit([i](auto&& arg) { return arg.revert_index(i); }, m_slice);
    }

    template <class T>
    inline bool xaxis_index_slice<T>::operator==(const self_type& rhs) const noexcept
    {
        return m_slice == rhs.m_slice;
    }

    template <class T>
    inline bool xaxis_index_slice<T>::operator!=(const self_type& rhs) const noexcept
    {
        return !(*this == rhs);
    }
}

#endif
