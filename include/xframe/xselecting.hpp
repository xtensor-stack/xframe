/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XSELECTING_HPP
#define XFRAME_XSELECTING_HPP

#include <limits>

#include "xtl/xmasked_value.hpp"
#include "xtl/xsequence.hpp"
#include "xtl/xoptional.hpp"

#include "xframe_utils.hpp"
#include "xcoordinate.hpp"
#include "xdimension.hpp"

namespace xf
{
    namespace mpl = xtl::mpl;

    namespace detail
    {
        template <class T>
        struct static_missing_impl;

        template <class T, class B>
        struct static_missing_impl<xtl::xoptional<const T&, const B&>>
        {
            using return_type = xtl::xoptional<const T&, const B&>;
            static inline return_type get()
            {
                static T val = T(0);
                static B has_val = false;
                return return_type(val, has_val);
            }
        };

        template <class T, class B>
        struct static_missing_impl<xtl::xmasked_value<T, B>>
        {
            using return_type = xtl::xmasked_value<T, B>;
            static inline return_type get()
            {
                static T val = static_missing_impl<T>::get();
                static B visible = true;
                return return_type(val, visible);
            }
        };

        template <class T>
        T static_missing() noexcept
        {
            return static_missing_impl<T>::get();
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
        using index_type = detail::xselector_sequence_t<size_type, N>;
        using outer_index_type = std::pair<index_type, bool>;
        using dimension_type = D;
        using sequence_type = detail::xselector_sequence_t<std::pair<key_type, mapped_type>, N>;

        xselector() = default;
        xselector(const sequence_type& coord);
        xselector(sequence_type&& coord);

        index_type get_index(const coordinate_type& coord, const dimension_type& dim) const;
        outer_index_type get_outer_index(const coordinate_type& coord, const dimension_type& dim) const;

    private:

        sequence_type m_coord;
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
        using index_type = detail::xselector_sequence_t<size_type, N>;
        using dimension_type = D;
        using sequence_type = detail::xselector_sequence_t<std::pair<key_type, size_type>, N>;

        xiselector() = default;
        xiselector(const sequence_type& coord);
        xiselector(sequence_type&& coord);

        index_type get_index(const coordinate_type& coord, const dimension_type& dim) const;

    private:

        sequence_type m_coord;
    };

    /************
     * xlocator *
     ************/

    template <class C, class D, std::size_t N>
    class xlocator
    {
    public:

        static_assert(is_coordinate<C>::value, "first parameter of xlocator must be xcoordinate");
        static_assert(is_dimension<D>::value, "second parameter of xlocator must be xdimension");

        using coordinate_type = C;
        using key_type = typename coordinate_type::key_type;
        using label_list = typename coordinate_type::label_list;
        using mapped_type = mpl::cast_t<label_list, xtl::variant>;
        using size_type = typename coordinate_type::index_type;
        using index_type = detail::xselector_sequence_t<size_type, N>;
        using dimension_type = D;
        using sequence_type = detail::xselector_sequence_t<mapped_type, N>;

        xlocator() = default;
        xlocator(const sequence_type& coord);
        xlocator(sequence_type&& coord);

        index_type get_index(const coordinate_type& coord, const dimension_type& dim) const;

    private:

        sequence_type m_coord;
    };

    /********************
     * xselector_traits *
     ********************/

    template <class C, class DM, std::size_t N>
    struct xselector_traits
    {
        using coordinate_type = C;
        using dimension_type = DM;

        using size_type = typename coordinate_type::index_type;
        using index_type = detail::xselector_sequence_t<size_type, N>;
        using selector_type = xselector<coordinate_type, dimension_type, N>;
        using selector_sequence_type = typename selector_type::sequence_type;
        using iselector_type = xiselector<coordinate_type, dimension_type, N>;
        using iselector_sequence_type = typename iselector_type::sequence_type;
        using locator_type = xlocator<coordinate_type, dimension_type, N>;
        using locator_sequence_type = typename locator_type::sequence_type;

        static constexpr std::size_t static_dimension = N;
    };

    /****************************
     * xselector implementation *
     ****************************/

    template <class C, class D, std::size_t N>
    inline xselector<C, D, N>::xselector(const sequence_type& coord)
        : m_coord(coord)
    {
    }

    template <class C, class D, std::size_t N>
    inline xselector<C, D, N>::xselector(sequence_type&& coord)
        : m_coord(std::move(coord))
    {
    }

    template <class C, class D, std::size_t N>
    inline auto xselector<C, D, N>::get_index(const coordinate_type& coord, const dimension_type& dim) const
        -> index_type
    {
        index_type res = xtl::make_sequence<index_type>(dim.size(), size_type(0));
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
    inline auto xselector<C, D, N>::get_outer_index(const coordinate_type& coord, const dimension_type& dim) const
        -> outer_index_type
    {
        outer_index_type res(xtl::make_sequence<index_type>(dim.size(), size_type(0)), true);
        for(const auto& c : m_coord)
        {
            auto iter = dim.find(c.first);
            if(iter != dim.end())
            {
                const auto& axis = coord[c.first];
                if(axis.contains(c.second))
                {
                    res.first[iter->second]= axis[c.second];
                }
                else
                {
                    res.second = false;
                    break;
                }
            }
        }
        return res;
    }

    /*****************************
     * xiselector implementation *
     *****************************/

    template <class C, class D, std::size_t N>
    inline xiselector<C, D, N>::xiselector(const sequence_type& coord)
        : m_coord(coord)
    {
    }

    template <class C, class D, std::size_t N>
    inline xiselector<C, D, N>::xiselector(sequence_type&& coord)
        : m_coord(std::move(coord))
    {
    }

    template <class C, class D, std::size_t N>
    inline auto xiselector<C, D, N>::get_index(const coordinate_type& /*coord*/, const dimension_type& dim) const
        -> index_type
    {
        index_type res = xtl::make_sequence<index_type>(dim.size(), size_type(0));
        for(const auto& c : m_coord)
        {
            auto iter = dim.find(c.first);
            if (iter != dim.end())
            {
                res[iter->second] = c.second;
            }
        }
        return res;
    }

    /***************************
     * xlocator implementation *
     ***************************/

    template <class C, class D, std::size_t N>
    inline xlocator<C, D, N>::xlocator(const sequence_type& coord)
        : m_coord(coord)
    {
    }

    template <class C, class D, std::size_t N>
    inline xlocator<C, D, N>::xlocator(sequence_type&& coord)
        : m_coord(std::move(coord))
    {
    }

    template <class C, class D, std::size_t N>
    inline auto xlocator<C, D, N>::get_index(const coordinate_type& coord, const dimension_type& dim) const
        -> index_type
    {
        index_type res = xtl::make_sequence<index_type>(dim.size(), size_type(0));
        for (std::size_t i = 0; i < m_coord.size(); ++i)
        {
            res[i] = coord[dim.labels()[i]][m_coord[i]];
        }
        return res;
    }

}

#endif
