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

        using squeeze_map = std::map<typename dimension_type::mapped_type, typename coordinate_type::index_type>;

        template <std::size_t N = dynamic()>
        using selector_traits = xselector_traits<coordinate_type, dimension_type, N>;
        template <std::size_t N = dynamic()>
        using selector_type = typename selector_traits<N>::selector_type;
        template <std::size_t N = dynamic()>
        using selector_map_type = typename selector_traits<N>::selector_map_type;
        template <std::size_t N = dynamic()>
        using iselector_type = typename selector_traits<N>::iselector_type;
        template <std::size_t N = dynamic()>
        using iselector_map_type = typename selector_traits<N>::iselector_map_type;

        template <class E>
        xvariable_view(E&& e, coordinate_type&& coord, dimension_type&& dim, squeeze_map&& squeeze);

        static const_reference missing();

        size_type size() const noexcept;
        size_type dimension() const noexcept;
        const dimension_list& dimension_labels() const noexcept;
        const coordinate_type& coordinates() const noexcept;
        const dimension_type& dimension_mapping() const noexcept;

        xexpression_type& data() noexcept;
        const xexpression_type& data() const noexcept;

        template <class... Args>
        reference operator()(Args... args);

        template <class... Args>
        const_reference operator()(Args... args) const;

        template <class... Args>
        reference locate(Args&&... args);

        template <class... Args>
        const_reference locate(Args&&... args) const;

        template <std::size_t N = dynamic()>
        reference select(const selector_map_type<N>& selector);

        template <class Join = DEFAULT_JOIN, std::size_t N = std::numeric_limits<size_type>::max()>
        const_reference select(const selector_map_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        reference select(selector_map_type<N>&& selector);

        template <class Join = DEFAULT_JOIN, std::size_t N = dynamic()>
        const_reference select(selector_map_type<N>&& selector) const;

        template <std::size_t N = dynamic()>
        reference iselect(const iselector_map_type<N>& selector);

        template <std::size_t N = dynamic()>
        const_reference iselect(const iselector_map_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        reference iselect(iselector_map_type<N>&& selector);

        template <std::size_t N = dynamic()>
        const_reference iselect(iselector_map_type<N>&& selector) const;

        bool operator==(const self_type& rhs) const noexcept;
        bool operator!=(const self_type& rhs) const noexcept;

    private:

        using index_type = std::vector<size_type>;

        template <std::size_t... I, class... Args>
        reference access_impl(std::index_sequence<I...>, Args... args);

        template <std::size_t... I, class... Args>
        const_reference access_impl(std::index_sequence<I...>, Args... args) const;

        template <class... Args>
        index_type build_accessor(Args&&... args) const;

        template <std::size_t I, class T, class... Args>
        void fill_accessor(index_type& accessor, T idx, Args... args) const;

        template <std::size_t I>
        void fill_accessor(index_type& accessor) const;

        template <std::size_t... I, class... Args>
        reference locate_impl(std::index_sequence<I...>, Args&&... args);

        template <std::size_t... I, class... Args>
        const_reference locate_impl(std::index_sequence<I...>, Args&&... args) const;

        template <class... Args>
        index_type build_locator(Args&&... args) const;

        template <std::size_t I, class T, class... Args>
        void fill_locator(index_type& locator, T idx, Args&&... args) const;

        template <std::size_t I>
        void fill_locator(index_type& locator) const;

        template <class S>
        reference select_impl(const S& selector);

        template <class S>
        const_reference select_impl(const S& selector) const;

        template <class S>
        const_reference select_outer(const S& selector) const;

        template <class Join, class S>
        const_reference select_join(const S& selector) const;

        template <class Idx>
        void fill_squeeze(Idx& index) const;

        template <std::size_t N>
        void adapt_iselector(iselector_map_type<N>& selector) const;

        CT m_e;
        coordinate_type m_coordinate;
        dimension_type m_dimension;
        squeeze_map m_squeeze;
    };

    /***************************
     * xvariable_view builders *
     ***************************/

    template <class E, class... S>
    auto view(E&& e, S&&... slices);

    template <class L = DEFAULT_LABEL_LIST, class E, class... S>
    auto locate(E&& e, S&&... slices);

    template <class E, class L = DEFAULT_LABEL_LIST>
    auto select(E&& e, std::map<typename std::decay_t<E>::key_type, xaxis_slice<L>>&& slices);

    template <class E, class T = typename std::decay_t<E>::size_type>
    auto iselect(E&& e, std::map<typename std::decay_t<E>::key_type, xaxis_extended_islice<T>> && slices);

    /*********************************
     * xvariable_view implementation *
     *********************************/

    template <class CT>
    template <class E>
    inline xvariable_view<CT>::xvariable_view(E&& e, coordinate_type&& coord, dimension_type&& dim, squeeze_map&& squeeze)
        : m_e(std::forward<E>(e)),
          m_coordinate(std::move(coord)),
          m_dimension(std::move(dim)),
          m_squeeze(std::move(squeeze))
    {
    }

    template <class CT>
    inline auto xvariable_view<CT>::missing() -> const_reference
    {
        return detail::static_missing<const_reference>();
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
    template <class... Args>
    inline auto xvariable_view<CT>::operator()(Args... args) ->reference
    {
        if (m_squeeze.empty())
        {
            return access_impl(std::make_index_sequence<sizeof...(Args)>(), args...);
        }
        else
        {
            auto idx = build_accessor(std::forward<Args>(args)...);
            return data().element(idx.cbegin(), idx.cend());
        }
    }

    template <class CT>
    template <class... Args>
    inline auto xvariable_view<CT>::operator()(Args... args) const -> const_reference
    {
        if (m_squeeze.empty())
        {
            return access_impl(std::make_index_sequence<sizeof...(Args)>(), args...);
        }
        else
        {
            auto idx = build_accessor(std::forward<Args>(args)...);
            return data().element(idx.cbegin(), idx.cend());
        }
    }

    template <class CT>
    template <class... Args>
    inline auto xvariable_view<CT>::locate(Args&&... args) -> reference
    {
        if (m_squeeze.empty())
        {
            return locate_impl(std::make_index_sequence<sizeof...(Args)>(), std::forward<Args>(args)...);
        }
        else
        {
            auto idx = build_locator(std::forward<Args>(args)...);
            return data().element(idx.cbegin(), idx.cend());
        }
    }

    template <class CT>
    template <class... Args>
    inline auto xvariable_view<CT>::locate(Args&&... args) const -> const_reference
    {
        if (m_squeeze.empty())
        {
            return locate_impl(std::make_index_sequence<sizeof...(Args)>(), std::forward<Args>(args)...);
        }
        else
        {
            auto idx = build_locator(std::forward<Args>(args)...);
            return data().element(idx.cbegin(), idx.cend());
        }
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::select(const selector_map_type<N>& selector) -> reference
    {
        return select(selector_type<N>(selector));
    }

    template <class CT>
    template <class Join, std::size_t N>
    inline auto xvariable_view<CT>::select(const selector_map_type<N>& selector) const -> const_reference
    {
        return select_join<Join>(selector_type<N>(selector));
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::select(selector_map_type<N>&& selector) -> reference
    {
        return select_impl(selector_type<N>(std::move(selector)));
    }

    template <class CT>
    template <class Join, std::size_t N>
    inline auto xvariable_view<CT>::select(selector_map_type<N>&& selector) const -> const_reference
    {
        return select_join<Join>(selector_type<N>(std::move(selector)));
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::iselect(const iselector_map_type<N>& selector) -> reference
    {
        return iselect(iselector_map_type<N>(selector));
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::iselect(const iselector_map_type<N>& selector) const -> const_reference
    {
        return iselect(iselector_map_type<N>(selector));
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::iselect(iselector_map_type<N>&& selector) -> reference
    {
        iselector_map_type<N> tmp_selector(std::move(selector));
        adapt_iselector<N>(tmp_selector);
        return select_impl(iselector_type<N>(std::move(tmp_selector)));
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::iselect(iselector_map_type<N>&& selector) const -> const_reference
    {
        iselector_map_type<N> tmp_selector(std::move(selector));
        adapt_iselector<N>(tmp_selector);
        return select_impl(iselector_type<N>(std::move(tmp_selector)));
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
    template <std::size_t... I, class... Args>
    inline auto xvariable_view<CT>::access_impl(std::index_sequence<I...>, Args... args) -> reference
    {
        return data()(coordinates()[dimension_labels()[I]].index(args)...);
    }

    template <class CT>
    template <std::size_t... I, class... Args>
    inline auto xvariable_view<CT>::access_impl(std::index_sequence<I...>, Args... args) const -> const_reference
    {
        return data()(coordinates()[dimension_labels()[I]].index(args)...);
    }

    template <class CT>
    template <class... Args>
    inline auto xvariable_view<CT>::build_accessor(Args&&... args) const -> index_type
    {
        index_type accessor(data().dimension());
        fill_accessor<0>(accessor, std::forward<Args>(args)...);
        fill_squeeze(accessor);
        return accessor;
    }

    template <class CT>
    template <std::size_t I, class T, class... Args>
    inline void xvariable_view<CT>::fill_accessor(index_type& accessor, T idx, Args... args) const
    {
        std::size_t new_index = data().dimension_mapping()[dimension_labels()[I]];
        accessor[new_index] = coordinates()[dimension_labels()[I]].index(idx);
        fill_accessor<I + 1>(accessor, std::forward<Args>(args)...);
    }

    template <class CT>
    template <std::size_t I>
    inline void xvariable_view<CT>::fill_accessor(index_type& accessor) const
    {

    }

    template <class CT>
    template <std::size_t... I, class... Args>
    inline auto xvariable_view<CT>::locate_impl(std::index_sequence<I...>, Args&&... args) -> reference
    {
        return data()(coordinates()[dimension_mapping().labels()[I]][args]...);
    }

    template <class CT>
    template <std::size_t... I, class... Args>
    inline auto xvariable_view<CT>::locate_impl(std::index_sequence<I...>, Args&&... args) const -> const_reference
    {
        data()(coordinates()[dimension_mapping().labels()[I]][args]...);
    }

    template <class CT>
    template <class... Args>
    inline auto xvariable_view<CT>::build_locator(Args&&... args) const -> index_type
    {
        index_type locator(data().dimension());
        fill_locator<0>(locator, std::forward<Args>(args)...);
        fill_squeeze(locator);
        return locator;
    }

    template <class CT>
    template <std::size_t I, class T, class... Args>
    inline void xvariable_view<CT>::fill_locator(index_type& locator, T idx, Args&&... args) const
    {
        std::size_t new_index = data().dimension_mapping()[dimension_labels()[I]];
        locator[new_index] = coordinates()[dimension_labels()[I]][idx];
        fill_locator<I + 1>(locator, std::forward<Args>(args)...);
    }

    template <class CT>
    template <std::size_t I>
    inline void xvariable_view<CT>::fill_locator(index_type&) const
    {
    }

    template <class CT>
    template <class S>
    inline auto xvariable_view<CT>::select_impl(const S& selector) -> reference
    {
        typename S::index_type idx = selector.get_index(coordinates(), m_e.dimension_mapping());
        fill_squeeze(idx);
        return data().element(idx.cbegin(), idx.cend());
    }

    template <class CT>
    template <class S>
    inline auto xvariable_view<CT>::select_impl(const S& selector) const -> const_reference
    {
        typename S::index_type idx = selector.get_index(coordinates(), m_e.dimension_mapping());
        fill_squeeze(idx);
        return data().element(idx.cbegin(), idx.cend());
    }

    template <class CT>
    template <class S>
    inline auto xvariable_view<CT>::select_outer(const S& selector) const -> const_reference
    {
        typename S::outer_index_type idx = selector.get_outer_index(coordinates(), m_e.dimension_mapping());
        if (idx.second)
        {
            fill_squeeze(idx.first);
            return data().element(idx.first.cbegin(), idx.first.cend());
        }
        else
        {
            return missing();
        }
    }

    template <class CT>
    template <class Join, class S>
    inline auto xvariable_view<CT>::select_join(const S& selector) const -> const_reference
    {
        return xtl::mpl::static_if<Join::id() == join::inner::id()>([&](auto self)
        {
            return self(*this).select_impl(selector);
        }, /*else*/ [&](auto self)
        {
            return self(*this).select_outer(selector);
        });
    }

    template <class CT>
    template <class Idx>
    inline void xvariable_view<CT>::fill_squeeze(Idx& index) const
    {
        for (const auto& sq : m_squeeze)
        {
            index[sq.first] = sq.second;
        }
    }

    template <class CT>
    template <std::size_t N>
    inline void xvariable_view<CT>::adapt_iselector(iselector_map_type<N>& selector) const
    {
        for (auto& sel : selector)
        {
            sel.second = m_coordinate[sel.first].index(sel.second);
        }
    }

    /******************************************
     * xvariable_view builders implementation *
     ******************************************/
    
    namespace detail
    {
        template <class E>
        struct view_params
        {
            using coordinate_type = typename std::decay_t<E>::coordinate_type;
            using dimension_type = typename std::decay_t<E>::dimension_type;
            using dimension_label_list = typename dimension_type::label_list;
            using view_type = xvariable_view<xtl::closure_type_t<E>>;
            using squeeze_map = typename view_type::squeeze_map;
            using coordinate_view_type = typename view_type::coordinate_type;
            using map_type = typename coordinate_view_type::map_type;
            using axis_type = typename coordinate_view_type::axis_type;

            map_type coord_map;
            squeeze_map sq_map;
            dimension_label_list dim_label_list;
        };

        template <std::size_t I, class V, class E>
        inline void fill_locate_view_params(V& /*param*/, E& /*e*/)
        {
        }

        template <std::size_t I, class V, class E, class SL, class... S>
        inline void fill_locate_view_params(V& param, E& e, SL&& sl, S&&... slices)
        {
            using axis_type = typename view_params<E>::axis_type;
            const auto& dim_label = e.dimension_labels()[I];
            const auto& axis = e.coordinates()[dim_label];
            if (auto* sq = sl.get_squeeze())
            {
                param.sq_map[I] = axis[*sq];
            }
            else
            {
                param.coord_map.emplace(dim_label, axis_type(axis, sl.build_islice(axis)));
                param.dim_label_list.push_back(dim_label);
            }
            fill_locate_view_params<I + 1>(param, e, std::forward<S>(slices)...);
        }

        template <std::size_t I, class V, class E>
        inline void fill_view_params(V& /*param*/, E& /*e*/)
        {
        }

        template <std::size_t I, class V, class E, class SL, class... S>
        inline void fill_view_params(V& param, E& e, SL&& sl, S&&... slices)
        {
            using axis_type = typename view_params<E>::axis_type;
            using size_type = typename std::decay_t<E>::size_type;

            const auto& dim_label = e.dimension_labels()[I];
            const auto& axis = e.coordinates()[dim_label];
            if (auto* sq = sl.get_squeeze())
            {
                param.sq_map[I] = *sq;
            }
            else if (auto* sq = sl.get_all())
            {
                param.coord_map.emplace(dim_label, axis_type(axis, xt::xall<size_type>(axis.size())));
                param.dim_label_list.push_back(dim_label);
            }
            else
            {
                param.coord_map.emplace(dim_label, axis_type(axis, *(sl.get_slice())));
                param.dim_label_list.push_back(dim_label);
            }
            fill_view_params<I + 1>(param, e, std::forward<S>(slices)...);
        }
    }

    template <class E, class... S>
    inline auto view(E&& e, S&&... slices)
    {
        using view_param_type = detail::view_params<E>;
        using coordinate_view_type = typename view_param_type::coordinate_view_type;
        using dimension_type = typename view_param_type::dimension_type;
        using view_type = typename view_param_type::view_type;

        view_param_type params;
        detail::fill_view_params<0>(params, e, xaxis_extended_islice<typename std::decay_t<E>::size_type>(std::forward<S>(slices))...);

        coordinate_view_type coordinate_view(std::move(params.coord_map));
        dimension_type view_dimension(std::move(params.dim_label_list));

        return view_type(std::forward<E>(e),
                         std::move(coordinate_view),
                         std::move(view_dimension),
                         std::move(params.sq_map));
    }

    template <class L, class E, class... S>
    inline auto locate(E&& e, S&&... slices)
    {
        using view_param_type = detail::view_params<E>;
        using coordinate_view_type = typename view_param_type::coordinate_view_type;
        using dimension_type = typename view_param_type::dimension_type;
        using view_type = typename view_param_type::view_type;

        view_param_type params;
        detail::fill_locate_view_params<0>(params, e, xaxis_slice<L>(std::forward<S>(slices))...);

        coordinate_view_type coordinate_view(std::move(params.coord_map));
        dimension_type view_dimension(std::move(params.dim_label_list));

        return view_type(std::forward<E>(e),
                         std::move(coordinate_view),
                         std::move(view_dimension),
                         std::move(params.sq_map));
    }

    template <class E, class L>
    inline auto select(E&& e, std::map<typename std::decay_t<E>::key_type, xaxis_slice<L>>&& slices)
    {
        using coordinate_type = typename std::decay_t<E>::coordinate_type;
        using dimension_type = typename std::decay_t<E>::dimension_type;
        using dimension_label_list = typename dimension_type::label_list;
        using view_type = xvariable_view<xtl::closure_type_t<E>>;
        using squeeze_map = typename view_type::squeeze_map;
        using coordinate_view_type = typename view_type::coordinate_type;
        using map_type = typename coordinate_view_type::map_type;
        using axis_type = typename coordinate_view_type::axis_type;
        using size_type = typename std::decay_t<E>::size_type;

        const coordinate_type& underlying_coords = e.coordinates();
        map_type coord_map;
        squeeze_map sq_map;
        dimension_label_list dim_label_list;

        for(const auto& dim_label: e.dimension_labels())
        {
            const auto& axis = underlying_coords[dim_label];
            auto slice_iter = slices.find(dim_label);
            if (slice_iter != slices.end())
            {
                if (auto* sq = (slice_iter->second).get_squeeze())
                {
                    sq_map[e.dimension_mapping()[dim_label]] = axis[*sq];
                }
                else
                {
                    coord_map.emplace(dim_label, axis_type(axis, (slice_iter->second).build_islice(axis)));
                    dim_label_list.push_back(dim_label);
                }
            }
            else
            {
                coord_map.emplace(dim_label, axis_type(axis, xt::xall<size_type>(axis.size())));
                dim_label_list.push_back(dim_label);
            }
        }

        coordinate_view_type coordinate_view(std::move(coord_map));
        dimension_type view_dimension(std::move(dim_label_list));

        return view_type(std::forward<E>(e),
                         std::move(coordinate_view),
                         std::move(view_dimension),
                         std::move(sq_map));
    }

    template <class E, class T>
    inline auto iselect(E&& e, std::map<typename std::decay_t<E>::key_type, xaxis_extended_islice<T>>&& slices)
    {
        using coordinate_type = typename std::decay_t<E>::coordinate_type;
        using dimension_type = typename std::decay_t<E>::dimension_type;
        using dimension_label_list = typename dimension_type::label_list;
        using view_type = xvariable_view<xtl::closure_type_t<E>>;
        using squeeze_map = typename view_type::squeeze_map;
        using coordinate_view_type = typename view_type::coordinate_type;
        using map_type = typename coordinate_view_type::map_type;
        using axis_type = typename coordinate_view_type::axis_type;
        using size_type = typename std::decay_t<E>::size_type;

        const coordinate_type& underlying_coords = e.coordinates();
        map_type coord_map;
        squeeze_map sq_map;
        dimension_label_list dim_label_list;

        for (const auto& dim_label : e.dimension_labels())
        {
            const auto& axis = underlying_coords[dim_label];
            auto slice_iter = slices.find(dim_label);
            if (slice_iter != slices.end())
            {
                if (auto* sq = (slice_iter->second).get_all())
                {
                    coord_map.emplace(dim_label, axis_type(axis, xt::xall<size_type>(axis.size())));
                    dim_label_list.push_back(dim_label);
                }
                else if (auto* sq = (slice_iter->second).get_squeeze())
                {
                    sq_map[e.dimension_mapping()[dim_label]] = *sq;
                }
                else
                {
                    auto* sl = (slice_iter->second).get_slice();
                    coord_map.emplace(dim_label, axis_type(axis, *sl));
                    dim_label_list.push_back(dim_label);
                }
            }
            else
            {
                coord_map.emplace(dim_label, axis_type(axis, xt::xall<size_type>(axis.size())));
                dim_label_list.push_back(dim_label);
            }
        }

        coordinate_view_type coordinate_view(std::move(coord_map));
        dimension_type view_dimension(std::move(dim_label_list));

        return view_type(std::forward<E>(e),
                         std::move(coordinate_view),
                         std::move(view_dimension),
                         std::move(sq_map));
    }
}

#endif