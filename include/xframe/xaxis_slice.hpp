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
#include "xtensor/xslice.hpp"
#include "xframe_config.hpp"

namespace xf
{

    /****************
     * xaxis_islice *
     ****************/

    template <class T>
    class xaxis_islice
    {
    public:

        using slice_type = xtl::variant<xt::xrange<T>, xt::xstepped_range<T>, xt::xall<T>>;
        using size_type = T;
        using self_type = xaxis_islice<T>;

        xaxis_islice() = default;
        template <class S>
        xaxis_islice(S&& slice) noexcept;

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
     * slice_variant_t *
     *******************/

    template <class L>
    using slice_variant_t = xtl::mpl::cast_t<L, xtl::variant>;

    /***************
     * xaxis_range *
     ***************/
    
    template <class L>
    class xaxis_range
    {
    public:

        using value_type = slice_variant_t<L>;

        xaxis_range(const value_type& first, const value_type& last) noexcept;
        xaxis_range(value_type&& first, value_type&& last) noexcept;

        template <class A>
        using slice_type = xt::xrange<typename A::mapped_type>;

        template <class A>
        slice_type<A> build_islice(const A& axis) const;

    private:

        value_type m_first;
        value_type m_last;
    };

    /***********************
     * xaxis_stepped_range *
     ***********************/

    template <class L>
    class xaxis_stepped_range
    {
    public:

        using value_type = slice_variant_t<L>;
        using size_type = std::size_t;

        xaxis_stepped_range(const value_type& first, const value_type& last, size_type step) noexcept;
        xaxis_stepped_range(value_type&& first, value_type&& last, size_type step) noexcept;

        template <class A>
        using slice_type = xt::xstepped_range<typename A::mapped_type>;

        template <class A>
        slice_type<A> build_islice(const A& axis) const;

    private:

        value_type m_first;
        value_type m_last;
        size_type m_step;
    };

    /*****************
     * xaxis_squeeze *
     *****************/

    /*template <class L>
    class xaxis_squeeze
    {
    public:

        using value_type = slice_variant_t<L>;

        xaxis_squeeze(const value_type& squeeze);
        xaxis_squeeze(value_type&& squeeze);

        template <class A>
        using slice_type = typename A::mapped_type;

        template <class A>
        slice_type<A> build_islice(const A& axis) const;

    private:

        value_type m_squeeze;
    };*/

    /***************
     * xaxis_slice *
     ***************/

    template <class L = DEFAULT_LABEL_LIST>
    class xaxis_slice
    {
    public:

        using squeeze_type = slice_variant_t<L>;
        using storage_type = xtl::variant<xaxis_range<L>, xaxis_stepped_range<L>, squeeze_type>;

        xaxis_slice() = default;
        template <class V>
        xaxis_slice(const V& slice);
        template <class V>
        xaxis_slice(V&& slice);

        template <class A>
        using slice_type = xaxis_islice<typename A::mapped_type>;

        template <class A>
        slice_type<A> build_islice(const A& axis) const;

        const squeeze_type* get_squeeze() const noexcept;

    private:

        storage_type m_data;
    };

    /********************
     * helper functions *
     ********************/

    template <class L = DEFAULT_LABEL_LIST>
    xaxis_slice<L> range(const slice_variant_t<L>& first, const slice_variant_t<L>& last);

    template <class L = DEFAULT_LABEL_LIST>
    xaxis_slice<L> range(slice_variant_t<L>&& first, slice_variant_t<L>&& last);

    template <class S, class L = DEFAULT_LABEL_LIST>
    xaxis_slice<L> range(const slice_variant_t<L>& first, const slice_variant_t<L>& last, S step);

    template <class S, class L = DEFAULT_LABEL_LIST>
    xaxis_slice<L> range(slice_variant_t<L>&& first, slice_variant_t<L>&& last, S step);

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

    template <class T>
    inline auto xaxis_islice<T>::revert_index(size_type i) const noexcept -> size_type
    {
        return xtl::visit([i](auto&& arg) { return arg.revert_index(i); }, m_slice);
    }

    template <class T>
    inline bool xaxis_islice<T>::operator==(const self_type& rhs) const noexcept
    {
        return m_slice == rhs.m_slice;
    }

    template <class T>
    inline bool xaxis_islice<T>::operator!=(const self_type& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    /******************************
     * xaxis_range implementation *
     ******************************/

    template <class V>
    inline xaxis_range<V>::xaxis_range(const value_type& first, const value_type& last) noexcept
        : m_first(first), m_last(last)
    {
    }

    template <class V>
    inline xaxis_range<V>::xaxis_range(value_type&& first, value_type&& last) noexcept
        : m_first(std::move(first)), m_last(std::move(last))
    {
    }

    template <class V>
    template <class A>
    inline auto xaxis_range<V>::build_islice(const A& axis) const -> slice_type<A>
    {
        return slice_type<A>(axis[m_first], axis[m_last] + 1);
    }

    /**************************************
     * xaxis_stepped_range implementation *
     **************************************/

    template <class V>
    inline xaxis_stepped_range<V>::xaxis_stepped_range(const value_type& first, const value_type& last, size_type step) noexcept
        : m_first(first), m_last(last), m_step(step)
    {
    }

    template <class V>
    inline xaxis_stepped_range<V>::xaxis_stepped_range(value_type&& first, value_type&& last, size_type step) noexcept
        : m_first(std::move(first)), m_last(std::move(last)), m_step(step)
    {
    }

    template <class V>
    template <class A>
    inline auto xaxis_stepped_range<V>::build_islice(const A& axis) const -> slice_type<A>
    {
        return slice_type<A>(axis[m_first], axis[m_last] + 1, m_step);
    }

    /******************************
     * xaxis_slice implementation *
     ******************************/

    /*template <class L>
    inline xaxis_squeeze<L>::xaxis_squeeze(const value_type& squeeze)
        : m_squeeze(squeeze)
    {
    }

    template <class L>
    inline xaxis_squeeze<L>::xaxis_squeeze(value_type&& squeeze)
        : m_squeeze(std::move(squeeze))
    {
    }

    template <class L>
    template <class A>
    inline auto xaxis_squeeze<L>::build_islice(const A& axis) const -> slice_type<A>
    {
        return axis[m_squeeze];
    }*/

    /******************************
     * xaxis_slice implementation *
     ******************************/

    template <class L>
    template <class V>
    inline xaxis_slice<L>::xaxis_slice(const V& slice)
        : m_data(slice)
    {
    }

    template <class L>
    template <class V>
    inline xaxis_slice<L>::xaxis_slice(V&& slice)
        : m_data(std::move(slice))
    {
    }

    template <class L>
    template <class A>
    inline auto xaxis_slice<L>::build_islice(const A& axis) const -> slice_type<A>
    {
        return xtl::visit(
            xtl::make_overload([&axis](const auto& arg) { return slice_type<A>(arg.build_islice(axis)); },
                               [&axis](const squeeze_type&) -> slice_type<A> { throw std::runtime_error("build_islice forbidden for squeeze"); }),
            m_data);
    }

    template <class L>
    inline auto xaxis_slice<L>::get_squeeze() const noexcept -> const squeeze_type*
    {
        return xtl::get_if<squeeze_type>(&m_data);
    }

    /***********************************
     * helper functions implementation *
     ***********************************/

    template <class L>
    inline xaxis_slice<L> range(const slice_variant_t<L>& first, const slice_variant_t<L>& last)
    {
        return xaxis_slice<L>(xaxis_range<L>(first, last));
    }

    template <class L>
    inline xaxis_slice<L> range(slice_variant_t<L>&& first, slice_variant_t<L>&& last)
    {
        return xaxis_slice<L>(xaxis_range<L>(std::move(first), std::move(last)));
    }

    template <class S, class L>
    inline xaxis_slice<L> range(const slice_variant_t<L>& first, const slice_variant_t<L>& last, S step)
    {
        return xaxis_slice<L>(xaxis_stepped_range<L>(first, last, step));
    }

    template <class S, class L>
    inline xaxis_slice<L> range(slice_variant_t<L>&& first, slice_variant_t<L>&& last, S step)
    {
        return xaxis_slice<L>(xaxis_stepped_range<L>(std::move(first), std::move(last), step));
    }
}

#endif
