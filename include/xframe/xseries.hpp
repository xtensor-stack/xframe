/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XSERIES_HPP
#define XFRAME_XSERIES_HPP

#include "xtl/xproxy_wrapper.hpp"
#include "xaxis.hpp"

namespace xf
{
    template <class E, class L, bool is_const>
    class xseries_iterator;

    /***********
     * xseries *
     ***********/

    template <class E, class L>
    class xseries
    {

    public:

        using label_list = L;
        using expression_type = E;
        using axis_type = xaxis<L, typename E::size_type>;
        using key_type = typename label_list::value_type;
        using mapped_type = typename expression_type::value_type;
        using value_type = std::pair<const key_type&, mapped_type>;
        using reference = std::pair<const key_type&, typename expression_type::reference>;
        using const_reference = std::pair<const key_type&, typename expression_type::const_reference>;
        using size_type = typename expression_type::size_type;
        using difference_type = typename expression_type::difference_type;
        using iterator = xseries_iterator<E, L, false>;
        using const_iterator = xseries_iterator<E, L, true>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        template <class AE, class AL>
        xseries(AE&& e, AL&& labels);

        const label_list& labels() const;

        expression_type& values();
        const expression_type& values() const;

        bool empty() const;
        size_type size() const;

        bool contains(const key_type& key) const;
        
        mapped_type& operator[](const key_type& key);
        const mapped_type& operator[](const key_type& key) const;
        
        iterator begin();
        iterator end();

        const_iterator begin() const;
        const_iterator end() const;
        const_iterator cbegin() const;
        const_iterator cend() const;

        reverse_iterator rbegin();
        reverse_iterator rend();
        
        const_reverse_iterator rbegin() const;
        const_reverse_iterator rend() const;
        const_reverse_iterator crbegin() const;
        const_reverse_iterator crend() const;

    private:

        axis_type m_axis;
        expression_type m_expression;
    };

    template <class E, class L>
    bool operator==(const xseries<E, L>& lhs, const xseries<E, L>& rhs);

    template <class E, class L>
    bool operator!=(const xseries<E, L>& lhs, const xseries<E, L>& rhs);

    /********************
     * xseries_iterator *
     ********************/

    template <class E, class L, bool is_const>
    struct xseries_iterator_traits
    {
        using container_type = xseries<E, L>;
        using value_type = typename container_type::value_type;
        using container_reference = std::conditional_t<is_const,
                                                       typename container_type::const_reference,
                                                       typename container_type::reference>;
        using reference = xtl::xproxy_wrapper<container_reference>;
        using pointer = xtl::xclosure_pointer<container_reference>;
        using difference_type = typename container_type::difference_type;
    };

    template <class E, class L, bool is_const>
    class xseries_iterator : public xtl::xrandom_access_iterator_base<xseries_iterator<E, L, is_const>,
                                                                      typename xseries_iterator_traits<E, L, is_const>::value_type,
                                                                      typename xseries_iterator_traits<E, L, is_const>::difference_type,
                                                                      typename xseries_iterator_traits<E, L, is_const>::pointer,
                                                                      typename xseries_iterator_traits<E, L, is_const>::reference>
    {

    public:

        using self_type = xseries_iterator<E, L, is_const>;
        using traits_type = xseries_iterator_traits<E, L, is_const>;
        using container_type = typename traits_type::container_type;
        using label_list = typename container_type::label_list;
        using label_iterator = typename label_list::const_iterator;
        using expression_type = typename container_type::expression_type;
        using value_iterator = std::conditional_t<is_const,
                                                  typename expression_type::const_iterator,
                                                  typename expression_type::iterator>;
        using container_reference = typename traits_type::container_reference;
        using value_type = typename traits_type::value_type;
        using reference = typename traits_type::reference;
        using pointer = typename traits_type::pointer;
        using difference_type = typename traits_type::difference_type;
        using iterator_category = std::random_access_iterator_tag;

        xseries_iterator() = default;
        xseries_iterator(label_iterator lit, value_iterator vit);

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

        label_iterator m_lit;
        value_iterator m_vit;
    };

    template <class E, class L, bool is_const>
    typename xseries_iterator<E, L, is_const>::difference_type operator-(const xseries_iterator<E, L, is_const>& lhs,
                                                                         const xseries_iterator<E, L, is_const>& rhs);

    template <class E, class L, bool is_const>
    bool operator==(const xseries_iterator<E, L, is_const>& lhs, const xseries_iterator<E, L, is_const>& rhs);
    
    template <class E, class L, bool is_const>
    bool operator<(const xseries_iterator<E, L, is_const>& lhs, const xseries_iterator<E, L, is_const>& rhs);

    /**************************
     * xseries implementation *
     **************************/

    template <class E, class L>
    template <class AE, class AL>
    inline xseries<E, L>::xseries(AE&& e, AL&& labels)
        : m_axis(std::forward<AL>(labels)), m_expression(std::forward<AE>(e))
    {
    }

    template <class E, class L>
    inline auto xseries<E, L>::labels() const -> const label_list&
    {
        return m_axis.labels();
    }

    template <class E, class L>
    inline auto xseries<E, L>::values() -> expression_type&
    {
        return m_expression;
    }

    template <class E, class L>
    inline auto xseries<E, L>::values() const -> const expression_type&
    {
        return m_expression;
    }

    template <class E, class L>
    inline bool xseries<E, L>::empty() const
    {
        return m_axis.empty();
    }

    template <class E, class L>
    inline auto xseries<E, L>::size() const -> size_type
    {
        return m_axis.size();
    }

    template <class E, class L>
    inline bool xseries<E, L>::contains(const key_type& key) const
    {
        return m_axis.contains(key);
    }

    template <class E, class L>
    inline auto xseries<E, L>::operator[](const key_type& key) -> mapped_type&
    {
        size_type index = m_axis[key];
        return m_expression[index];
    }

    template <class E, class L>
    inline auto xseries<E, L>::operator[](const key_type& key) const -> const mapped_type&
    {
        size_type index = m_axis[key];
        return m_expression[index];
    }

    template <class E, class L>
    inline auto xseries<E, L>::begin() -> iterator
    {
        return iterator(m_axis.labels().cbegin(), m_expression.begin());
    }

    template <class E, class L>
    inline auto xseries<E, L>::end() -> iterator
    {
        return iterator(m_axis.labels().cend(), m_expression.end());
    }

    template <class E, class L>
    inline auto xseries<E, L>::begin() const -> const_iterator
    {
        return cbegin();
    }

    template <class E, class L>
    inline auto xseries<E, L>::end() const -> const_iterator
    {
        return cend();
    }

    template <class E, class L>
    inline auto xseries<E, L>::cbegin() const -> const_iterator
    {
        return const_iterator(m_axis.labels().cbegin(), m_expression.cbegin());
    }

    template <class E, class L>
    inline auto xseries<E, L>::cend() const -> const_iterator
    {
        return const_iterator(m_axis.labels().cend(), m_expression.cend());
    }

    template <class E, class L>
    inline auto xseries<E, L>::rbegin() -> reverse_iterator
    {
        return reverse_iterator(end());
    }

    template <class E, class L>
    inline auto xseries<E, L>::rend() -> reverse_iterator
    {
        return reverse_iterator(begin());
    }

    template <class E, class L>
    inline auto xseries<E, L>::rbegin() const -> const_reverse_iterator
    {
        return crbegin();
    }

    template <class E, class L>
    inline auto xseries<E, L>::rend() const -> const_reverse_iterator
    {
        return crend();
    }

    template <class E, class L>
    inline auto xseries<E, L>::crbegin() const -> const_reverse_iterator
    {
        return const_reverse_iterator(cend());
    }
        
    template <class E, class L>
    inline auto xseries<E, L>::crend() const -> const_reverse_iterator
    {
        return const_reverse_iterator(cbegin());
    }

    template <class E, class L>
    inline bool operator==(const xseries<E, L>& lhs, const xseries<E, L>& rhs)
    {
        return lhs.labels() == rhs.labels() && lhs.values() == rhs.values();
    }

    template <class E, class L>
    inline bool operator!=(const xseries<E, L>& lhs, const xseries<E, L>& rhs)
    {
        return !(lhs == rhs);
    }

    /***********************************
     * xseries_iterator implementation *
     ***********************************/

    template <class E, class L, bool is_const>
    inline xseries_iterator<E, L, is_const>::xseries_iterator(label_iterator lit, value_iterator vit)
        : m_lit(lit), m_vit(vit)
    {
    }

    template <class E, class L, bool is_const>
    inline auto xseries_iterator<E, L, is_const>::operator++() -> self_type&
    {
        ++m_lit;
        ++m_vit;
        return *this;
    }

    template <class E, class L, bool is_const>
    inline auto xseries_iterator<E, L, is_const>::operator--() -> self_type&
    {
        --m_lit;
        --m_vit;
        return *this;
    }

    template <class E, class L, bool is_const>
    inline auto xseries_iterator<E, L, is_const>::operator+=(difference_type n) -> self_type&
    {
        m_lit += n;
        m_vit += n;
        return *this;
    }

    template <class E, class L, bool is_const>
    inline auto xseries_iterator<E, L, is_const>::operator-=(difference_type n) -> self_type&
    {
        m_lit -= n;
        m_vit -= n;
        return *this;
    }

    template <class E, class L, bool is_const>
    inline auto xseries_iterator<E, L, is_const>::operator-(const self_type& rhs) const -> difference_type
    {
        return m_lit - rhs.m_lit;
    }

    template <class E, class L, bool is_const>
    inline auto xseries_iterator<E, L, is_const>::operator*() const -> reference
    {
        return reference(container_reference(*m_lit, *m_vit));
    }

    template <class E, class L, bool is_const>
    inline auto xseries_iterator<E, L, is_const>::operator->() const -> pointer
    {
        return pointer(container_reference(*m_lit, *m_vit));
    }

    template <class E, class L, bool is_const>
    inline bool xseries_iterator<E, L, is_const>::equal(const self_type& rhs) const
    {
        return m_lit == rhs.m_lit && m_vit == rhs.m_vit;
    }

    template <class E, class L, bool is_const>
    inline bool xseries_iterator<E, L, is_const>::less_than(const self_type& rhs) const
    {
        return m_lit < rhs.m_lit;
    }

    template <class E, class L, bool is_const>
    inline typename xseries_iterator<E, L, is_const>::difference_type operator-(const xseries_iterator<E, L, is_const>& lhs,
                                                                                const xseries_iterator<E, L, is_const>& rhs)
    {
        return lhs.operator-(rhs);
    }

    template <class E, class L, bool is_const>
    inline bool operator==(const xseries_iterator<E, L, is_const>& lhs, const xseries_iterator<E, L, is_const>& rhs)
    {
        return lhs.equal(rhs);
    }
    
    template <class E, class L, bool is_const>
    inline bool operator<(const xseries_iterator<E, L, is_const>& lhs, const xseries_iterator<E, L, is_const>& rhs)
    {
        return lhs.less_than(rhs);
    }
}

#endif

