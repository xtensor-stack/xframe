/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
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

    template <class K, class A>
    class xcoordinate_base
    {
    public:

        using self_type = xcoordinate_base<K, A>;
        using axis_type = A;
        using map_type = std::map<K, axis_type>;
        using key_type = typename map_type::key_type;
        using mapped_type = typename map_type::mapped_type;
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
        const mapped_type& operator[](const key_type& key) const;

        template <class KB, class LB>
        index_type operator[](const std::pair<KB, LB>& key) const;

        const map_type& data() const noexcept;

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

        template <class AX1, class... AX>
        void insert_impl(std::pair<K, AX1> axis, std::pair<K, AX>... axes);
        void insert_impl();

        map_type m_coordinate;
    };

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
    {
        insert_impl(std::move(axes)...);
    }

    template <class K, class A>
    inline bool xcoordinate_base<K, A>::empty() const
    {
        return m_coordinate.empty();
    }

    template <class K, class A>
    inline auto xcoordinate_base<K, A>::size() const -> size_type
    {
        return m_coordinate.size();
    }

    template <class K, class A>
    inline bool xcoordinate_base<K, A>::contains(const key_type& key) const
    {
        return m_coordinate.find(key) != m_coordinate.end();
    }

    template <class K, class A>
    inline auto xcoordinate_base<K, A>::operator[](const key_type& key) const -> const mapped_type&
    {
        return m_coordinate.at(key);
    }

    template <class K, class A>
    template <class KB, class LB>
    inline auto xcoordinate_base<K, A>::operator[](const std::pair<KB, LB>& key) const -> index_type
    {
        return (*this)[key.first][key.second];
    }

    template <class K, class A>
    inline auto xcoordinate_base<K, A>::data() const noexcept -> const map_type&
    {
        return m_coordinate;
    }

    template <class K, class A>
    inline auto xcoordinate_base<K, A>::begin() const noexcept -> const_iterator
    {
        return cbegin();
    }

    template <class K, class A>
    inline auto xcoordinate_base<K, A>::end() const noexcept -> const_iterator
    {
        return cend();
    }

    template <class K, class A>
    inline auto xcoordinate_base<K, A>::cbegin() const noexcept -> const_iterator
    {
        return m_coordinate.cbegin();
    }

    template <class K, class A>
    inline auto xcoordinate_base<K, A>::cend() const noexcept -> const_iterator
    {
        return m_coordinate.cend();
    }

    template <class K, class A>
    inline auto xcoordinate_base<K, A>::key_begin() const noexcept -> key_iterator
    {
        return key_iterator(begin());
    }

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

    template <class K, class A>
    template <class AX1, class... AX>
    inline void xcoordinate_base<K, A>::insert_impl(std::pair<K, AX1> axis, std::pair<K, AX>... axes)
    {
        m_coordinate.insert(value_type(std::move(axis.first), std::move(axis.second)));
        insert_impl(std::move(axes)...);
    }

    template <class K, class A>
    inline void xcoordinate_base<K, A>::insert_impl()
    {
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
