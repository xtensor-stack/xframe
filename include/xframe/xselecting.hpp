/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XSELECTING_HPP
#define XFRAME_XSELECTING_HPP

#include <limits>
#include "xtl/xsequence.hpp"
#include "xtl/xoptional.hpp"
#include "xcoordinate.hpp"

namespace xf
{
    namespace mpl = xtl::mpl;

    namespace detail
    {
        template <class S, std::size_t N>
        struct xselector_index
        {
            using type = std::conditional_t<N == std::numeric_limits<S>::max(), std::vector<S>, std::array<S, N>>;
        };

        template <class S, std::size_t N>
        using xselector_index_t = typename xselector_index<S, N>::type;

        template <class T>
        const T& static_missing() noexcept
        {
            using value_type = typename T::value_type;
            using flag_type = typename T::flag_type;
            static T res = xtl::missing<value_type>();
            return res;
        }
    }

    /*************
     * xselector *
     *************/

    template <class C, class D, std::size_t N>
    class xselector
    {
    public:

        static_assert(is_coordinate<C>::value, "first parameter of xselector must be xcoordinate");
        static_assert(is_dimension<D>::value, "second parameter of xselector must be xdimension");

        using coordinate_type = C;
        using key_type = typename coordinate_type::key_type;
        using label_list = typename coordinate_type::label_list;
        using mapped_type = mpl::cast_t<label_list, xtl::variant>;
        using size_type = typename coordinate_type::index_type;
        using index_type = detail::xselector_index_t<size_type, N>;
        using dimension_type = D;
        using map_type = std::map<key_type, mapped_type>;
        
        xselector() = default;
        xselector(const map_type& coord);
        xselector(map_type&& coord);

        index_type get_index(const coordinate_type& coord, const dimension_type& dim) const;

        template <class V>
        typename V::const_reference get_outer_value(const V& v, const coordinate_type& coord, const dimension_type& dim) const;

    private:

        map_type m_coord;
    };

    /**************
     * xiselector *
     **************/

    template <class C, class D, std::size_t N>
    class xiselector
    {
    public:

        static_assert(is_coordinate<C>::value, "first parameter of xiselector must be xcoordinate");
        static_assert(is_dimension<D>::value, "second parameter of xiselector must be xdimension");

        using coordinate_type = C;
        using key_type = typename coordinate_type::key_type;
        using size_type = typename coordinate_type::index_type;
        using index_type = detail::xselector_index_t<size_type, N>;
        using dimension_type = D;
        using map_type = std::map<key_type, size_type>;

        xiselector() = default;
        xiselector(const map_type& coord);
        xiselector(map_type&& coord);

        index_type get_index(const coordinate_type& coord, const dimension_type& dim) const;

    private:

        map_type m_coord;
    };

    template <class C, class D, std::size_t N>
    class xlocator
    {
    public:

        static_assert(is_coordinate<C>::value, "first parameter of xlocator must be xcoordinate");
        static_assert(is_dimension<D>::value, "second parameter of xiselector must be xdimension");

        using coordinate_type = C;
        using key_type = typename coordinate_type::key_type;
        using label_list = typename coordinate_type::label_list;
        using mapped_type = mpl::cast_t<label_list, xtl::variant>;
        using size_type = typename coordinate_type::index_type;
        using index_type = detail::xselector_index_t<size_type, N>;
        using dimension_type = D;
        using map_type = std::map<size_type, mapped_type>;

        xlocator() = default;
        xlocator(const map_type& coord);
        xlocator(map_type&& coord);

        index_type get_index(const coordinate_type& coord, const dimension_type& dim) const;

    private:

        map_type m_coord;
    };

    /****************************
     * xselector implementation *
     ****************************/

    template <class C, class D, std::size_t N>
    inline xselector<C, D, N>::xselector(const map_type& coord)
        : m_coord(coord)
    {
    }

    template <class C, class D, std::size_t N>
    inline xselector<C, D, N>::xselector(map_type&& coord)
        : m_coord(std::move(coord))
    {
    }

    template <class C, class D, std::size_t N>
    inline auto xselector<C, D, N>::get_index(const coordinate_type& coord, const dimension_type& dim) const
        -> index_type
    {
        index_type res = xtl::make_sequence<index_type>(m_coord.size(), size_type(0));
        for(const auto& c : m_coord)
        {
            auto iter = dim.find(c.first);
            if(iter != dim.end())
            {
                res[iter->second] = coord[c.first][c.second];
            }
        }
        return res;
    }

    template <class C, class D, std::size_t N>
    template <class V>
    inline auto xselector<C, D, N>::get_outer_value(const V& v, const coordinate_type& coord, const dimension_type& dim) const
        -> typename V::const_reference
    {
        index_type idx = xtl::make_sequence<index_type>(m_coord.size(), size_type(0));
        for(const auto& c : m_coord)
        {
            auto iter = dim.find(c.first);
            if(iter != dim.end())
            {
                const auto& axis = coord[c.first];
                if(axis.contains(c.second))
                {
                    idx[iter->second]= axis[c.second];
                }
                else
                {
                    return detail::static_missing<typename V::value_type>();
                }
            }
        }
        return v.data().element(idx.cbegin(), idx.cend());
    }

    /*****************************
     * xiselector implementation *
     *****************************/

    template <class C, class D, std::size_t N>
    inline xiselector<C, D, N>::xiselector(const map_type& coord)
        : m_coord(coord)
    {
    }

    template <class C, class D, std::size_t N>
    inline xiselector<C, D, N>::xiselector(map_type&& coord)
        : m_coord(std::move(coord))
    {
    }

    template <class C, class D, std::size_t N>
    inline auto xiselector<C, D, N>::get_index(const coordinate_type& coord, const dimension_type& dim) const
        -> index_type
    {
        index_type res = xtl::make_sequence<index_type>(m_coord.size(), size_type(0));
        for(const auto& c : m_coord)
        {
            res[dim[c.first]] = c.second;
        }
        return res;
    }

    /***************************
     * xlocator implementation *
     ***************************/

    template <class C, class D, std::size_t N>
    inline xlocator<C, D, N>::xlocator(const map_type& coord)
        : m_coord(coord)
    {
    }

    template <class C, class D, std::size_t N>
    inline xlocator<C, D, N>::xlocator(map_type&& coord)
        : m_coord(std::move(coord))
    {
    }

    template <class C, class D, std::size_t N>
    inline auto xlocator<C, D, N>::get_index(const coordinate_type& coord, const dimension_type& dim) const
        -> index_type
    {
        index_type res = xtl::make_sequence<index_type>(m_coord.size(), size_type(0));
        for(const auto& c : m_coord)
        {
            res[c.first] = coord[dim.labels()[c.first]][c.second];
        }
        return res;
    }

}

#endif

