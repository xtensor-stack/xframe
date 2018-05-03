/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay, Wolf Vollprecht and   *
* Martin Renou                                                             *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_BASE_HPP
#define XFRAME_XAXIS_BASE_HPP

#include <algorithm>
#include <iterator>
#include <vector>

namespace xf
{

    template <class D>
    struct xaxis_inner_types;

    /**************
     * xaxis_base *
     **************/

    template <class D>
    class xaxis_base
    {
    public:

        using derived_type = D;
        using inner_types = xaxis_inner_types<D>;

        using key_type = typename inner_types::key_type;
        using mapped_type = typename inner_types::mapped_type;
        using label_list = std::vector<key_type>;
        using size_type = typename label_list::size_type;
        using difference_type = typename label_list::difference_type;
        using iterator = typename inner_types::iterator;
        using const_iterator = iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = reverse_iterator;

        static_assert(std::is_integral<mapped_type>::value, "mapped_type T must be an integral type");

        derived_type& derived_cast() & noexcept;
        const derived_type& derived_cast() const & noexcept;
        derived_type derived_cast() && noexcept;

        const label_list& labels() const noexcept;

        bool empty() const noexcept;
        size_type size() const noexcept;

        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;

        const_reverse_iterator rbegin() const noexcept;
        const_reverse_iterator rend() const noexcept;

        const_reverse_iterator crbegin() const noexcept;
        const_reverse_iterator crend() const noexcept;

    protected:

        xaxis_base();
        xaxis_base(const label_list& labels);
        xaxis_base(label_list&& labels);
        xaxis_base(std::initializer_list<key_type> init);

        template <class InputIt>
        xaxis_base(InputIt first, InputIt last);

        ~xaxis_base() = default;

        xaxis_base(const xaxis_base&) = default;
        xaxis_base& operator=(const xaxis_base&) = default;

        xaxis_base(xaxis_base&&) = default;
        xaxis_base& operator=(xaxis_base&&) = default;

        label_list& mutable_labels() noexcept;

        template <class F>
        label_list filter_labels(const F& f) const noexcept;

        template <class F>
        label_list filter_labels(const F& f, size_type size) const noexcept;

        label_list m_labels;
    };

    template <class D1, class D2>
    bool operator==(const xaxis_base<D1>& lhs, const xaxis_base<D2>& rhs) noexcept;

    template <class D1, class D2>
    bool operator!=(const xaxis_base<D1>& lhs, const xaxis_base<D2>& rhs) noexcept;

    template <class OS, class D>
    OS& operator<<(OS& out, const xaxis_base<D>& axis);

    template <class D, class... Args>
    bool merge_axes(xaxis_base<D>& output, const Args&... axes);

    template <class D, class... Args>
    bool intersect_axes(xaxis_base<D>& output, const Args&... axes);

    /************************
     * is_axis metafunction *
     ************************/

    namespace detail
    {
        template <class T>
        struct is_axis_impl : std::false_type
        {
        };

        template <class D>
        struct is_axis_impl<xaxis_base<D>> : std::true_type
        {
        };
    }

    template <class T>
    struct is_axis : detail::is_axis_impl<std::decay_t<T>>
    {
    };

    /*****************************
     * xaxis_base implementation *
     *****************************/

    template <class D>
    inline xaxis_base<D>::xaxis_base()
        : m_labels()
    {
    }

    template <class D>
    inline xaxis_base<D>::xaxis_base(const label_list& labels)
        : m_labels(labels)
    {
    }

    template <class D>
    inline xaxis_base<D>::xaxis_base(label_list&& labels)
        : m_labels(std::move(labels))
    {
    }

    template <class D>
    inline xaxis_base<D>::xaxis_base(std::initializer_list<key_type> init)
        : m_labels(init)
    {
    }

    template <class D>
    template <class InputIt>
    inline xaxis_base<D>::xaxis_base(InputIt first, InputIt last)
        : m_labels(first, last)
    {
    }

    template <class D>
    inline auto xaxis_base<D>::derived_cast() & noexcept -> derived_type&
    {
        return *static_cast<derived_type*>(this);
    }

    template <class D>
    inline auto xaxis_base<D>::derived_cast() const & noexcept -> const derived_type&
    {
        return *static_cast<const derived_type*>(this);
    }

    template <class D>
    inline auto xaxis_base<D>::derived_cast() && noexcept -> derived_type
    {
        return *static_cast<derived_type*>(this);
    }

    template <class D>
    inline auto xaxis_base<D>::labels() const noexcept -> const label_list&
    {
        return m_labels;
    }

    template <class D>
    inline bool xaxis_base<D>::empty() const noexcept
    {
        return m_labels.empty();
    }

    template <class D>
    inline auto xaxis_base<D>::size() const noexcept -> size_type
    {
        return m_labels.size();
    }

    template <class D>
    inline auto xaxis_base<D>::begin() const noexcept -> const_iterator
    {
        return derived_cast().cbegin();
    }

    template <class D>
    inline auto xaxis_base<D>::end() const noexcept -> const_iterator
    {
        return derived_cast().cend();
    }

    template <class D>
    inline auto xaxis_base<D>::rbegin() const noexcept -> const_reverse_iterator
    {
        return crbegin();
    }

    template <class D>
    inline auto xaxis_base<D>::rend() const noexcept -> const_reverse_iterator
    {
        return crend();
    }

    template <class D>
    inline auto xaxis_base<D>::crbegin() const noexcept -> const_reverse_iterator
    {
        return const_reverse_iterator(derived_cast().cend());
    }

    template <class D>
    inline auto xaxis_base<D>::crend() const noexcept -> const_reverse_iterator
    {
        return const_reverse_iterator(derived_cast().cbegin());
    }

    template <class D>
    inline auto xaxis_base<D>::mutable_labels() noexcept -> label_list&
    {
        return m_labels;
    }

    template <class D>
    template <class F>
    inline auto xaxis_base<D>::filter_labels(const F& f) const noexcept -> label_list
    {
        label_list l;
        std::copy_if(m_labels.cbegin(), m_labels.cend(), std::back_inserter(l), f);
        return l;
    }

    template <class D>
    template <class F>
    inline auto xaxis_base<D>::filter_labels(const F& f, size_type size) const noexcept -> label_list
    {
        label_list l(size);
        std::copy_if(m_labels.cbegin(), m_labels.cend(), l.begin(), f);
        return l;
    }

    template <class D1, class D2>
    inline bool operator==(const xaxis_base<D1>& lhs, const xaxis_base<D2>& rhs) noexcept
    {
        return lhs.labels() == rhs.labels();
    }

    template <class D1, class D2>
    inline bool operator!=(const xaxis_base<D1>& lhs, const xaxis_base<D2>& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    template <class OS, class D>
    inline OS& operator<<(OS& out, const xaxis_base<D>& axis)
    {
        using iterator = std::ostream_iterator<typename xaxis_base<D>::key_type, typename OS::char_type, typename OS::traits_type>;
        out << '(';
        std::copy(axis.labels().begin(), axis.labels().end(), iterator(out, ", "));
        out << ')';
        return out;
    }
}

#endif
