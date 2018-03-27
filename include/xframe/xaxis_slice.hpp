/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_SLICE_HPP
#define XFRAME_XAXIS_SLICE_HPP

#include <cmath>
#include <xtl/xvariant.hpp>

namespace xf
{
    // TODO: move additional features to slices of xtensor
    // and remove xaxis_iXXX

    /***************
     * xaxis_range *
     ***************/

    template <class T>
    class xaxis_irange
    {
    public:

        using size_type = T;

        xaxis_irange() = default;
        xaxis_irange(size_type min_val, size_type max_val) noexcept;

        size_type size() const noexcept;
        bool contains(size_type i) const noexcept;

        size_type operator()(size_type i) const noexcept;
        size_type step_size(size_type i, size_type n = 1) const noexcept;

    private:

        size_type m_min;
        size_type m_size;
    };

    template <class V>
    class xaxis_range
    {
    public:

        using value_type = V;

        xaxis_range(value_type first, value_type last);

        template <class A>
        xaxis_irange<typename A::mapped_type> build_islice(const A& axis) const;

    private:

        value_type m_first;
        value_type m_last;
    };

    /***********************
     * xaxis_stepped_range *
     ***********************/

    template <class T>
    class xaxis_istepped_range
    {
    public:

        using size_type = T;

        xaxis_istepped_range() = default;
        xaxis_istepped_range(size_type min_val, size_type max_val, size_type step) noexcept;

        size_type size() const noexcept;
        bool contains(size_type i) const noexcept;

        size_type operator()(size_type i) const noexcept;
        size_type step_size(size_type i, size_type n = 1) const noexcept;

    private:

        size_type m_min;
        size_type m_step;
        size_type m_size;
    };

    /*************
     * xaxis_all *
     *************/

    template <class T>
    class xaxis_iall
    {
    public:

        using size_type = T;

        xaxis_iall() = default;
        explicit xaxis_iall(size_type size) noexcept;

        size_type size() const noexcept;
        bool contains(size_type i) const noexcept;

        size_type operator()(size_type i) const noexcept;
        size_type step_size(size_type i, size_type n = 1) const noexcept;

    private:

        size_type m_size;
    };

    /****************
     * xaxis_islice *
     ****************/

    template <class T>
    class xaxis_islice
    {
    public:

        using slice_type = xtl::variant<xaxis_irange<T>, xaxis_istepped_range<T>, xaxis_iall<T>>;
        using size_type = T;

        xaxis_islice() = default;
        template <class S>
        xaxis_islice(S&& slice) noexcept;

        size_type size() const noexcept;
        bool contains(size_type i) const noexcept;

        size_type operator()(size_type i) const noexcept;
        size_type step_size(size_type i, size_type n = 1) const noexcept;

    private:

        slice_type m_slice;
    };

    /******************************
     * xaxis_range implementation *
     ******************************/

    template <class T>
    inline xaxis_irange<T>::xaxis_irange(size_type min_val, size_type max_val) noexcept
        : m_min(min_val), m_size(max_val - min_val)
    {
    }

    template <class T>
    inline auto xaxis_irange<T>::size() const noexcept -> size_type
    {
        return m_size;
    }

    template <class T>
    inline bool xaxis_irange<T>::contains(size_type i) const noexcept
    {
        return i >= m_min && i < m_min + m_size;
    }

    template <class T>
    inline auto xaxis_irange<T>::operator()(size_type i) const noexcept -> size_type
    {
        return m_min + i;
    }

    template <class T>
    inline auto xaxis_irange<T>::step_size(size_type i, size_type n) const noexcept -> size_type
    {
        return n;
    }

    /**************************************
     * xaxis_stepped_range implementation *
     **************************************/

    template <class T>
    inline xaxis_istepped_range<T>::xaxis_istepped_range(size_type min_val, size_type max_val, size_type step) noexcept
        : m_min(min_val), m_step(step), m_size(size_type(std::ceil(double(max_val - min_val) / double(step))))
    {
    }

    template <class T>
    inline auto xaxis_istepped_range<T>::size() const noexcept -> size_type
    {
        return m_size;
    }

    template <class T>
    inline bool xaxis_istepped_range<T>::contains(size_type i) const noexcept
    {
        return i >= m_min && i < m_min + m_size * m_step && ((i - m_min) % m_step == 0);
    }

    template <class T>
    inline auto xaxis_istepped_range<T>::operator()(size_type i) const noexcept -> size_type
    {
        return m_min + i * m_step;
    }
    
    template <class T>
    inline auto xaxis_istepped_range<T>::step_size(size_type i, size_type n) const noexcept -> size_type
    {
        return m_min + (i * m_step) * n;
    }

    /****************************
     * xaxis_all implementation *
     ****************************/

    template <class T>
    inline xaxis_iall<T>::xaxis_iall(size_type size) noexcept
        : m_size(size)
    {
    }

    template <class T>
    inline auto xaxis_iall<T>::size() const noexcept -> size_type
    {
        return m_size;
    }

    template <class T>
    inline bool xaxis_iall<T>::contains(size_type i) const noexcept
    {
        return i < m_size;
    }

    template <class T>
    inline auto xaxis_iall<T>::operator()(size_type i) const noexcept -> size_type
    {
        return i;
    }

    template <class T>
    inline auto xaxis_iall<T>::step_size(size_type i, size_type n) const noexcept -> size_type
    {
        return n;
    }

    /*******************************
     * xaxis_islice implementation *
     *******************************/

    template <class T>
    template <class S>
    inline xaxis_islice<T>::xaxis_islice(S&& slice) noexcept
        : m_slice(std::forward<S>(slice))
    {
    }

    template <class T>
    inline auto xaxis_islice<T>::size() const noexcept -> size_type
    {
        return xtl::visit([](auto&& arg) { return arg.size(); }, m_slice);
    }

    template <class T>
    inline bool xaxis_islice<T>::contains(size_type i) const noexcept
    {
        return xtl::visit([i](auto&& arg) { return arg.contains(i); }, m_slice);
    }

    template <class T>
    inline auto xaxis_islice<T>::operator()(size_type i) const noexcept -> size_type
    {
        return xtl::visit([i](auto&& arg) { return arg(i); }, m_slice);
    }

    template <class T>
    inline auto xaxis_islice<T>::step_size(size_type i, size_type n) const noexcept -> size_type
    {
        return xtl::visit([i, n](auto&& arg) { return arg.step_size(i, n); }, m_slice);
    }
}

#endif
