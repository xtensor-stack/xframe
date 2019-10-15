/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
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

    /********************
     * xaxis_keep_slice *
     *******************/

    template <class L>
    class xaxis_keep_slice;

    namespace detail
    {
        template <class T>
        struct is_xaxis_keep_slice : std::false_type
        {
        };

        template <class L>
        struct is_xaxis_keep_slice<xaxis_keep_slice<L>>
            : std::true_type
        {
        };

        template <class T>
        using disable_xaxis_keep_slice_t = std::enable_if_t<!is_xaxis_keep_slice<std::decay_t<T>>::value, void>;

        template <class T>
        using enable_xaxis_keep_slice_t = std::enable_if_t<is_xaxis_keep_slice<std::decay_t<T>>::value, void>;
    }

    template <class L>
    class xaxis_keep_slice
    {
    public:

        using value_type = xlabel_variant_t<L>;
        using container_type = xt::svector<value_type>;

        template <class C, typename = detail::disable_xaxis_keep_slice_t<C>>
        explicit xaxis_keep_slice(C& cont);
        explicit xaxis_keep_slice(container_type&& cont);

        template <class A>
        using index_slice_type = xt::xkeep_slice<typename A::mapped_type>;

        template <class A>
        index_slice_type<A> build_index_slice(const A& axis) const;

    private:

        container_type m_labels;
    };

    /********************
     * xaxis_drop_slice *
     ********************/

    template <class L>
    class xaxis_drop_slice;

    namespace detail
    {
        template <class T>
        struct is_axis_drop_slice : std::false_type
        {
        };

        template <class L>
        struct is_axis_drop_slice<xaxis_drop_slice<L>>
            : std::true_type
        {
        };

        template <class T>
        using disable_xaxis_drop_slice_t = std::enable_if_t<!is_axis_drop_slice<std::decay_t<T>>::value, void>;

        template <class T>
        using enable_xaxis_drop_slice_t = std::enable_if_t<is_axis_drop_slice<std::decay_t<T>>::value, void>;
    }

    template <class L>
    class xaxis_drop_slice
    {
    public:

        using value_type = xlabel_variant_t<L>;
        using container_type = xt::svector<value_type>;

        template <class C, typename = detail::disable_xaxis_drop_slice_t<C>>
        explicit xaxis_drop_slice(C& cont);
        explicit xaxis_drop_slice(container_type&& cont);

        template <class A>
        using index_slice_type = xt::xdrop_slice<typename A::mapped_type>;

        template <class A>
        index_slice_type<A> build_index_slice(const A& axis) const;

    private:

        container_type m_labels;
    };

    /***************
     * xaxis_slice *
     ***************/

    template <class L = XFRAME_DEFAULT_LABEL_LIST>
    class xaxis_slice
    {
    public:

        using squeeze_type = xlabel_variant_t<L>;
        using storage_type = xtl::variant<xaxis_range<L>,
                                          xaxis_stepped_range<L>,
                                          xaxis_keep_slice<L>,
                                          xaxis_drop_slice<L>,
                                          xaxis_all,
                                          squeeze_type>;

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

    template <class L = XFRAME_DEFAULT_LABEL_LIST>
    xaxis_slice<L> range(const xlabel_variant_t<L>& first, const xlabel_variant_t<L>& last);

    template <class L = XFRAME_DEFAULT_LABEL_LIST>
    xaxis_slice<L> range(xlabel_variant_t<L>&& first, xlabel_variant_t<L>&& last);

    template <class S, class L = XFRAME_DEFAULT_LABEL_LIST>
    xaxis_slice<L> range(const xlabel_variant_t<L>& first, const xlabel_variant_t<L>& last, S step);

    template <class S, class L = XFRAME_DEFAULT_LABEL_LIST>
    xaxis_slice<L> range(xlabel_variant_t<L>&& first, xlabel_variant_t<L>&& last, S step);

    xaxis_all all() noexcept;

    namespace detail
    {
        template <class T, typename = xt::void_t<>>
        struct has_size : std::false_type
        {
        };

        template <class T>
        struct has_size<T, xt::void_t<decltype(std::declval<T>().size())>>
            : std::true_type
        {
        };

        template <class T, class R>
        using enable_container_t = std::enable_if_t<has_size<T>::value, R>;

        template <class T, class R>
        using disable_container_t = std::enable_if_t<!has_size<T>::value, R>;
    }

    template <class L = XFRAME_DEFAULT_LABEL_LIST, class T>
    detail::enable_container_t<T, xaxis_slice<L>> keep(T&& indices);

    template <class L = XFRAME_DEFAULT_LABEL_LIST, class T>
    detail::disable_container_t<T, xaxis_slice<L>> keep(T index);

    template <class L = XFRAME_DEFAULT_LABEL_LIST, class T0, class T1, class... Args>
    xaxis_slice<L> keep(T0 t0, T1 t1, Args... args);

    template <class L = XFRAME_DEFAULT_LABEL_LIST, class T>
    detail::enable_container_t<T, xaxis_slice<L>> drop(T&& indices);

    template <class L = XFRAME_DEFAULT_LABEL_LIST, class T>
    detail::disable_container_t<T, xaxis_slice<L>> drop(T index);

    template <class L = XFRAME_DEFAULT_LABEL_LIST, class T0, class T1, class... Args>
    xaxis_slice<L> drop(T0 t0, T1 t1, Args... args);

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

    /*****************************
     * xaxis_keep_implementation *
     *****************************/

    template <class L>
    template <class C, typename>
    inline xaxis_keep_slice<L>::xaxis_keep_slice(C& cont)
        : m_labels(cont.begin(), cont.end())
    {
    }

    template <class L>
    inline xaxis_keep_slice<L>::xaxis_keep_slice(container_type&& cont)
        : m_labels(std::move(cont))
    {
    }

    template <class L>
    template <class A>
    inline auto xaxis_keep_slice<L>::build_index_slice(const A& axis) const -> index_slice_type<A>
    {
        using index_container_type = typename index_slice_type<A>::container_type;
        index_container_type c(m_labels.size());
        std::transform(m_labels.cbegin(), m_labels.cend(), c.begin(), [&axis](const auto& arg) { return axis[arg]; });
        index_slice_type<A> res(std::move(c));
        res.normalize(axis.size());
        return res;
    }

    /****************************
     * xaxis_drop_implemenation *
     ****************************/

    template <class L>
    template <class C, typename>
    inline xaxis_drop_slice<L>::xaxis_drop_slice(C& cont)
        : m_labels(cont.begin(), cont.end())
    {
    }

    template <class L>
    inline xaxis_drop_slice<L>::xaxis_drop_slice(container_type&& cont)
        : m_labels(std::move(cont))
    {
    }

    template <class L>
    template <class A>
    inline auto xaxis_drop_slice<L>::build_index_slice(const A& axis) const -> index_slice_type<A>
    {
        using index_container_type = typename index_slice_type<A>::container_type;
        index_container_type c(m_labels.size());
        std::transform(m_labels.cbegin(), m_labels.cend(), c.begin(), [&axis](const auto& arg) { return axis[arg]; });
        index_slice_type<A> res(std::move(c));
        res.normalize(axis.size());
        return res;
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
            xtl::make_overload(
                [&axis](const auto& arg) { return index_slice_type<A>(arg.build_index_slice(axis)); },
                [&axis](const squeeze_type&) -> index_slice_type<A> { throw std::runtime_error("build_islice forbidden for squeeze"); }
            ),
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

    namespace detail
    {
        template <template <class> class R, class L, class T>
        inline detail::enable_container_t<T, xaxis_slice<L>>
        common_drop_keep(T&& indices)
        {
            R<L> slice(std::forward<T>(indices));
            return xaxis_slice<L>(std::move(slice));
        }

        template <template <class> class R, class L, class T>
        inline detail::disable_container_t<T, xaxis_slice<L>>
        common_drop_keep(T index)
        {
            using slice_type = R<L>;
            using container_type = typename slice_type::container_type;
            using value_type = typename slice_type::value_type;
            container_type tmp = { value_type(index) };
            return xaxis_slice<L>(slice_type(std::move(tmp)));
        }

        template <template <class> class R, class L, class T0, class T1, class... Args>
        inline xaxis_slice<L>
        common_drop_keep(T0 t0, T1 t1, Args... args)
        {
            using slice_type = R<L>;
            using container_type = typename slice_type::container_type;
            using value_type = typename slice_type::value_type;
            container_type tmp = { value_type(t0), value_type(t1), value_type(args)... };
            return xaxis_slice<L>(slice_type(std::move(tmp)));
        }
    }

    template <class L, class T>
    inline detail::enable_container_t<T, xaxis_slice<L>> keep(T&& indices)
    {
        return detail::common_drop_keep<xaxis_keep_slice, L>(std::forward<T>(indices));
    }

    template <class L, class T>
    inline detail::disable_container_t<T, xaxis_slice<L>> keep(T index)
    {
        return detail::common_drop_keep<xaxis_keep_slice, L>(index);
    }

    template <class L, class T0, class T1, class... Args>
    inline xaxis_slice<L> keep(T0 t0, T1 t1, Args... args)
    {
        return detail::common_drop_keep<xaxis_keep_slice, L>(t0, t1, args...);
    }

    template <class L, class T>
    inline detail::enable_container_t<T, xaxis_slice<L>> drop(T&& indices)
    {
        return detail::common_drop_keep<xaxis_drop_slice, L>(std::forward<T>(indices));
    }

    template <class L, class T>
    inline detail::disable_container_t<T, xaxis_slice<L>> drop(T index)
    {
        return detail::common_drop_keep<xaxis_drop_slice, L>(index);
    }

    template <class L, class T0, class T1, class... Args>
    inline xaxis_slice<L> drop(T0 t0, T1 t1, Args... args)
    {
        return detail::common_drop_keep<xaxis_drop_slice, L>(t0, t1, args...);
    }

    inline xaxis_all all() noexcept
    {
        return xaxis_all();
    }
}

#endif
