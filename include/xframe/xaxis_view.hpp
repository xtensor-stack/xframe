/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_VIEW_HPP
#define XFRAME_XAXIS_VIEW_HPP

#include "xaxis_label_slice.hpp"
#include "xaxis_variant.hpp"
#include "xsequence_view.hpp"

namespace xf
{

    template <class L, class T, class MT>
    class xaxis_view_iterator;

    /**************
     * xaxis_view *
     **************/

    template <class L, class T, class MT = hash_map_tag>
    class xaxis_view
    {
    public:

        using self_type = xaxis_view<L, T, MT>;
        using axis_type = xaxis_variant<L, T, MT>;
        using slice_type = xaxis_index_slice<T>;

        using key_type = typename axis_type::key_type;
        using mapped_type = typename axis_type::mapped_type;
        using value_type = typename axis_type::value_type;
        using reference = typename axis_type::const_reference;
        using const_reference = typename axis_type::const_reference;
        using pointer = typename axis_type::const_pointer;
        using const_pointer = typename axis_type::const_pointer;
        using size_type = typename axis_type::size_type;
        using difference_type = typename axis_type::difference_type;
        using iterator = xaxis_view_iterator<L, T, MT>;
        using const_iterator = iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using subiterator = typename axis_type::const_iterator;

        using label_list = xsequence_view<const typename axis_type::label_list, slice_type>;

        template <class S>
        xaxis_view(const axis_type& axis, S&& slice);

        explicit operator axis_type() const;

        label_list labels() const;
        key_type label(size_type i) const;

        bool empty() const;
        size_type size() const;

        bool contains(const key_type& key) const;
        mapped_type operator[](const key_type& key) const;
        mapped_type index(size_type label_index) const;

        template <class F>
        axis_type filter(const F& f) const;

        template <class F>
        axis_type filter(const F& f, size_type size) const;

        const_iterator find(const key_type& key) const;

        const_iterator begin() const;
        const_iterator end() const;

        const_iterator cbegin() const;
        const_iterator cend() const;

        const_reverse_iterator rbegin() const;
        const_reverse_iterator rend() const;

        const_reverse_iterator crbegin() const;
        const_reverse_iterator crend() const;

        axis_type as_xaxis() const;

        bool operator==(const self_type& rhs) const noexcept;
        bool operator!=(const self_type& rhs) const noexcept;

    private:

        const axis_type& m_axis;
        slice_type m_slice;
    };

    template <class OS, class L, class T, class MT>
    OS& operator<<(OS& out, const xaxis_view<L, T, MT>& axis);

    /***********************
     * xaxis_view_iterator *
     ***********************/

    template <class L, class T, class MT>
    class xaxis_view_iterator : public xtl::xrandom_access_iterator_base<xaxis_view_iterator<L, T, MT>,
                                                                         typename xaxis_view<L, T, MT>::value_type,
                                                                         typename xaxis_view<L, T, MT>::difference_type,
                                                                         typename xaxis_view<L, T, MT>::const_pointer,
                                                                         typename xaxis_view<L, T, MT>::const_reference>
    {
    public:

        using self_type = xaxis_view_iterator<L, T, MT>;
        using container_type = xaxis_view<L, T, MT>;
        using value_type = typename container_type::value_type;
        using reference = typename container_type::const_reference;
        using pointer = typename container_type::const_pointer;
        using difference_type = typename container_type::difference_type;
        using iterator_category = std::random_access_iterator_tag;
        using subiterator = typename container_type::subiterator;
        using size_type = typename container_type::size_type;
        using slice_type = typename container_type::slice_type;

        xaxis_view_iterator() = default;
        xaxis_view_iterator(subiterator it, const slice_type* sl, size_type index);

        self_type& operator++();
        self_type& operator--();

        self_type& operator+=(difference_type n);
        self_type& operator-=(difference_type n);

        difference_type operator-(const self_type& rhs) const;

        reference operator*() const;
        pointer operator->() const;

        bool equal(const self_type& rhs) const;
        bool less_than(const self_type& rhs) const;

    private:

        subiterator m_it;
        const slice_type* p_slice;
        size_type m_index;
    };

    template <class L, class T, class MT>
    typename xaxis_view_iterator<L, T, MT>::difference_type operator-(const xaxis_view_iterator<L, T, MT>& lhs,
                                                                      const xaxis_view_iterator<L, T, MT>& rhs);

    template <class L, class T, class MT>
    bool operator==(const xaxis_view_iterator<L, T, MT>& lhs, const xaxis_view_iterator<L, T, MT>& rhs);

    template <class L, class T, class MT>
    bool operator<(const xaxis_view_iterator<L, T, MT>& lhs, const xaxis_view_iterator<L, T, MT>& rhs);

    /*****************************
     * xaxis_view implementation *
     *****************************/

    template <class L, class T, class MT>
    template <class S>
    inline xaxis_view<L, T, MT>::xaxis_view(const axis_type& axis, S&& slice)
        : m_axis(axis), m_slice(std::forward<S>(slice))
    {
    }

    template <class L, class T, class MT>
    inline xaxis_view<L, T, MT>::operator axis_type() const
    {
        return m_axis.filter([this](const auto& arg) { return this->contains(arg); }, size());
    }

    template <class L, class T, class MT>
    inline auto xaxis_view<L, T, MT>::labels() const -> label_list
    {
        return label_list(m_axis.labels(), m_slice);
    }

    template <class L, class T, class MT>
    inline auto xaxis_view<L, T, MT>::label(size_type i) const -> key_type
    {
        return m_axis.label(m_slice(i));
    }

    template <class L, class T, class MT>
    inline bool xaxis_view<L, T, MT>::empty() const
    {
        return size() == 0;
    }

    template <class L, class T, class MT>
    inline auto xaxis_view<L, T, MT>::size() const -> size_type
    {
        return m_slice.size();
    }

    template <class L, class T, class MT>
    inline bool xaxis_view<L, T, MT>::contains(const key_type& key) const
    {
        if (m_axis.contains(key))
        {
            mapped_type idx = m_axis[key];
            return m_slice.contains(idx);
        }
        return false;
    }

    template <class L, class T, class MT>
    inline auto xaxis_view<L, T, MT>::operator[](const key_type& key) const -> mapped_type
    {
        mapped_type idx = m_axis[key];
        if (m_slice.contains(idx))
        {
            return idx;
        }
        else
        {
            throw std::out_of_range("invalid xaxis_view key");
        }
    }

    template <class L, class T, class MT>
    inline auto xaxis_view<L, T, MT>::index(size_type label_index) const -> mapped_type
    {
        return this->operator[](label(label_index));
    }

    template <class L, class T, class MT>
    template <class F>
    inline auto xaxis_view<L, T, MT>::filter(const F& f) const -> axis_type
    {
        return m_axis.filter([&f, this](const auto& arg) { return f(arg) && this->contains(arg); });
    }

    template <class L, class T, class MT>
    template <class F>
    inline auto xaxis_view<L, T, MT>::filter(const F& f, size_type size) const -> axis_type
    {
        return m_axis.filter([&f, this](const auto& arg) { return f(arg) && this->contains(arg); }, size);
    }

    template <class L, class T, class MT>
    inline auto xaxis_view<L, T, MT>::find(const key_type& key) const -> const_iterator
    {
        auto iter = m_axis.find(key);
        if (iter != m_axis.end() && m_slice.contains(iter->second))
        {
            return const_iterator(iter, &m_slice, m_slice.revert_index(iter->second));
        }
        else
        {
            return cend();
        }
    }

    template <class L, class T, class MT>
    inline auto xaxis_view<L, T, MT>::begin() const -> const_iterator
    {
        return cbegin();
    }

    template <class L, class T, class MT>
    inline auto xaxis_view<L, T, MT>::end() const -> const_iterator
    {
        return cend();
    }

    template <class L, class T, class MT>
    inline auto xaxis_view<L, T, MT>::cbegin() const -> const_iterator
    {
        return const_iterator(m_axis.cbegin() + m_slice(0u), &m_slice, 0u);
    }

    template <class L, class T, class MT>
    inline auto xaxis_view<L, T, MT>::cend() const -> const_iterator
    {
        size_type inc = m_slice(size());
        return const_iterator(m_axis.cbegin() + m_slice(size()), &m_slice, size());
    }

    template <class L, class T, class MT>
    inline auto xaxis_view<L, T, MT>::rbegin() const -> const_reverse_iterator
    {
        return const_reverse_iterator(end());
    }

    template <class L, class T, class MT>
    inline auto xaxis_view<L, T, MT>::rend() const -> const_reverse_iterator
    {
        return const_reverse_iterator(begin());
    }

    template <class L, class T, class MT>
    inline auto xaxis_view<L, T, MT>::crbegin() const -> const_reverse_iterator
    {
        return const_reverse_iterator(cend());
    }

    template <class L, class T, class MT>
    inline auto xaxis_view<L, T, MT>::crend() const -> const_reverse_iterator
    {
        return const_reverse_iterator(cbegin());
    }

    template <class L, class T, class MT>
    inline auto xaxis_view<L, T, MT>::as_xaxis() const -> axis_type
    {
        return m_axis.as_xaxis();
    }

    template <class L, class T, class MT>
    inline bool xaxis_view<L, T, MT>::operator==(const self_type& rhs) const noexcept
    {
        return m_axis == rhs.m_axis && m_slice == rhs.m_slice;
    }

    template <class L, class T, class MT>
    inline bool xaxis_view<L, T, MT>::operator!=(const self_type& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    template <class OS, class L, class T, class MT>
    inline OS& operator<<(OS& out, const xaxis_view<L, T, MT>& axis)
    {
        out << '(';
        for (std::size_t i = 0; i < axis.size(); ++i)
        {
            xtl::visit([&out](const auto& v) { out << v << ", "; }, axis.label(i));
        }
        out << ')';
        return out;
    }

    /**************************************
     * xaxis_view_iterator implementation *
     **************************************/

    template <class L, class T, class MT>
    inline xaxis_view_iterator<L, T, MT>::xaxis_view_iterator(subiterator it, const slice_type* sl, size_type index)
        : m_it(it), p_slice(sl), m_index(index)
    {
    }

    template <class L, class T, class MT>
    inline auto xaxis_view_iterator<L, T, MT>::operator++() -> self_type&
    {
        size_type step = p_slice->step_size(m_index);
        m_it += static_cast<difference_type>(step);
        ++m_index;
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_view_iterator<L, T, MT>::operator--() -> self_type&
    {
        --m_index;
        size_type step = p_slice->step_size(m_index);
        m_it -= static_cast<difference_type>(step);
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_view_iterator<L, T, MT>::operator+=(difference_type n) -> self_type&
    {
        size_type step = p_slice->step_size(m_index, n);
        m_it += static_cast<difference_type>(step);
        m_index += n;
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_view_iterator<L, T, MT>::operator-=(difference_type n) -> self_type&
    {
        m_index -= n;
        size_type step = p_slice->step_size(m_index, n);
        m_it -= static_cast<difference_type>(step);
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_view_iterator<L, T, MT>::operator-(const self_type& rhs) const -> difference_type
    {
        return static_cast<difference_type>(m_index) - static_cast<difference_type>(rhs.m_index);
    }

    template <class L, class T, class MT>
    inline auto xaxis_view_iterator<L, T, MT>::operator*() const -> reference
    {
        return *m_it;
    }

    template <class L, class T, class MT>
    inline auto xaxis_view_iterator<L, T, MT>::operator->() const -> pointer
    {
        return m_it.operator->();
    }

    template <class L, class T, class MT>
    inline bool xaxis_view_iterator<L, T, MT>::equal(const self_type& rhs) const
    {
        return m_it == rhs.m_it && p_slice == rhs.p_slice;
    }

    template <class L, class T, class MT>
    inline bool xaxis_view_iterator<L, T, MT>::less_than(const self_type& rhs) const
    {
        return p_slice == rhs.p_slice && m_it < rhs.m_it;
    }

    template <class L, class T, class MT>
    inline typename xaxis_view_iterator<L, T, MT>::difference_type operator-(const xaxis_view_iterator<L, T, MT>& lhs,
                                                                             const xaxis_view_iterator<L, T, MT>& rhs)
    {
        return lhs.operator-(rhs);
    }

    template <class L, class T, class MT>
    inline bool operator==(const xaxis_view_iterator<L, T, MT>& lhs, const xaxis_view_iterator<L, T, MT>& rhs)
    {
        return lhs.equal(rhs);
    }

    template <class L, class T, class MT>
    inline bool operator<(const xaxis_view_iterator<L, T, MT>& lhs, const xaxis_view_iterator<L, T, MT>& rhs)
    {
        return lhs.less_than(rhs);
    }
}

#endif
