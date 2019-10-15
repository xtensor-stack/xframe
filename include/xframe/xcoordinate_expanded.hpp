/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XCOORDINATE_EXPANDED_HPP
#define XFRAME_XCOORDINATE_EXPANDED_HPP

#include <type_traits>

#include "xtl/xiterator_base.hpp"

#include "xcoordinate.hpp"

namespace xf
{
    template <class C, class M>
    class xmap_expanded_iterator;

    /************************
     * xcoordinate_expanded *
     ************************/

    /**
     * @class xcoordinate_expanded
     * @brief view of an xcoordinate which expands the xcoordinate dimensions
     *
     * The xcoordinate_expanded is used for modeling an expanded view on an existing
     * xcoordinate, i.e. a superset of this xcoordinate. This is done by holding an
     * extra coordinate map and keeping a reference to the underlying xcoordinate.
     *
     * @tparam C the type of the underlying xcoordinate.
     * @sa xcoordinate
     * @sa xcoordinate_chain
     */
    template <class C>
    class xcoordinate_expanded
    {
    public:

        using coordinate_type = C;
        using self_type = xcoordinate_expanded<C>;
        using label_list = typename coordinate_type::label_list;
        using key_type = typename coordinate_type::key_type;
        using axis_type = typename coordinate_type::axis_type;
        using label_type = typename axis_type::key_type;
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
        using const_iterator = xmap_expanded_iterator<coordinate_type, map_type>;
        using key_iterator = xtl::xkey_iterator<self_type>;

        xcoordinate_expanded(const coordinate_type& sub_coord, const map_type& new_coord);
        xcoordinate_expanded(const coordinate_type& sub_coord, map_type&& new_coord);

        bool empty() const;
        size_type size() const;

        bool contains(const key_type& key) const;
        bool contains(const key_type& key, const label_type& label) const;
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
        map_type m_extra_coordinate;
    };

    template <class C, class K, class A>
    bool operator==(const xcoordinate_expanded<C>& lhs, const xcoordinate_base<K, A>& rhs);

    template <class C, class K, class A>
    bool operator==(const xcoordinate_base<K, A>& lhs, const xcoordinate_expanded<C>& rhs);

    template <class C, class K, class A>
    bool operator!=(const xcoordinate_expanded<C>& lhs, const xcoordinate_base<K, A>& rhs);

    template <class C, class K, class A>
    bool operator!=(const xcoordinate_base<K, A>& lhs, const xcoordinate_expanded<C>& rhs);

    template <class OS, class C>
    std::ostream& operator<<(std::ostream& out, const xcoordinate_expanded<C>& c);

    template <class C>
    xcoordinate_expanded<C> expand_dims(const C& coordinate, const typename C::map_type& new_coord);

    template <class C>
    xcoordinate_expanded<C> expand_dims(const C& coordinate, typename C::map_type&& new_coord);

    /**************************
     * xmap_expanded_iterator *
     **************************/

    template <class C, class M>
    class xmap_expanded_iterator
    {
    public:

        using self_type = xmap_expanded_iterator<C, M>;
        using coordinate_type = C;
        using map_type = M;
        using map_iterator = typename M::const_iterator;
        using value_type = typename map_iterator::value_type;
        using reference = typename map_iterator::reference;
        using pointer = typename map_iterator::pointer;
        using difference_type = typename map_iterator::difference_type;
        using iterator_category = typename map_iterator::iterator_category;

        xmap_expanded_iterator();
        xmap_expanded_iterator(map_iterator it, const coordinate_type* sub_coordinate, const map_type* extra_coordinate);

        self_type& operator++();
        self_type& operator--();

        self_type operator++(int);
        self_type operator--(int);

        reference operator*() const;
        pointer operator->() const;

        bool operator==(const self_type& rhs) const;
        bool operator!=(const self_type& rhs) const;

    private:

        map_iterator m_it;
        const coordinate_type* p_sub_coordinate;
        const map_type* p_extra_coordinate;
    };

    /***************************************
     * xcoordinate_expanded implementation *
     ***************************************/

    /**
     * Constructs an xcoordinate_expanded, given the reference to an xcoordinate
     * and the reference to the additional dimension mapping.
     * @param sub_coord the reference to the underlying xcoordinate.
     * @param new_coord the `dimension name <-> axis` mapping of the additional coordinates.
     */
    template <class C>
    inline xcoordinate_expanded<C>::xcoordinate_expanded(const coordinate_type& sub_coord, const map_type& new_coord)
        : m_sub_coordinate(sub_coord), m_extra_coordinate(new_coord)
    {
        check_consistency();
    }

    /**
     * Constructs an xcoordinate_expanded, given the reference to an xcoordinate
     * and the reference to the additional dimension mapping.
     * @param sub_coord the reference to the underlying xcoordinate.
     * @param new_coord the `dimension name <-> axis` mapping of the additional coordinates.
     */
    template <class C>
    inline xcoordinate_expanded<C>::xcoordinate_expanded(const coordinate_type& sub_coord, map_type&& new_coord)
        : m_sub_coordinate(sub_coord), m_extra_coordinate(std::move(new_coord))
    {
        check_consistency();
    }

    /**
     * Returns true if the coordinates is empty, i.e. it contains no mapping
     * of axes with dimension names.
     */
    template <class C>
    inline bool xcoordinate_expanded<C>::empty() const
    {
        return m_sub_coordinate.empty() && m_extra_coordinate.empty();
    }

    /**
     * Returns the number of axes in the coordinates.
     */
    template <class C>
    inline auto xcoordinate_expanded<C>::size() const -> size_type
    {
        return m_sub_coordinate.size() + m_extra_coordinate.size();
    }

    /**
     * Returns true if the coordinates contains the specified dimension
     * name.
     * @param key the dimension name to search for.
     */
    template <class C>
    inline bool xcoordinate_expanded<C>::contains(const key_type& key) const
    {
        return (m_extra_coordinate.find(key) != m_extra_coordinate.end()) || m_sub_coordinate.contains(key);
    }

    /**
     * Returns true if the coordinates contains the specified dimension
     * name and if the axis mapped with this name contains the specified
     * label.
     * @param key the dimension name to search for.
     * @param label the label to search for in the mapped axis.
     */
    template <class C>
    inline bool xcoordinate_expanded<C>::contains(const key_type& key, const label_type& label) const
    {
        auto iter = find(key);
        return iter != end() ? (iter->second).contains(label) : false;
    }

    /**
     * Returns the axis mapped to the specified dimension name. If this last one is not
     * found, throws an exception.
     * @param key the name of the dimension to search for.
     */
    template <class C>
    inline auto xcoordinate_expanded<C>::operator[](const key_type& key) const -> const mapped_type&
    {
        auto iter = m_extra_coordinate.find(key);
        return iter != m_extra_coordinate.end() ? iter->second : m_sub_coordinate[key];
    }

    /**
     * Returns the position of the specified labels of the axis mapped to the specified
     * dimension name. Throws an exception if either the dimension name or the label is
     * not part of this coordinate.
     * @param key the pair dimension name - label to search for.
     */
    template <class C>
    template <class KB, class LB>
    inline auto xcoordinate_expanded<C>::operator[](const std::pair<KB, LB>& key) const -> index_type
    {
        auto iter = m_extra_coordinate.find(key.first);
        return iter != m_extra_coordinate.end() ? (iter->second)[key.second] : m_sub_coordinate[key.first][key.second];
    }

    /**
     * Returns a constant iterator to the axis mapped to the specified dimension name.
     * If no such element is found, past-the-end iterator is returned.
     * @param key the dimension name to search for.
     */
    template <class C>
    inline auto xcoordinate_expanded<C>::find(const key_type& key) const -> const_iterator
    {
        auto iter = m_sub_coordinate.find(key);
        if(iter == m_sub_coordinate.end())
        {
            iter = m_extra_coordinate.find(key);
        }
        return const_iterator(iter, &m_sub_coordinate, &m_extra_coordinate);
    }

    /**
     * Returns a constant iterator to the first element of the coordinates. Such an element
     * is a pair dimension name - axis.
     */
    template <class C>
    inline auto xcoordinate_expanded<C>::begin() const noexcept -> const_iterator
    {
        return cbegin();
    }

    /**
     * Returns a constant iterator to the element following the last element of
     * the coordinates.
     */
    template <class C>
    inline auto xcoordinate_expanded<C>::end() const noexcept -> const_iterator
    {
        return cend();
    }

    /**
     * Returns a constant iterator to the first element of the coordinates. Such an element
     * is a pair dimension name - axis.
     */
    template <class C>
    inline auto xcoordinate_expanded<C>::cbegin() const noexcept -> const_iterator
    {
        return const_iterator(m_sub_coordinate.cbegin(), &m_sub_coordinate, &m_extra_coordinate);
    }

    /**
     * Returns a constant iterator to the element following the last element of
     * the coordinates.
     */
    template <class C>
    inline auto xcoordinate_expanded<C>::cend() const noexcept -> const_iterator
    {
        return const_iterator(m_extra_coordinate.cend(), &m_sub_coordinate, &m_extra_coordinate);
    }

    /**
     * Returns a constant iterator to the first dimension name of the coordinates.
     */
    template <class C>
    inline auto xcoordinate_expanded<C>::key_begin() const noexcept -> key_iterator
    {
        return key_iterator(begin());
    }

    /**
     * Returns a constant iterator to the element following the last dimension name of
     * the coordinates.
     */
    template <class C>
    inline auto xcoordinate_expanded<C>::key_end() const noexcept -> key_iterator
    {
        return key_iterator(end());
    }

    template <class C>
    inline bool xcoordinate_expanded<C>::operator==(const self_type& rhs) const noexcept
    {
        return m_sub_coordinate == rhs.m_sub_coordinate
            && m_extra_coordinate == rhs.m_extra_coordinate;
    }

    template <class C>
    inline bool xcoordinate_expanded<C>::operator!=(const self_type& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    template <class C>
    inline void xcoordinate_expanded<C>::check_consistency() const
    {
        for(const auto& a: m_extra_coordinate)
        {
            if(m_sub_coordinate.contains(a.first))
            {
                throw std::out_of_range("coordinate already there");
            }
        }
    }

    template <class C, class K, class A>
    inline bool operator==(const xcoordinate_expanded<C>& lhs, const xcoordinate_base<K, A>& rhs)
    {
        bool res = lhs.size() == rhs.size();
        auto iter = lhs.begin();
        auto end = lhs.end();
        auto rhs_end = rhs.end();
        while(res && iter != end)
        {
            auto rhs_iter = rhs.find(iter->first);
            res = (rhs_iter != rhs_end) && (iter->second == rhs_iter->second);
            ++iter;
        }
        return res;
    }

    template <class C, class K, class A>
    inline bool operator==(const xcoordinate_base<K, A>& lhs, const xcoordinate_expanded<C>& rhs)
    {
        return rhs == lhs;
    }

    template <class C, class K, class A>
    inline bool operator!=(const xcoordinate_expanded<C>& lhs, const xcoordinate_base<K, A>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class C, class K, class A>
    inline bool operator!=(const xcoordinate_base<K, A>& lhs, const xcoordinate_expanded<C>& rhs)
    {
        return rhs != lhs;
    }

    template <class C>
    inline std::ostream& operator<<(std::ostream& out, const xcoordinate_expanded<C>& c)
    {
        for (auto& v : c)
        {
            out << v.first << ": " << v.second << std::endl;
        }
        return out;
    }

    /**
     * Expand the dimensions of an xcoordinate, given the reference to an xcoordinate
     * and the reference to the additional dimension mapping.
     * @param sub_coord the reference to the underlying xcoordinate.
     * @param new_coord the `dimension name <-> axis` mapping of the additional coordinates.
     */
    template <class C>
    inline xcoordinate_expanded<C> expand_dims(const C& coordinate, const typename C::map_type& new_coord)
    {
        return xcoordinate_expanded<C>(coordinate, new_coord);
    }

    /**
     * Expand the dimensions of an xcoordinate, given the reference to an xcoordinate
     * and the reference to the additional dimension mapping.
     * @param sub_coord the reference to the underlying xcoordinate.
     * @param new_coord the `dimension name <-> axis` mapping of the additional coordinates.
     */
    template <class C>
    inline xcoordinate_expanded<C> expand_dims(const C& coordinate, typename C::map_type&& new_coord)
    {
        return xcoordinate_expanded<C>(coordinate, std::move(new_coord));
    }

    /*****************************************
     * xmap_expanded_iterator implementation *
     *****************************************/

    template <class C, class M>
    inline xmap_expanded_iterator<C, M>::xmap_expanded_iterator()
        : m_it(), p_sub_coordinate(nullptr), p_extra_coordinate(nullptr)
    {
    }

    template <class C, class M>
    inline xmap_expanded_iterator<C, M>::xmap_expanded_iterator(map_iterator it, const coordinate_type* sub_coordinate, const map_type* extra_coordinate)
        : m_it(it), p_sub_coordinate(sub_coordinate), p_extra_coordinate(extra_coordinate)
    {
        if (m_it == p_sub_coordinate->cend())
        {
            m_it = p_extra_coordinate->cbegin();
        }
    }

    template <class C, class M>
    inline auto xmap_expanded_iterator<C, M>::operator++() -> self_type&
    {
        ++m_it;
        if (m_it == p_sub_coordinate->cend())
        {
            m_it = p_extra_coordinate->cbegin();
        }
        return *this;
    }

    template <class C, class M>
    inline auto xmap_expanded_iterator<C, M>::operator--() -> self_type&
    {
        if (m_it == p_extra_coordinate->cbegin())
        {
            m_it = p_sub_coordinate->cend();
        }
        --m_it;
        return *this;
    }

    template <class C, class M>
    inline auto xmap_expanded_iterator<C, M>::operator++(int) -> self_type
    {
        self_type tmp(*this);
        ++(*this);
        return tmp;
    }

    template <class C, class M>
    inline auto xmap_expanded_iterator<C, M>::operator--(int) -> self_type
    {
        self_type tmp(*this);
        --(*this);
        return tmp;
    }

    template <class C, class M>
    inline auto xmap_expanded_iterator<C, M>::operator*() const -> reference
    {
        return *m_it;
    }

    template <class C, class M>
    inline auto xmap_expanded_iterator<C, M>::operator->() const -> pointer
    {
        return m_it.operator->();
    }

    template <class C, class M>
    inline bool xmap_expanded_iterator<C, M>::operator==(const self_type& rhs) const
    {
        return m_it == rhs.m_it;
    }

    template <class C, class M>
    inline bool xmap_expanded_iterator<C, M>::operator!=(const self_type& rhs) const
    {
        return !(*this == rhs);
    }
}

#endif
