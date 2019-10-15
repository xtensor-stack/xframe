/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XSEQUENCE_VIEW_HPP
#define XFRAME_XSEQUENCE_VIEW_HPP

#include <algorithm>
#include "xtl/xiterator_base.hpp"

namespace xf
{

    template <class C, class S, bool is_const>
    class xsequence_view_iterator;

    /******************
     * xsequence_view *
     ******************/

    template <class C, class S>
    class xsequence_view
    {
    public:

        using self_type = xsequence_view<C, S>;
        using container_type = C;
        using slice_type = S;

        static constexpr bool is_const = std::is_const<std::remove_reference_t<C>>::value;
        using value_type = typename container_type::value_type;
        using reference = std::conditional_t<is_const,
                                             typename container_type::const_reference,
                                             typename container_type::reference>;
        using const_reference = typename container_type::const_reference;
        using pointer = typename container_type::pointer;
        using const_pointer = std::conditional_t<is_const,
                                                 typename container_type::const_pointer,
                                                 typename container_type::pointer>;
        using size_type = typename container_type::size_type;
        using difference_type = typename container_type::difference_type;

        using iterator = xsequence_view_iterator<C, S, false>;
        using const_iterator = xsequence_view_iterator<C, S, true>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        template <class OC>
        xsequence_view(OC&& c, slice_type slice) noexcept;

        size_type size() const noexcept;
        bool empty() const noexcept;

        reference operator[](size_type i);
        const_reference operator[](size_type i) const;

        reference at(size_type i);
        const_reference at(size_type i) const;

        reference front();
        const_reference front() const;

        reference back();
        const_reference back() const;

        iterator begin() noexcept;
        iterator end() noexcept;

        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;

        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;

        reverse_iterator rbegin() noexcept;
        reverse_iterator rend() noexcept;

        const_reverse_iterator rbegin() const noexcept;
        const_reverse_iterator rend() const noexcept;

        const_reverse_iterator crbegin() const noexcept;
        const_reverse_iterator crend() const noexcept;

    private:

        container_type m_container;
        slice_type m_slice;
    };

    template <class C, class S>
    bool operator==(const xsequence_view<C, S>& lhs, const xsequence_view<C, S>& rhs);

    template <class C, class S>
    bool operator==(const xsequence_view<C, S>& lhs, const C& rhs);

    template <class C, class S>
    bool operator==(const C& lhs, const xsequence_view<C, S>& rhs);

    template <class C, class S>
    bool operator!=(const xsequence_view<C, S>& lhs, const xsequence_view<C, S>& rhs);

    template <class C, class S>
    bool operator!=(const xsequence_view<C, S>& lhs, const C& rhs);

    template <class C, class S>
    bool operator!=(const C& lhs, const xsequence_view<C, S>& rhs);

    template <class C, class S, bool is_const>
    struct xsequence_view_iterator_traits
    {
        using iterator_type = xsequence_view_iterator<C, S, is_const>;
        using value_type = typename C::value_type;
        using reference = std::conditional_t<is_const, typename C::const_reference, typename C::reference>;
        using pointer = std::conditional_t<is_const, typename C::const_pointer, typename C::pointer>;
        using difference_type = typename C::difference_type;
    };

    /***************************
     * xsequence_view_iterator *
     ***************************/

    template <class C, class S, bool is_const>
    class xsequence_view_iterator
        : public xtl::xrandom_access_iterator_base2<xsequence_view_iterator_traits<C, S, is_const>>
    {
    public:

        using self_type = xsequence_view_iterator<C, S, is_const>;
        using base_type = xtl::xrandom_access_iterator_base2<xsequence_view_iterator_traits<C, S, is_const>>;
        using view_type = std::conditional_t<is_const, const xsequence_view<C, S>, xsequence_view<C, S>>;
        using slice_type = S;

        using value_type = typename base_type::value_type;
        using reference = typename base_type::reference;
        using pointer = typename base_type::pointer;
        using difference_type = typename base_type::difference_type;
        using size_type = typename slice_type::size_type;

        xsequence_view_iterator() = default;
        xsequence_view_iterator(view_type* view, size_type index);

        self_type& operator++();
        self_type& operator--();

        self_type& operator+=(difference_type n);
        self_type& operator-=(difference_type n);

        difference_type operator-(const self_type& rhs) const;

        reference operator*() const;
        pointer operator->() const;

        bool operator==(const self_type& rhs) const;
        bool operator<(const self_type& rhs) const;

    private:

        view_type* p_view;
        size_type m_index;

        // NB: another possible implementation is to store an iterator on the underlying
        // sequence and calls step_size(m_index) on the slice to get the increment to apply
        // to the iterator. Benchmark is required to check which method is faster.
    };

    /*********************************
     * xsequence_view implementation *
     *********************************/

    template <class C, class S>
    template <class OC>
    inline xsequence_view<C, S>::xsequence_view(OC&& c, slice_type slice) noexcept
        : m_container(std::forward<OC>(c)), m_slice(std::move(slice))
    {
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::size() const noexcept -> size_type
    {
        return m_slice.size();
    }

    template <class C, class S>
    inline bool xsequence_view<C, S>::empty() const noexcept
    {
        return size() == 0;
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::operator[](size_type i) -> reference
    {
        return m_container[m_slice(i)];
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::operator[](size_type i) const -> const_reference
    {
        return m_container[m_slice(i)];
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::at(size_type i) -> reference
    {
        return m_container.at(m_slice(i));
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::at(size_type i) const -> const_reference
    {
        return m_container.at(m_slice(i));
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::front() -> reference
    {
        return m_container[m_slice(0)];
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::front() const -> const_reference
    {
        return m_container[m_slice(0)];
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::back() -> reference
    {
        return m_container[m_slice(m_slice.size() - 1)];
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::back() const -> const_reference
    {
        return m_container[m_slice(m_slice.size() - 1)];
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::begin() noexcept -> iterator
    {
        return iterator(this, size_type(0));
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::end() noexcept -> iterator
    {
        return iterator(this, size());
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::begin() const noexcept -> const_iterator
    {
        return cbegin();
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::end() const noexcept -> const_iterator
    {
        return cend();
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::cbegin() const noexcept -> const_iterator
    {
        return const_iterator(this, size_type(0));
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::cend() const noexcept -> const_iterator
    {
        return const_iterator(this, size());
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::rbegin() noexcept -> reverse_iterator
    {
        return reverse_iterator(end());
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::rend() noexcept -> reverse_iterator
    {
        return reverse_iterator(begin());
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::rbegin() const noexcept -> const_reverse_iterator
    {
        return crbegin();
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::rend() const noexcept -> const_reverse_iterator
    {
        return crend();
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::crbegin() const noexcept -> const_reverse_iterator
    {
        return const_reverse_iterator(cend());
    }

    template <class C, class S>
    inline auto xsequence_view<C, S>::crend() const noexcept -> const_reverse_iterator
    {
        return const_reverse_iterator(cbegin());
    }

    namespace detail
    {
        // clang complains on OSX that std::equal_to is not defined for mpark::variant
#ifdef __APPLE__
        template <class InputIt1, class InputIt2>
        inline bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2)
        {
            for(; first1 != last1; ++first1, ++first2)
            {
                if(!(*first1 == *first2))
                {
                    return false;
                }
            }
            return true;
        }
#endif
        template <class C1, class C2>
        inline bool compare_sequence_view(const C1& lhs, const C2& rhs)
        {
            // clang complains on OSX that std::equal_to is not defined for mpark::variant
#ifdef __APPLE__
            return lhs.size() == rhs.size() && detail::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
#else
            return lhs.size() == rhs.size() && std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
#endif
        }
    }

    template <class C, class S>
    inline bool operator==(const xsequence_view<C, S>& lhs, const xsequence_view<C, S>& rhs)
    {
        return detail::compare_sequence_view(lhs, rhs);
    }

    template <class C, class S>
    inline bool operator==(const xsequence_view<C, S>& lhs, const C& rhs)
    {
        return detail::compare_sequence_view(lhs, rhs);
    }

    template <class C, class S>
    inline bool operator==(const C& lhs, const xsequence_view<C, S>& rhs)
    {
        return detail::compare_sequence_view(lhs, rhs);
    }

    template <class C, class S>
    inline bool operator!=(const xsequence_view<C, S>& lhs, const xsequence_view<C, S>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class C, class S>
    inline bool operator!=(const xsequence_view<C, S>& lhs, const C& rhs)
    {
        return !(lhs == rhs);
    }

    template <class C, class S>
    inline bool operator!=(const C& lhs, const xsequence_view<C, S>& rhs)
    {
        return !(lhs == rhs);
    }

    /******************************************
     * xsequence_view_iterator implementation *
     ******************************************/

    template <class C, class S, bool IC>
    inline xsequence_view_iterator<C, S, IC>::xsequence_view_iterator(view_type* view, size_type index)
        : p_view(view), m_index(index)
    {
    }

    template <class C, class S, bool IC>
    inline auto xsequence_view_iterator<C, S, IC>::operator++() -> self_type&
    {
        ++m_index;
        return *this;
    }

    template <class C, class S, bool IC>
    inline auto xsequence_view_iterator<C, S, IC>::operator--() -> self_type&
    {
        --m_index;
        return *this;
    }

    template <class C, class S, bool IC>
    inline auto xsequence_view_iterator<C, S, IC>::operator+=(difference_type n) -> self_type&
    {
        difference_type tmp = static_cast<difference_type>(m_index) + n;
        m_index = static_cast<size_type>(tmp);
        return *this;
    }

    template <class C, class S, bool IC>
    inline auto xsequence_view_iterator<C, S, IC>::operator-=(difference_type n) -> self_type&
    {
        difference_type tmp = static_cast<difference_type>(m_index) - n;
        m_index = static_cast<size_type>(tmp);
        return *this;
    }

    template <class C, class S, bool IC>
    inline auto xsequence_view_iterator<C, S, IC>::operator-(const self_type& rhs) const -> difference_type
    {
        return static_cast<difference_type>(m_index) - static_cast<difference_type>(rhs.m_index);
    }

    template <class C, class S, bool IC>
    inline auto xsequence_view_iterator<C, S, IC>::operator*() const -> reference
    {
        return p_view->operator[](m_index);
    }

    template <class C, class S, bool IC>
    inline auto xsequence_view_iterator<C, S, IC>::operator->() const -> pointer
    {
        return &(this->operator*());
    }

    template <class C, class S, bool IC>
    inline bool xsequence_view_iterator<C, S, IC>::operator==(const self_type& rhs) const
    {
        return p_view == rhs.p_view && m_index == rhs.m_index;
    }

    template <class C, class S, bool IC>
    inline bool xsequence_view_iterator<C, S, IC>::operator<(const self_type& rhs) const
    {
        return p_view == rhs.p_view && m_index < rhs.m_index;
    }
}

#endif
