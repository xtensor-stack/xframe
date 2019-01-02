/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XCOORDINATE_VIEW_HPP
#define XFRAME_XCOORDINATE_VIEW_HPP

#include "xaxis_view.hpp"
#include "xcoordinate_base.hpp"

namespace xf
{

    /********************
     * xcoordinate_view *
     ********************/

    template <class K, class L = DEFAULT_LABEL_LIST, class S = std::size_t, class MT = hash_map_tag>
    class xcoordinate_view : public xcoordinate_base<K, xaxis_view<L, S, MT>>
    {
    public:

        using self_type = xcoordinate_view<K, L, S, MT>;
        using base_type = xcoordinate_base<K, xaxis_view<L, S, MT>>;
        using label_list = L;
        using axis_type = typename base_type::axis_type;
        using map_type = typename base_type::map_type;
        using key_type = typename base_type::key_type;
        using mapped_type = typename base_type::mapped_type;
        using index_type = typename base_type::index_type;
        using value_type = typename base_type::value_type;
        using reference = typename base_type::reference;
        using const_reference = typename base_type::const_reference;
        using pointer = typename base_type::pointer;
        using const_pointer = typename base_type::const_pointer;
        using size_type = typename base_type::size_type;
        using difference_type = typename base_type::difference_type;
        using iterator = typename base_type::iterator;
        using const_iterator = typename base_type::const_iterator;
        using key_iterator = typename base_type::key_iterator;

        explicit xcoordinate_view(const map_type& axes);
        explicit xcoordinate_view(map_type&& axes);
    };

    template <class K, class L, class S, class MT>
    xcoordinate_view<K, L, S, MT> coordinate_view(const std::map<K, xaxis_view<L, S, MT>>& axes);

    template <class K, class L, class S, class MT>
    xcoordinate_view<K, L, S, MT> coordinate_view(std::map<K, xaxis_view<L, S, MT>>&& axes);

    /*************************
     * xcoordinate_view_type *
     *************************/

    template <class K, class L, class S, class MT>
    class xcoordinate;

    template <class C>
    struct xcoordinate_view_type;

    template <class K, class L, class S, class MT>
    struct xcoordinate_view_type<xcoordinate<K, L, S, MT>>
    {
        using type = xcoordinate_view<K, L, S, MT>;
    };

    template <class C>
    using xcoordinate_view_type_t = typename xcoordinate_view_type<C>::type;

    /***********************************
     * xcoordinate_view implementation *
     ***********************************/

    template <class K, class L, class S, class MT>
    inline xcoordinate_view<K, L, S, MT>::xcoordinate_view(const map_type& axes)
        : base_type(axes)
    {
    }

    template <class K, class L, class S, class MT>
    inline xcoordinate_view<K, L, S, MT>::xcoordinate_view(map_type&& axes)
        : base_type(std::move(axes))
    {
    }

    template <class K, class L, class S, class MT>
    inline xcoordinate_view<K, L, S, MT> coordinate_view(const std::map<K, xaxis_view<L, S, MT>>& axes)
    {
        return xcoordinate_view<K, L, S, MT>(axes);
    }

    template <class K, class L, class S, class MT>
    inline xcoordinate_view<K, L, S, MT> coordinate_view(std::map<K, xaxis_view<L, S, MT>>&& axes)
    {
        return xcoordinate_view<K, L, S, MT>(std::move(axes));
    }
}

#endif
