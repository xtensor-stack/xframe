/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
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

    /**
     * @class xcoordinate_view
     * @brief view of an xcoordinate
     *
     * The xcoordinate_view is used for modeling a view on an existing xcoordinate,
     * i.e. a subset of this xcoordinate. This is done by either ignoring some axes,
     * or by holding views on axes of the underlying xcoordinate.
     * 
     * @tparam K the type of dimension names.
     * @tparam L the type list of axes labels.
     * @tparam S the integer type used to represent positions in axes. Default value
     *           is \c std::size_t.
     * @tparam MT the tag used for choosing the map type which holds the label-
     *            position pairs in the axes. Possible values are \c map_tag and
     *            \c hash_map_tag. Default value is \c hash_map_tag.
     * @sa xaxis_view
     */
    template <class K, class L = XFRAME_DEFAULT_LABEL_LIST, class S = std::size_t, class MT = hash_map_tag>
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

    /**
     * Constructs an xcoordinate_view with the given mapping of dumension names
     * to views on axes. The mapping is copied.
     * @param axes the dimension names to views on axes mapping.
     */
    template <class K, class L, class S, class MT>
    inline xcoordinate_view<K, L, S, MT>::xcoordinate_view(const map_type& axes)
        : base_type(axes)
    {
    }

    /**
     * Constructs an xcoordinate_view with the given mapping of dumension names
     * to views on axes. This mapping is moved and therefore it is invalid after
     * the xcoordinate_view has been constructed.
     * @param axes the dimension names to views on axes mapping.
     */
    template <class K, class L, class S, class MT>
    inline xcoordinate_view<K, L, S, MT>::xcoordinate_view(map_type&& axes)
        : base_type(std::move(axes))
    {
    }

    /**
     * Builds and returns an xcoordinate_view from the specified mapping of
     * dimension names to views on axes. The map is copied.
     * @param axes the dimension names to views on axes mapping.
     */
    template <class K, class L, class S, class MT>
    inline xcoordinate_view<K, L, S, MT> coordinate_view(const std::map<K, xaxis_view<L, S, MT>>& axes)
    {
        return xcoordinate_view<K, L, S, MT>(axes);
    }

    /**
     * Builds and returns an xcoordinate_viewfrom the specified mapping of
     * dimension names to views on axes. The map is moved, therefore it is
     * invalid after the xcoordinate object has been built.
     * @param axes the dimension names to views on axes mapping.
     */
    template <class K, class L, class S, class MT>
    inline xcoordinate_view<K, L, S, MT> coordinate_view(std::map<K, xaxis_view<L, S, MT>>&& axes)
    {
        return xcoordinate_view<K, L, S, MT>(std::move(axes));
    }
}

#endif
