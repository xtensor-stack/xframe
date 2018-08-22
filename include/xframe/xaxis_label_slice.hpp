/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_LABEL_SLICE_HPP
#define XFRAME_XAXIS_LABEL_SLICE_HPP

#include <cmath>
#include <xtl/xvariant.hpp>
#include "xaxis_index_slice.hpp"
#include "xframe_config.hpp"

namespace xf
{
    /**************************
     * xlabel_slice_variant_t *
     **************************/

    template <class L>
    using xlabel_variant_t = xtl::mpl::cast_t<L, xtl::variant>;

    /***************
     * xaxis_range *
     ***************/

    template <class L>
    class xaxis_range
    {
    public:

        using value_type = xlabel_variant_t<L>;

        xaxis_range(const value_type& first, const value_type& last) noexcept;
        xaxis_range(value_type&& first, value_type&& last) noexcept;

        template <class A>
        using index_slice_type = xt::xrange<typename A::mapped_type>;

        template <class A>
        index_slice_type<A> build_index_slice(const A& axis) const;

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

        using value_type = xlabel_variant_t<L>;
        using size_type = std::size_t;

        xaxis_stepped_range(const value_type& first, const value_type& last, size_type step) noexcept;
        xaxis_stepped_range(value_type&& first, value_type&& last, size_type step) noexcept;

        template <class A>
        using index_slice_type = xt::xstepped_range<typename A::mapped_type>;

        template <class A>
        index_slice_type<A> build_index_slice(const A& axis) const;

    private:

        value_type m_first;
        value_type m_last;
        size_type m_step;
    };

    /*************
     * xaxis_all *
     *************/

    class xaxis_all
    {
    public:

        template <class A>
        using index_slice_type = xt::xall<typename A::mapped_type>;

        template <class A>
        index_slice_type<A> build_index_slice(const A& axis) const;
    };

    /***************
    * xaxis_slice *
    ***************/

    template <class L = DEFAULT_LABEL_LIST>
    class xaxis_slice
    {
    public:

        using squeeze_type = xlabel_variant_t<L>;
        using storage_type = xtl::variant<xaxis_range<L>, xaxis_stepped_range<L>, xaxis_all, squeeze_type>;

        xaxis_slice() = default;
        template <class V>
        xaxis_slice(const V& slice);
        template <class V>
        xaxis_slice(V&& slice);

        template <class A>
        using index_slice_type = xaxis_index_slice<typename A::mapped_type>;

        template <class A>
        index_slice_type<A> build_index_slice(const A& axis) const;

        const squeeze_type* get_squeeze() const noexcept;

    private:

        storage_type m_data;
    };

    template <class L = DEFAULT_LABEL_LIST>
    xaxis_slice<L> range(const xlabel_variant_t<L>& first, const xlabel_variant_t<L>& last);

    template <class L = DEFAULT_LABEL_LIST>
    xaxis_slice<L> range(xlabel_variant_t<L>&& first, xlabel_variant_t<L>&& last);

    template <class S, class L = DEFAULT_LABEL_LIST>
    xaxis_slice<L> range(const xlabel_variant_t<L>& first, const xlabel_variant_t<L>& last, S step);

    template <class S, class L = DEFAULT_LABEL_LIST>
    xaxis_slice<L> range(xlabel_variant_t<L>&& first, xlabel_variant_t<L>&& last, S step);

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
    inline auto xaxis_range<V>::build_index_slice(const A& axis) const -> index_slice_type<A>
    {
        return index_slice_type<A>(axis[m_first], axis[m_last] + 1);
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
    inline auto xaxis_stepped_range<V>::build_index_slice(const A& axis) const -> index_slice_type<A>
    {
        return index_slice_type<A>(axis[m_first], axis[m_last] + 1, m_step);
    }

    /****************************
     * xaxis_all implementation *
     ****************************/

    template <class A>
    inline auto xaxis_all::build_index_slice(const A& axis) const -> index_slice_type<A>
    {
        return index_slice_type<A>(axis.size());
    }

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
    inline auto xaxis_slice<L>::build_index_slice(const A& axis) const -> index_slice_type<A>
    {
        return xtl::visit(
            xtl::make_overload([&axis](const auto& arg) { return index_slice_type<A>(arg.build_index_slice(axis)); },
                [&axis](const squeeze_type&) -> index_slice_type<A> { throw std::runtime_error("build_islice forbidden for squeeze"); }),
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
    inline xaxis_slice<L> range(const xlabel_variant_t<L>& first, const xlabel_variant_t<L>& last)
    {
        return xaxis_slice<L>(xaxis_range<L>(first, last));
    }

    template <class L>
    inline xaxis_slice<L> range(xlabel_variant_t<L>&& first, xlabel_variant_t<L>&& last)
    {
        return xaxis_slice<L>(xaxis_range<L>(std::move(first), std::move(last)));
    }

    template <class S, class L>
    inline xaxis_slice<L> range(const xlabel_variant_t<L>& first, const xlabel_variant_t<L>& last, S step)
    {
        return xaxis_slice<L>(xaxis_stepped_range<L>(first, last, step));
    }

    template <class S, class L>
    inline xaxis_slice<L> range(xlabel_variant_t<L>&& first, xlabel_variant_t<L>&& last, S step)
    {
        return xaxis_slice<L>(xaxis_stepped_range<L>(std::move(first), std::move(last), step));
    }
}

#endif
