/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XCOORDINATE_CHAIN_HPP
#define XFRAME_XCOORDINATE_CHAIN_HPP

#include <type_traits>
#include "xtl/xiterator_base.hpp"

namespace xf
{
    template <class M>
    class xmap_chain_iterator;

    namespace detail
    {
        template <class M>
        struct map_chain_traits
        {
            using key_type = typename M::key_type;
            using subiterator = xmap_chain_iterator<M>;
        };
    }

    /*********************
     * xcoordinate_chain *
     *********************/

    template <class C>
    class xcoordinate_chain
    {
    public:

        using coordinate_type = C;
        using self_type = xcoordinate_chain<C>;
        using label_list = typename coordinate_type::label_list;
        using key_type = typename coordinate_type::key_type;
        using axis_type = typename coordinate_type::axis_type;
        using map_type = typename coordinate_type::map_type;
        using mapped_type = typename coordinate_type::mapped_type;
        using index_type = typename coordinate_type::index_type;
        using value_type = typename coordinate_type::value_type;
        using reference = typename coordinate_type::reference;
        using const_reference = typename coordinate_type::const_reference;
        using pointer = typename coordinate_type::pointer;
        using const_pointer = typename coordinate_type::const_pointer;
        using size_type = typename coordinate_type::size_type;
        using difference_type = typename coordinate_type::difference_type;
        using const_iterator = xmap_chain_iterator<map_type>;
        using key_iterator = xtl::xkey_iterator<detail::map_chain_traits<map_type>>;

        xcoordinate_chain(const coordinate_type& sub_coord, const map_type& axes);
        xcoordinate_chain(const coordinate_type& sub_coord, map_type&& axes);

        bool empty() const;
        size_type size() const;

        bool contains(const key_type& key) const;
        const mapped_type& operator[](const key_type& key) const;

        template <class KB, class LB>
        index_type operator[](const std::pair<KB, LB>& key) const;

        // TODO: check if that's required and why
        //const map_type& data() const noexcept;

        const_iterator find(const key_type& key) const;

        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;

        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;

        key_iterator key_begin() const noexcept;
        key_iterator key_end() const noexcept;

        bool operator==(const self_type& rhs) const noexcept;
        bool operator!=(const self_type& rhs) const noexcept;

    private:

        void check_consistency() const;

        const coordinate_type& m_sub_coordinate;
        map_type m_coordinate;
    };

    template <class C>
    xcoordinate_chain<C> reindex(const C& coordinate, const typename C::map_type& axes);

    template <class C>
    xcoordinate_chain<C> reindex(const C& coordinate, typename C::map_type&& axes);

    /*******************
     * xiterator_chain *
     *******************/

    template <class M>
    class xmap_chain_iterator
    {
    public:

        using self_type = xmap_chain_iterator<M>;
        using map_type = M;
        using map_iterator = typename M::const_iterator;
        using value_type = typename map_iterator::value_type;
        using reference = typename map_iterator::reference;
        using pointer = typename map_iterator::pointer;
        using difference_type = typename map_iterator::difference_type;
        using iterator_category = typename map_iterator::iterator_category;

        xmap_chain_iterator();
        xmap_chain_iterator(map_iterator sub_it, const map_type* coordinate);

        self_type& operator++();
        self_type& operator--();

        self_type operator++(int);
        self_type operator--(int);

        reference operator*() const;
        pointer operator->() const;

        bool operator==(const self_type& rhs) const;
        bool operator!=(const self_type& rhs) const;

    private:

        void update_it();

        map_iterator m_sub_it;
        map_iterator m_it;
        const map_type* p_coordinate;
    };

    /************************************
     * xcoordinate_chain implementation *
     ************************************/

    template <class C>
    inline xcoordinate_chain<C>::xcoordinate_chain(const coordinate_type& sub_coord, const map_type& axes)
        : m_sub_coordinate(sub_coord), m_coordinate(axes)
    {
        check_consistency();
    }

    template <class C>
    inline xcoordinate_chain<C>::xcoordinate_chain(const coordinate_type& sub_coord, map_type&& axes)
        : m_sub_coordinate(sub_coord), m_coordinate(std::move(axes))
    {
        check_consistency();
    }

    template <class C>
    inline bool xcoordinate_chain<C>::empty() const
    {
        return m_sub_coordinate.empty();
    }

    template <class C>
    inline auto xcoordinate_chain<C>::size() const -> size_type
    {
        return m_sub_coordinate.size();
    }

    template <class C>
    inline bool xcoordinate_chain<C>::contains(const key_type& key) const
    {
        return (m_coordinate.find(key) != m_coordinate.end()) || m_sub_coordinate.contains(key);
    }

    template <class C>
    inline auto xcoordinate_chain<C>::operator[](const key_type& key) const -> const mapped_type&
    {
        auto iter = m_coordinate.find(key);
        return iter != m_coordinate.end() ? iter->second : m_sub_coordinate[key];
    }

    template <class C>
    template <class KB, class LB>
    inline auto xcoordinate_chain<C>::operator[](const std::pair<KB, LB>& key) const -> index_type
    {
        auto iter = m_coordinate.find(key.first);
        return iter != m_coordinate.end() ? (iter->second)[key.second] : m_sub_coordinate[key.first][key.second];
    }
    
    template <class C>
    inline auto xcoordinate_chain<C>::find(const key_type& key) const -> const_iterator
    {
        auto iter = m_coordinate.find(key);
        if(iter == m_coordinate.end())
        {
            iter = m_sub_coordinate.find(key);
        }
        return const_iterator(m_coordinate, m_sub_coordinate, iter);
    }

    template <class C>
    inline auto xcoordinate_chain<C>::begin() const noexcept -> const_iterator
    {
        return cbegin();
    }

    template <class C>
    inline auto xcoordinate_chain<C>::end() const noexcept -> const_iterator
    {
        return cend();
    }

    template <class C>
    inline auto xcoordinate_chain<C>::cbegin() const noexcept -> const_iterator
    {
        return const_iterator(m_sub_coordinate.cbegin(), &m_coordinate);
    }

    template <class C>
    inline auto xcoordinate_chain<C>::cend() const noexcept -> const_iterator
    {
        return const_iterator(m_sub_coordinate.cend(), &m_coordinate);
    }

    template <class C>
    inline auto xcoordinate_chain<C>::key_begin() const noexcept -> key_iterator
    {
        return key_iterator(begin());
    }

    template <class C>
    inline auto xcoordinate_chain<C>::key_end() const noexcept -> key_iterator
    {
        return key_iterator(end());
    }

    template <class C>
    inline bool xcoordinate_chain<C>::operator==(const self_type& rhs) const noexcept
    {
        return m_sub_coordinate == rhs.m_sub_coordinate
            && m_coordinate == rhs.m_coordinate;
    }

    template <class C>
    inline bool xcoordinate_chain<C>::operator!=(const self_type& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    template <class C>
    inline void xcoordinate_chain<C>::check_consistency() const
    {
        for(const auto& a: m_coordinate)
        {
            if(!m_sub_coordinate.contains(a.first))
            {
                throw std::out_of_range("coordinate not found");
            }
        }
    }

    template <class C>
    inline xcoordinate_chain<C> reindex(const C& coordinate, const typename C::map_type& axes)
    {
        return xcoordinate_chain<C>(coordinate, axes);
    }

    template <class C>
    inline xcoordinate_chain<C> reindex(const C& coordinate, typename C::map_type&& axes)
    {
        return xcoordinate_chain<C>(coordinate, std::move(axes));
    }

    /**************************************
     * xmap_chain_iterator implementation *
     **************************************/

    template <class M>
    inline xmap_chain_iterator<M>::xmap_chain_iterator()
        : m_sub_it(), m_it(), p_coordinate(nullptr)
    {
    }

    template <class M>
    inline xmap_chain_iterator<M>::xmap_chain_iterator(map_iterator sub_it, const map_type* coordinate)
        : m_sub_it(sub_it), m_it(), p_coordinate(coordinate)
    {
        update_it();
    }

    template <class M>
    inline auto xmap_chain_iterator<M>::operator++() -> self_type&
    {
        ++m_sub_it;
        update_it();
        return *this;
    }

    template <class M>
    inline auto xmap_chain_iterator<M>::operator--() -> self_type&
    {
        --m_it;
        update_it();
        return *this;
    }

    template <class M>
    inline auto xmap_chain_iterator<M>::operator++(int) -> self_type
    {
        self_type tmp(*this);
        ++(*this);
        return tmp;
    }

    template <class M>
    inline auto xmap_chain_iterator<M>::operator--(int) -> self_type
    {
        self_type tmp(*this);
        --(*this);
        return tmp;
    }

    template <class M>
    inline auto xmap_chain_iterator<M>::operator*() const -> reference
    {
        return *m_it;
    }

    template <class M>
    inline auto xmap_chain_iterator<M>::operator->() const -> pointer
    {
        return m_it.operator->();
    }

    template <class M>
    inline bool xmap_chain_iterator<M>::operator==(const self_type& rhs) const
    {
        return m_it == rhs.m_it;
    }

    template <class M>
    inline bool xmap_chain_iterator<M>::operator!=(const self_type& rhs) const
    {
        return !(*this == rhs);
    }

    template <class M>
    inline void xmap_chain_iterator<M>::update_it()
    {
        m_it = p_coordinate->find(m_sub_it->first);
        if(m_it == p_coordinate->end())
        {
            m_it = m_sub_it;
        }
    }
}

#endif
