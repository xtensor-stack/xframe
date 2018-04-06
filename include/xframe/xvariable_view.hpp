/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVARIABLE_VIEW_HPP
#define XFRAME_XVARIABLE_VIEW_HPP

#include "xvariable.hpp"
#include "xcoordinate_view.hpp"

namespace xf
{

    /*******************
     * xvariable_view  *
     *******************/

    template <class CT>
    class xvariable_view
    {
    public:

        using self_type = xvariable_view<CT>;
        using xexpression_type = std::decay_t<CT>;

        static constexpr bool is_const = std::is_const<std::remove_reference_t<CT>>::value;
        using value_type = typename xexpression_type::value_type;
        using reference = std::conditional_t<is_const,
                                             typename xexpression_type::const_reference,
                                             typename xexpression_type::reference>;
        using const_reference = typename xexpression_type::const_reference;
        using pointer = std::conditional_t<is_const,
                                           typename xexpression_type::const_pointer,
                                           typename xexpression_type::pointer>;
        using const_pointer = typename xexpression_type::const_pointer;
        using size_type = typename xexpression_type::size_type;
        using difference_type = typename xexpression_type::difference_type;

        using coordinate_type = xcoordinate_view_type_t<typename xexpression_type::coordinate_type>;
        using dimension_type = typename xexpression_type::dimension_type;
        using dimension_list = typename dimension_type::label_list;

        template <class E>
        xvariable_view(E&& e, coordinate_type&& coord, dimension_type&& dim);

        size_type size() const noexcept;
        size_type dimension() const noexcept;
        const dimension_list& dimension_labels() const noexcept;
        const coordinate_type& coordinates() const noexcept;
        const dimension_type& dimension_mapping() const noexcept;

        xexpression_type& data() noexcept;
        const xexpression_type& data() const noexcept;

        template <std::size_t N = dynamic()>
        using selector_traits = xselector_traits<coordinate_type, dimension_type, N>;
        template <std::size_t N = dynamic()>
        using selector_type = typename selector_traits<N>::selector_type;
        template <std::size_t N = dynamic()>
        using selector_map_type = typename selector_traits<N>::selector_map_type;

        template <std::size_t N = dynamic()>
        reference select(const selector_map_type<N>& selector);

        template <std::size_t N = dynamic()>
        reference select(selector_map_type<N>&& selector);

        bool operator==(const self_type& rhs) const noexcept;
        bool operator!=(const self_type& rhs) const noexcept;

    private:

        template <class S>
        reference select_impl(const S& selector);

        CT m_e;
        coordinate_type m_coordinate;
        dimension_type m_dimension;
    };

    /***************************
     * xvariable_view builders *
     ***************************/

    template <class E, class L = DEFAULT_LABEL_LIST>
    auto select(E&& e, std::map<typename std::decay_t<E>::key_type, xaxis_slice<L>>&& slices);

    /*********************************
     * xvariable_view implementation *
     *********************************/

    template <class CT>
    template <class E>
    inline xvariable_view<CT>::xvariable_view(E&& e, coordinate_type&& coord, dimension_type&& dim)
        : m_e(std::forward<E>(e)), m_coordinate(std::move(coord)), m_dimension(std::move(dim))
    {
    }

    template <class CT>
    inline auto xvariable_view<CT>::size() const noexcept -> size_type
    {
        return std::accumulate(m_coordinate.cbegin(), m_coordinate.cend(), size_type(1),
            [](size_type val, auto&& entry) {
                return val * entry.second.size();
            }
        );
    }

    template <class CT>
    inline auto xvariable_view<CT>::dimension() const noexcept -> size_type
    {
        return m_dimension.size();
    }

    template <class CT>
    inline auto xvariable_view<CT>::dimension_labels() const noexcept -> const dimension_list&
    {
        return m_dimension.labels();
    }

    template <class CT>
    inline auto xvariable_view<CT>::coordinates() const noexcept -> const coordinate_type&
    {
        return m_coordinate;
    }

    template <class CT>
    inline auto xvariable_view<CT>::dimension_mapping() const noexcept -> const dimension_type&
    {
        return m_dimension;
    }

    template <class CT>
    inline auto xvariable_view<CT>::data() noexcept -> xexpression_type&
    {
        return m_e;
    }

    template <class CT>
    inline auto xvariable_view<CT>::data() const noexcept -> const xexpression_type&
    {
        return m_e;
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::select(const selector_map_type<N>& selector) -> reference
    {
        return select(selector_map_type<N>(selector));
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::select(selector_map_type<N>&& selector) -> reference
    {
        return select_impl(selector_type<N>(std::move(selector)));
    }

    template <class CT>
    inline bool xvariable_view<CT>::operator==(const self_type& rhs) const noexcept
    {
        return m_e == rhs.m_e && m_coordinate == rhs.m_coordinate && m_dimension == rhs.m_dimension;
    }

    template <class CT>
    inline bool xvariable_view<CT>::operator!=(const self_type& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    template <class CT>
    template <class S>
    inline auto xvariable_view<CT>::select_impl(const S& selector) -> reference
    {
        typename S::index_type idx = selector.get_index(coordinates(), dimension_mapping());
        return data().element(idx.cbegin(), idx.cend());
    }

    /******************************************
     * xvariable_view builders implementation *
     ******************************************/

    namespace detail
    {
        template <class M, class C>
        inline void fill_coordinate_map_impl(const C& coord, M& cmap)
        {
        }

        template <class M, class C, class L0, class SL0, class... L, class... SL>
        inline void fill_coordinate_map_impl(const C& coord, M& cmap, std::pair<L0, SL0> sl0, std::pair<L, SL>... sl)
        {
            using axis_view_type = typename M::axis_type;
            const auto& ax = coord[sl0.first];
            cmap.emplace(std::make_pair(sl0.first, axis_view_type(ax, sl0.build_islice(ax))));
            fill_coordinate_map_impl(coord, cmap, std::move(sl)...);
        }
    }
    
    template <class E, class L>
    auto select(E&& e, std::map<typename std::decay_t<E>::key_type, xaxis_slice<L>>&& slices)
    {
        using coordinate_type = typename std::decay_t<E>::coordinate_type;
        using dimension_type = typename std::decay_t<E>::dimension_type;
        using view_type = xvariable_view<xtl::closure_type_t<E>>;
        using coordinate_view_type = typename view_type::coordinate_type;
        using map_type = typename coordinate_view_type::map_type;
        using axis_type = typename coordinate_view_type::axis_type;

        const coordinate_type& coords = e.coordinates();
        map_type view_map;
        for_each(coords.cbegin(), coords.cend(), [&view_map, &slices](auto&& arg) {
            auto iter = slices.find(arg.first);
            if (iter != slices.end())
            {
                const auto& ax = arg.second;
                view_map.emplace(arg.first, axis_type(ax, (iter->second).build_islice(arg.second)));
            }
            else
            {
                using size_type = typename std::decay_t<E>::size_type;
                const auto& ax = arg.second;
                view_map.emplace(arg.first, axis_type(ax, xaxis_iall<size_type>(arg.second.size())));
            }
        });
        coordinate_view_type coordinate_view(std::move(view_map));
        dimension_type view_dimension(e.dimension_mapping());

        return view_type(std::forward<E>(e), std::move(coordinate_view), std::move(view_dimension));
    }
}

#endif