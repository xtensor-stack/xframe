/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_LABELS_HPP
#define XFRAME_XAXIS_LABELS_HPP

#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "xtl/xiterator_base.hpp"

#include "xframe_utils.hpp"

namespace xf
{
    template <class L, class T>
    class xaxis_iterator;

    /*********
     * xaxis *
     *********/

    template <class L, class T>
    class xaxis
    {
    public:

        static_assert(std::is_integral<T>::value, "index_type must be an integral type");

        using self_type = xaxis<L, T>;
        using label_list = std::vector<L>;
        using key_type = L;
        using mapped_type = T;
        using index_type = std::unordered_map<key_type, mapped_type>;
        using value_type = typename index_type::value_type;
        using reference = typename index_type::const_reference;
        using const_reference = typename index_type::const_reference;
        using pointer = typename index_type::const_pointer;
        using const_pointer = typename index_type::const_pointer;
        using size_type = typename label_list::size_type;
        using difference_type = typename label_list::difference_type;
        using iterator = xaxis_iterator<L, T>;
        using const_iterator = iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = reverse_iterator;

        xaxis() = default;
        explicit xaxis(const label_list& labels);
        explicit xaxis(label_list&& labels);
        xaxis(std::initializer_list<key_type> init);

        template <class InputIt>
        xaxis(InputIt first, InputIt last);

        const label_list& labels() const;

        bool empty() const;
        size_type size() const;

        bool contains(const key_type& key) const;
        const mapped_type& operator[](const key_type& key) const;

        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;

        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;

        const_reverse_iterator rbegin() const noexcept;
        const_reverse_iterator rend() const noexcept;

        const_reverse_iterator crbegin() const noexcept;
        const_reverse_iterator crend() const noexcept;

    private:

        void populate_index();
        typename index_type::const_iterator find_index(const key_type& key) const;

        label_list m_labels;
        index_type m_index;

        friend class xaxis_iterator<L, T>;
    };

    template <class L, class T>
    bool operator==(const xaxis<L, T>& lhs, const xaxis<L, T>& rhs);

    template <class L, class T>
    bool operator!=(const xaxis<L, T>& lhs, const xaxis<L, T>& rhs);

    template <class L, class T, class... Args>
    bool merge_axes(xaxis<L, T>& output, const Args&... axes);

    /******************
     * xaxis_iterator *
     ******************/

    template <class L, class T>
    struct xaxis_traits
    {
        using axis_type = xaxis<L, T>;
        using value_type = typename axis_type::value_type;
        using reference = typename axis_type::reference;
        using pointer = typename axis_type::pointer;
        using difference_type = typename axis_type::difference_type;
    };

    template <class L, class T>
    class xaxis_iterator : public xtl::xrandom_access_iterator_base<xaxis_iterator<L, T>,
                                                                    typename xaxis_traits<L, T>::value_type,
                                                                    typename xaxis_traits<L, T>::difference_type,
                                                                    typename xaxis_traits<L, T>::pointer,
                                                                    typename xaxis_traits<L, T>::reference>
    {

    public:

        using self_type = xaxis_iterator<L, T>;
        using container_type = xaxis<L, T>;
        using label_list = typename container_type::label_list;
        using label_iterator = typename label_list::const_iterator;
        using value_type = typename container_type::value_type;
        using reference = typename container_type::reference;
        using pointer = typename container_type::pointer;
        using difference_type = typename container_type::difference_type;
        using iterator_category = std::random_access_iterator_tag;

        xaxis_iterator() = default;
        xaxis_iterator(const container_type* c, label_iterator it);

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

        const container_type* p_c;
        label_iterator m_it;
    };

    template <class L, class T>
    typename xaxis_iterator<L, T>::difference_type operator-(const xaxis_iterator<L, T>& lhs, const xaxis_iterator<L, T>& rhs);

    template <class L, class T>
    bool operator==(const xaxis_iterator<L, T>& lhs, const xaxis_iterator<L, T>& rhs);

    template <class L, class T>
    bool operator<(const xaxis_iterator<L, T>& lhs, const xaxis_iterator<L, T>& rhs);
    
    /************************
     * xaxis implementation *
     ************************/

    template <class L, class T>
    inline xaxis<L, T>::xaxis(const label_list& labels)
        : m_labels(labels)
    {
        populate_index();
    }

    template <class L, class T>
    inline xaxis<L, T>::xaxis(label_list&& labels)
        : m_labels(std::move(labels))
    {
        populate_index();
    }

    template <class L, class T>
    inline xaxis<L, T>::xaxis(std::initializer_list<key_type> init)
        : m_labels(init)
    {
        populate_index();
    }

    template <class L, class T>
    template <class InputIt>
    inline xaxis<L, T>::xaxis(InputIt first, InputIt last)
        : m_labels(first, last)
    {
        populate_index();
    }

    template <class L, class T>
    inline void xaxis<L, T>::populate_index()
    {
        for(size_type i = 0; i < m_labels.size(); ++i)
        {
            m_index[m_labels[i]] = T(i);
        }
    }

    template <class L, class T>
    inline auto xaxis<L, T>::find_index(const key_type& key) const -> typename index_type::const_iterator
    {
        return m_index.find(key);
    }

    template <class L, class T>
    inline auto xaxis<L, T>::labels() const -> const label_list&
    {
        return m_labels;
    }

    template <class L, class T>
    inline bool xaxis<L, T>::empty() const
    {
        return m_labels.empty();
    }

    template <class L, class T>
    inline auto xaxis<L, T>::size() const -> size_type
    {
        return m_labels.size();
    }

    template <class L, class T>
    inline bool xaxis<L, T>::contains(const key_type& key) const
    {
        return m_index.count(key) != typename index_type::size_type(0);
    }

    template <class L, class T>
    inline auto xaxis<L, T>::operator[](const key_type& key) const -> const mapped_type&
    {
        return m_index.at(key);
    }

    template <class L, class T>
    inline auto xaxis<L, T>::begin() const noexcept -> const_iterator
    {
        return cbegin();
    }

    template <class L, class T>
    inline auto xaxis<L, T>::end() const noexcept -> const_iterator
    {
        return cend();
    }

    template <class L, class T>
    inline auto xaxis<L, T>::cbegin() const noexcept -> const_iterator
    {
        return const_iterator(this, m_labels.begin());
    }

    template <class L, class T>
    inline auto xaxis<L, T>::cend() const noexcept -> const_iterator
    {
        return const_iterator(this, m_labels.end());
    }

    template <class L, class T>
    inline auto xaxis<L, T>::rbegin() const noexcept -> const_reverse_iterator
    {
        return crbegin();
    }

    template <class L, class T>
    inline auto xaxis<L, T>::rend() const noexcept -> const_reverse_iterator
    {
        return crend();
    }

    template <class L, class T>
    inline auto xaxis<L, T>::crbegin() const noexcept -> const_reverse_iterator
    {
        return const_reverse_iterator(cend());
    }

    template <class L, class T>
    inline auto xaxis<L, T>::crend() const noexcept -> const_reverse_iterator
    {
        return const_reverse_iterator(cbegin());
    }

    template <class L, class T>
    inline bool operator==(const xaxis<L, T>& lhs, const xaxis<L, T>& rhs)
    {
        return lhs.labels() == rhs.labels();
    }

    template <class L, class T>
    inline bool operator!=(const xaxis<L, T>& lhs, const xaxis<L, T>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class L, class T, class... Args>
    inline bool merge_axes(xaxis<L, T>& output, const Args&... axes)
    {
        using axis_type = xaxis<L, T>;
        using label_list = typename axis_type::label_list;
        label_list l;
        bool res = merge_to(l, axes.labels()...);
        output = axis_type(std::move(l));
        return res;
    }

    /*********************************
     * xaxis_iterator implementation *
     *********************************/

    template <class L, class T>
    inline xaxis_iterator<L, T>::xaxis_iterator(const container_type* c, label_iterator it)
        : p_c(c), m_it(it)
    {
    }

    template <class L, class T>
    inline auto xaxis_iterator<L, T>::operator++() -> self_type&
    {
        ++m_it;
        return *this;
    }

    template <class L, class T>
    inline auto xaxis_iterator<L, T>::operator--() -> self_type&
    {
        --m_it;
        return *this;
    }

    template <class L, class T>
    inline auto xaxis_iterator<L, T>::operator+=(difference_type n) -> self_type&
    {
        m_it += n;
        return *this;
    }

    template <class L, class T>
    inline auto xaxis_iterator<L, T>::operator-=(difference_type n) -> self_type&
    {
        m_it -= n;
        return *this;
    }

    template <class L, class T>
    inline auto xaxis_iterator<L, T>::operator-(const self_type& rhs) const -> difference_type 
    {
        return m_it - rhs.m_it;
    }

    template <class L, class T>
    inline auto xaxis_iterator<L, T>::operator*() const -> reference 
    {
        return *(p_c->find_index(*m_it));
    }

    template <class L, class T>
    inline auto xaxis_iterator<L, T>::operator->() const -> pointer 
    {
        return &(*(p_c->find_index(*m_it)));
    }

    template <class L, class T>
    inline bool xaxis_iterator<L, T>::equal(const self_type& rhs) const
    {
        return m_it == rhs.m_it;
    }

    template <class L, class T>
    inline bool xaxis_iterator<L, T>::less_than(const self_type& rhs) const
    {
        return m_it < rhs.m_it;
    }

    template <class L, class T>
    inline typename xaxis_iterator<L, T>::difference_type operator-(const xaxis_iterator<L, T>& lhs, const xaxis_iterator<L, T>& rhs)
    {
        return lhs.operator-(rhs);
    }

    template <class L, class T>
    inline bool operator==(const xaxis_iterator<L, T>& lhs, const xaxis_iterator<L, T>& rhs)
    {
        return lhs.equal(rhs);
    }

    template <class L, class T>
    inline bool operator<(const xaxis_iterator<L, T>& lhs, const xaxis_iterator<L, T>& rhs)
    {
        return lhs.less_than(rhs);
    }
}

#endif

