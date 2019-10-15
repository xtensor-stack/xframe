/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XCOORDINATE_BASE_HPP
#define XFRAME_XCOORDINATE_BASE_HPP

#include <map>

#include "xtl/xiterator_base.hpp"
#include "xaxis_variant.hpp"
#include "xframe_config.hpp"

namespace xf
{

    /********************
     * xcoordinate_base *
     ********************/

    /**
     * @class xcoordinate_base
     * @brief Base class for coordinates.
     *
     * The xcoordinate_base class defines the common interface for coordinates,
     * which define the mapping of dimension names to axes.
     *
     * @tparam D The derived type, i.e. the inheriting class for which xcoordinate_base
     *           provides the interface.
     */
    template <class K, class A>
    class xcoordinate_base
    {
    public:

        using self_type = xcoordinate_base<K, A>;
        using axis_type = A;
        using map_type = std::map<K, axis_type>;
        using key_type = typename map_type::key_type;
        using mapped_type = typename map_type::mapped_type;
        using label_type = typename axis_type::key_type;
        using index_type = typename axis_type::mapped_type;
        using value_type = typename map_type::value_type;
        using reference = typename map_type::reference;
        using const_reference = typename map_type::const_reference;
        using pointer = typename map_type::pointer;
        using const_pointer = typename map_type::const_pointer;
        using size_type = typename map_type::size_type;
        using difference_type = typename map_type::difference_type;
        using iterator = typename map_type::iterator;
        using const_iterator = typename map_type::const_iterator;
        using key_iterator = xtl::xkey_iterator<map_type>;

        bool empty() const;
        size_type size() const;

        bool contains(const key_type& key) const;
        bool contains(const key_type& key, const label_type& label) const;

        const mapped_type& operator[](const key_type& key) const;
        template <class KB, class LB>
        index_type operator[](const std::pair<KB, LB>& key) const;

        const map_type& data() const noexcept;

        const_iterator find(const key_type& key) const;

        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;

        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;

        key_iterator key_begin() const noexcept;
        key_iterator key_end() const noexcept;

    protected:

        xcoordinate_base(const map_type& axes);
        xcoordinate_base(map_type&& axes);
        xcoordinate_base(std::initializer_list<value_type> init);
        template <class... AX>
        xcoordinate_base(std::pair<K, AX>... axes);

        ~xcoordinate_base() = default;

        xcoordinate_base(const xcoordinate_base&) = default;
        xcoordinate_base& operator=(const xcoordinate_base&) = default;

        xcoordinate_base(xcoordinate_base&&) = default;
        xcoordinate_base& operator=(xcoordinate_base&&) = default;

        map_type& coordinate() noexcept;

    private:

        map_type m_coordinate;
    };

    template <class K, class A1, class A2>
    bool operator==(const xcoordinate_base<K, A1>& lhs, const xcoordinate_base<K, A2>& rhs);

    template <class K, class A1, class A2>
    bool operator!=(const xcoordinate_base<K, A1>& lhs, const xcoordinate_base<K, A2>& rhs);

    template <class OS, class K, class A>
    OS& operator<<(OS& out, const xcoordinate_base<K, A>& c);

    /***********************************
     * xcoordinate_base implementation *
     ***********************************/

    template <class K, class A>
    inline xcoordinate_base<K, A>::xcoordinate_base(const map_type& axes)
        : m_coordinate(axes)
    {
    }

    template <class K, class A>
    inline xcoordinate_base<K, A>::xcoordinate_base(map_type&& axes)
        : m_coordinate(std::move(axes))
    {
    }

    template <class K, class A>
    inline xcoordinate_base<K, A>::xcoordinate_base(std::initializer_list<value_type> init)
        : m_coordinate(init)
    {
    }

    template <class K, class A>
    template <class... AX>
    inline xcoordinate_base<K, A>::xcoordinate_base(std::pair<K, AX>... axes)
        : m_coordinate({std::move(axes)...})
    {
    }

    /**
     * Returns true if the coordinates is empty, i.e. it contains no mapping
     * of axes with dimension names.
     */
    template <class K, class A>
    inline bool xcoordinate_base<K, A>::empty() const
    {
        return m_coordinate.empty();
    }

    /**
     * Returns the number of axes in the coordinates.
     */
    template <class K, class A>
    inline auto xcoordinate_base<K, A>::size() const -> size_type
    {
        return m_coordinate.size();
    }

    /**
     * Returns true if the coordinates contains the specified dimension
     * name.
     * @param key the dimension name to search for.
     */
    template <class K, class A>
    inline bool xcoordinate_base<K, A>::contains(const key_type& key) const
    {
        return m_coordinate.find(key) != m_coordinate.end();
    }

    /**
     * Returns true if the coordinates contains the specified dimension
     * name and if the axis mapped with this name contains the specified
     * label.
     * @param key the dimension name to search for.
     * @param label the label to search for in the mapped axis.
     */
    template <class K, class A>
    inline bool xcoordinate_base<K, A>::contains(const key_type& key, const label_type& label) const
    {
        auto iter = m_coordinate.find(key);
        return iter != m_coordinate.end() ? (iter->second).contains(label) : false;
    }

    /**
     * Returns the axis mapped to the specified dimension name. If this last one is not
     * found, throws an exception.
     * @param key the name of the dimension to search for.
     */
    template <class K, class A>
    inline auto xcoordinate_base<K, A>::operator[](const key_type& key) const -> const mapped_type&
    {
        return m_coordinate.at(key);
    }

    /**
     * Returns the position of the specified labels of the axis mapped to the specified
     * dimension name. Throws an exception if either the dimension name or the label is
     * not part of this coordinate.
     * @param key the pair dimension name - label to search for.
     */
    template <class K, class A>
    template <class KB, class LB>
    inline auto xcoordinate_base<K, A>::operator[](const std::pair<KB, LB>& key) const -> index_type
    {
        return (*this)[key.first][key.second];
    }

    /**
     * Returns the container of the dimension names to axes mapping.
     */
    template <class K, class A>
    inline auto xcoordinate_base<K, A>::data() const noexcept -> const map_type&
    {
        return m_coordinate;
    }

    /**
     * Returns a constant iterator to the axis mapped to the specified dimension name.
     * If no such element is found, past-the-end iterator is returned.
     * @param key the dimension name to search for.
     */
    template <class K, class A>
    inline auto xcoordinate_base<K, A>::find(const key_type& key) const -> const_iterator
    {
        return m_coordinate.find(key);
    }

    /**
     * Returns a constant iterator to the first element of the coordinates. Such an element
     * is a pair dimension name - axis.
     */
    template <class K, class A>
    inline auto xcoordinate_base<K, A>::begin() const noexcept -> const_iterator
    {
        return cbegin();
    }

    /**
     * Returns a constant iterator to the element following the last element of
     * the coordinates.
     */
    template <class K, class A>
    inline auto xcoordinate_base<K, A>::end() const noexcept -> const_iterator
    {
        return cend();
    }

    /**
     * Returns a constant iterator to the first element of the coordinates. Such an element
     * is a pair dimension name - axis.
     */
    template <class K, class A>
    inline auto xcoordinate_base<K, A>::cbegin() const noexcept -> const_iterator
    {
        return m_coordinate.cbegin();
    }

    /**
     * Returns a constant iterator to the element following the last element of
     * the coordinates.
     */
    template <class K, class A>
    inline auto xcoordinate_base<K, A>::cend() const noexcept -> const_iterator
    {
        return m_coordinate.cend();
    }

    /**
     * Returns a constant iterator to the first dimension name of the coordinates.
     */
    template <class K, class A>
    inline auto xcoordinate_base<K, A>::key_begin() const noexcept -> key_iterator
    {
        return key_iterator(begin());
    }

    /**
     * Returns a constant iterator to the element following the last dimension name of
     * the coordinates.
     */
    template <class K, class A>
    inline auto xcoordinate_base<K, A>::key_end() const noexcept -> key_iterator
    {
        return key_iterator(end());
    }

    template <class K, class A>
    inline auto xcoordinate_base<K, A>::coordinate() noexcept -> map_type&
    {
        return m_coordinate;
    }

    /**
     * Returns true if \c lhs and \c rhs are equivalent coordinates, i.e. they hold the same
     * axes mapped to the same dimension names.
     * @param lhs a coordinate object.
     * @param rhs a coordinate object.
     */
    template <class K, class A1, class A2>
    inline bool operator==(const xcoordinate_base<K, A1>& lhs, const xcoordinate_base<K, A2>& rhs)
    {
        bool res = lhs.size() == rhs.size();

        auto liter = lhs.cbegin();
        auto lend = lhs.cend();
        auto riter = rhs.cbegin();
        while(res && liter != lend)
        {
            res = (liter->second == riter->second);
            ++liter;
            ++riter;
        }
        return res;
    }

    /**
     * Returns true if \c lhs and \c rhs are not equivalent coordinates, i.e. they hold different 
     * axes or the dimension names to axes mappings are different.
     * @param lhs a coordinate object.
     * @param rhs a coordinate object.
     */
    template <class K, class A1, class A2>
    inline bool operator!=(const xcoordinate_base<K, A1>& lhs, const xcoordinate_base<K, A2>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class OS, class K, class A>
    inline OS& operator<<(OS& out, const xcoordinate_base<K, A>& c)
    {
        for (auto& v : c)
        {
            out << v.first << ": " << v.second << std::endl;
        }
        return out;
    }
}

#endif
