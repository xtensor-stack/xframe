/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XSELECTING_HPP
#define XFRAME_XSELECTING_HPP

#include <limits>
#include "xtl/xsequence.hpp"
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
    }

    /*************
     * xselector *
     *************/

    template <class C, std::size_t N>
    class xselector
    {
    public:

        static_assert(is_coordinate<C>::value, "first parameter of xselector must be xcoordinate");

        using self_type = xselector<C, N>;
        using coordinate_type = C;
        using key_type = typename coordinate_type::key_type;
        using label_list = typename coordinate_type::label_list;
        using mapped_type = mpl::cast_t<label_list, xtl::variant>;
        using size_type = typename coordinate_type::index_type;
        using index_type = detail::xselector_index_t<size_type, N>;
        using dimension_type = xaxis<key_type, size_type>;
        using map_type = std::map<key_type, mapped_type>;
        
        xselector() = default;
        xselector(const map_type& coord);
        xselector(map_type&& coord);

        index_type get_index(const coordinate_type& coord, const dimension_type& dim) const;

    private:

        map_type m_coord;
    };

    /**************
     * xiselector *
     **************/

    template <class C, std::size_t N>
    class xiselector
    {
    public:

        static_assert(is_coordinate<C>::value, "first parameter of xiselector must be xcoordinate");

        using self_type = xiselector<C, N>;
        using coordinate_type = C;
        using key_type = typename coordinate_type::key_type;
        using size_type = typename coordinate_type::index_type;
        using index_type = detail::xselector_index_t<size_type, N>;
        using dimension_type = xaxis<key_type, size_type>;
        using map_type = std::map<key_type, size_type>;

        xiselector() = default;
        xiselector(const map_type& coord);
        xiselector(map_type&& coord);

        index_type get_index(const coordinate_type& coord, const dimension_type& dim) const;

    private:

        map_type m_coord;
    };

    /****************************
     * xselector implementation *
     ****************************/

    template <class C, std::size_t N>
    inline xselector<C, N>::xselector(const map_type& coord)
        : m_coord(coord)
    {
    }

    template <class C, std::size_t N>
    inline xselector<C, N>::xselector(map_type&& coord)
        : m_coord(std::move(coord))
    {
    }

    template <class C, std::size_t N>
    inline auto xselector<C, N>::get_index(const coordinate_type& coord, const dimension_type& dim) const
        -> index_type
    {
        index_type res = xtl::make_sequence<index_type>(m_coord.size(), size_type(0));
        for(const auto& c : m_coord)
        {
            res[dim[c.first]] = coord[c.first][c.second];
        }
        return res;
    }

    /*****************************
     * xiselector implementation *
     *****************************/

    template <class C, std::size_t N>
    inline xiselector<C, N>::xiselector(const map_type& coord)
        : m_coord(coord)
    {
    }

    template <class C, std::size_t N>
    inline xiselector<C, N>::xiselector(map_type&& coord)
        : m_coord(std::move(coord))
    {
    }

    template <class C, std::size_t N>
    inline auto xiselector<C, N>::get_index(const coordinate_type& coord, const dimension_type& dim) const
        -> index_type
    {
        index_type res = xtl::make_sequence<index_type>(m_coord.size(), size_type(0));
        for(const auto& c : m_coord)
        {
            res[dim[c.first]] = c.second;
        }
        return res;
    }
}

#endif

