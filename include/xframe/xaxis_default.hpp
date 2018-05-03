/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay, Wolf Vollprecht and   *
* Martin Renou                                                             *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_DEFAULT_HPP
#define XFRAME_XAXIS_DEFAULT_HPP

#include <utility>
#include <vector>
#include <ostream>
#include <iterator>

#include "xtl/xiterator_base.hpp"

#include "xaxis_base.hpp"
#include "xaxis.hpp"

namespace xf
{
    template <class L, class T>
    class xaxis_default_iterator;

    /*****************
     * xaxis_default *
     *****************/

    template <class L, class T>
    class xaxis_default : public xaxis_base<xaxis_default<L, T>>
    {
    public:

        using base_type = xaxis_base<xaxis_default>;
        using self_type = xaxis_default<L, T>;
        using axis_type = xaxis<L, T>;
        using key_type = typename base_type::key_type;
        using label_list = typename base_type::label_list;
        using mapped_type = typename base_type::mapped_type;
        using value_type = std::pair<key_type, mapped_type>;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using size_type = typename base_type::size_type;
        using difference_type = typename base_type::difference_type;
        using iterator = typename base_type::iterator;
        using const_iterator = typename base_type::const_iterator;
        using reverse_iterator = typename base_type::reverse_iterator;
        using const_reverse_iterator = typename base_type::const_reverse_iterator;

        static_assert(std::is_integral<key_type>::value, "key_type L must be an integral type");

        explicit xaxis_default(const size_type& size = 0);

        bool is_sorted() const noexcept;

        bool contains(const key_type& key) const;
        mapped_type operator[](const key_type& key) const;

        template <class F>
        axis_type filter(const F& f) const noexcept;

        template <class F>
        axis_type filter(const F& f, size_type size) const noexcept;

        const_iterator find(const key_type& key) const;

        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;

    protected:

        void populate_labels(const size_type& size = 0);
    };

    /********************
    * xaxis_inner_types *
    *********************/

    template <class L, class T>
    struct xaxis_inner_types<xaxis_default<L, T>>
    {
        using key_type = L;
        using mapped_type = T;
        using iterator = xaxis_default_iterator<L, T>;
    };

    /**************************
     * xaxis_default_iterator *
     **************************/

    template <class L, class T>
    class xaxis_default_iterator : public xtl::xrandom_access_iterator_base<xaxis_default_iterator<L, T>,
                                                                   typename xaxis_default<L, T>::value_type,
                                                                   typename xaxis_default<L, T>::difference_type,
                                                                   typename xaxis_default<L, T>::const_pointer,
                                                                   typename xaxis_default<L, T>::const_reference>
    {

    public:

        using self_type = xaxis_default_iterator<L, T>;
        using container_type = xaxis_default<L, T>;
        using label_list = typename container_type::label_list;
        using key_type = typename container_type::key_type;
        using mapped_type = typename container_type::mapped_type;
        using value_type = typename container_type::value_type;
        using reference = typename container_type::const_reference;
        using pointer = typename container_type::const_pointer;
        using difference_type = typename label_list::difference_type;
        using iterator_category = std::random_access_iterator_tag;

        xaxis_default_iterator() = default;
        xaxis_default_iterator(mapped_type value);

        self_type& operator++();
        self_type& operator--();

        self_type& operator+=(difference_type n);
        self_type& operator-=(difference_type n);

        difference_type operator-(const self_type& rhs) const;

        reference operator*() const;
        pointer operator->() const;

        bool equal(const self_type& rhs) const noexcept;
        bool less_than(const self_type& rhs) const noexcept;

    private:

        value_type m_value;
    };

    template <class L, class T>
    typename xaxis_default_iterator<L, T>::difference_type operator-(const xaxis_default_iterator<L, T>& lhs, const xaxis_default_iterator<L, T>& rhs);

    template <class L, class T>
    bool operator==(const xaxis_default_iterator<L, T>& lhs, const xaxis_default_iterator<L, T>& rhs) noexcept;

    template <class L, class T>
    bool operator<(const xaxis_default_iterator<L, T>& lhs, const xaxis_default_iterator<L, T>& rhs) noexcept;

    /********************************
     * xaxis_default implementation *
     ********************************/

    template <class L, class T>
    inline xaxis_default<L, T>::xaxis_default(const size_type& size)
        : base_type()
    {
        populate_labels(size);
    }

    template <class L, class T>
    inline bool xaxis_default<L, T>::is_sorted() const noexcept
    {
        return true;
    }

    template <class L, class T>
    inline bool xaxis_default<L, T>::contains(const key_type& key) const
    {
        return key_type(0) <= key && key < key_type(this->size());
    }

    template <class L, class T>
    inline auto xaxis_default<L, T>::operator[](const key_type& key) const -> mapped_type
    {
        return this->labels().at(key);
    }

    template <class L, class T>
    template <class F>
    inline auto xaxis_default<L, T>::filter(const F& f) const noexcept -> axis_type
    {
        return axis_type(base_type::filter_labels(f), true);
    }

    template <class L, class T>
    template <class F>
    inline auto xaxis_default<L, T>::filter(const F& f, size_type size) const noexcept -> axis_type
    {
        return axis_type(base_type::filter_labels(f, size), true);
    }

    template <class L, class T>
    inline auto xaxis_default<L, T>::find(const key_type& key) const -> const_iterator
    {
        return contains(key) ? const_iterator(mapped_type(key)) : cend();
    }

    template <class L, class T>
    inline auto xaxis_default<L, T>::cbegin() const noexcept -> const_iterator
    {
        return const_iterator(mapped_type(0));
    }

    template <class L, class T>
    inline auto xaxis_default<L, T>::cend() const noexcept -> const_iterator
    {
        return const_iterator(mapped_type(this->size()));
    }

    template <class L, class T>
    inline void xaxis_default<L, T>::populate_labels(const size_type& size)
    {
        auto& labels = this->mutable_labels();
        for(size_type i = 0; i < size; ++i)
        {
            labels.push_back(key_type(i));
        }
    }

    /*****************************************
     * xaxis_default_iterator implementation *
     *****************************************/

    template <class L, class T>
    inline xaxis_default_iterator<L, T>::xaxis_default_iterator(mapped_type value)
        : m_value(std::make_pair(static_cast<key_type>(value), value))
    {
    }

    template <class L, class T>
    inline auto xaxis_default_iterator<L, T>::operator++() -> self_type&
    {
        ++m_value.first;
        ++m_value.second;
        return *this;
    }

    template <class L, class T>
    inline auto xaxis_default_iterator<L, T>::operator--() -> self_type&
    {
        --m_value.first;
        --m_value.second;
        return *this;
    }

    template <class L, class T>
    inline auto xaxis_default_iterator<L, T>::operator+=(difference_type n) -> self_type&
    {
        m_value.first += static_cast<key_type>(n);
        m_value.second += static_cast<mapped_type>(n);
        return *this;
    }

    template <class L, class T>
    inline auto xaxis_default_iterator<L, T>::operator-=(difference_type n) -> self_type&
    {
        m_value.first -= static_cast<key_type>(n);
        m_value.second -= static_cast<mapped_type>(n);
        return *this;
    }

    template <class L, class T>
    inline auto xaxis_default_iterator<L, T>::operator-(const self_type& rhs) const -> difference_type
    {
        return m_value.first - rhs.m_value.first;
        return m_value.second - rhs.m_value.second;
    }

    template <class L, class T>
    inline auto xaxis_default_iterator<L, T>::operator*() const -> reference
    {
        return m_value;
    }

    template <class L, class T>
    inline auto xaxis_default_iterator<L, T>::operator->() const -> pointer
    {
        return &m_value;
    }

    template <class L, class T>
    inline bool xaxis_default_iterator<L, T>::equal(const self_type& rhs) const noexcept
    {
        return m_value.first == rhs.m_value.first;
    }

    template <class L, class T>
    inline bool xaxis_default_iterator<L, T>::less_than(const self_type& rhs) const noexcept
    {
        return m_value.first < rhs.m_value.first;
    }

    template <class L, class T>
    inline typename xaxis_default_iterator<L, T>::difference_type operator-(const xaxis_default_iterator<L, T>& lhs, const xaxis_default_iterator<L, T>& rhs)
    {
        return lhs.operator-(rhs);
    }

    template <class L, class T>
    inline bool operator==(const xaxis_default_iterator<L, T>& lhs, const xaxis_default_iterator<L, T>& rhs) noexcept
    {
        return lhs.equal(rhs);
    }

    template <class L, class T>
    inline bool operator<(const xaxis_default_iterator<L, T>& lhs, const xaxis_default_iterator<L, T>& rhs) noexcept
    {
        return lhs.less_than(rhs);
    }
}

#endif
