/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XREINDEX_VIEW_HPP
#define XREINDEX_VIEW_HPP

#include "xtensor/xexpression.hpp"

#include "xcoordinate_chain.hpp"
#include "xreindex_data.hpp"
#include "xvariable.hpp"

namespace xf
{

    /*****************
     * xreindex_view *
     *****************/

    template <class CT>
    class xreindex_view : public xt::xexpression<xreindex_view<CT>>
    {
    public:

        using self_type = xreindex_view<CT>;
        using xexpression_type = std::decay_t<CT>;
        using value_type = typename xexpression_type::value_type;
        using reference = typename xexpression_type::const_reference;
        using const_reference = reference;
        using pointer = typename xexpression_type::const_pointer;
        using const_pointer = pointer;
        using size_type = typename xexpression_type::size_type;
        using difference_type = typename xexpression_type::difference_type;

        using shape_type = typename xexpression_type::shape_type;
        using data_type = xreindex_data<self_type>;

        using subcoordinate_type = typename xexpression_type::coordinate_type;
        using coordinate_type = xcoordinate_chain<subcoordinate_type>;
        using dimension_type = typename xexpression_type::dimension_type;
        using dimension_list = typename dimension_type::label_list;
        using coordinate_map = typename coordinate_type::map_type;

        using expression_tag = xvariable_expression_tag;

        template <std::size_t N = dynamic()>
        using selector_traits = xselector_traits<coordinate_type, dimension_type, N>;
        template <std::size_t N = dynamic()>
        using index_type = typename selector_traits<N>::index_type;
        template <std::size_t N = dynamic()>
        using selector_type = typename selector_traits<N>::selector_type;
        template <std::size_t N = dynamic()>
        using selector_sequence_type = typename selector_traits<N>::selector_sequence_type;
        template <std::size_t N = dynamic()>
        using iselector_type = typename selector_traits<N>::iselector_type;
        template <std::size_t N = dynamic()>
        using iselector_sequence_type = typename selector_traits<N>::iselector_sequence_type;
        template <std::size_t N = dynamic()>
        using locator_type = typename selector_traits<N>::locator_type;
        template <std::size_t N = dynamic()>
        using locator_sequence_type = typename selector_traits<N>::locator_sequence_type;

        static const_reference missing();

        xreindex_view(self_type&& rhs);
        xreindex_view(const self_type& rhs);

        template <class E>
        xreindex_view(E&& e, const coordinate_map& new_coord);

        template <class E>
        xreindex_view(E&& e, coordinate_map&& new_coord);

        size_type size() const noexcept;
        size_type dimension() const noexcept;
        const dimension_list& dimension_labels() const noexcept;
        const coordinate_type& coordinates() const noexcept;
        const dimension_type& dimension_mapping() const noexcept;

        template <class Join = XFRAME_DEFAULT_JOIN, class C = coordinate_type>
        xtrivial_broadcast broadcast_coordinates(C& coords) const;
        bool broadcast_dimensions(dimension_type& dims, bool trivial_bc = false) const;

        const shape_type& shape() const noexcept;
        const data_type& data() const noexcept;

        template <class... Args>
        const_reference operator()(Args... args) const;

        template <std::size_t N = dynamic()>
        const_reference element(const index_type<N>& index) const;

        template <std::size_t N = dynamic()>
        const_reference element(index_type<N>&& index) const;

        template <class... Args>
        const_reference locate(Args&&... args) const;

        template <std::size_t N = dynamic()>
        const_reference locate_element(const locator_sequence_type<N>& locator) const;

        template <std::size_t N = dynamic()>
        const_reference locate_element(locator_sequence_type<N>&& locator) const;

        template <class Join = XFRAME_DEFAULT_JOIN, std::size_t N = std::numeric_limits<size_type>::max()>
        const_reference select(const selector_sequence_type<N>& selector) const;

        template <class Join = XFRAME_DEFAULT_JOIN, std::size_t N = dynamic()>
        const_reference select(selector_sequence_type<N>&& selector) const;

        template <std::size_t N = dynamic()>
        const_reference iselect(const iselector_sequence_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        const_reference iselect(iselector_sequence_type<N>&& selector) const;

    private:

        void init_shape();

        template <std::size_t N, class IDX>
        const_reference element_impl(IDX&& index) const;

        template <std::size_t N, class L>
        const_reference locate_element_impl(L&& locator) const;

        template <class S>
        const_reference select_impl(S&& selector) const;

        template <class Join, class S>
        const_reference select_join(S&& selector) const;

        template <std::size_t N, class S>
        const_reference iselect_impl(S&& selector) const;

        template <std::size_t N, class S>
        std::pair<index_type<N>, bool> build_iselect_index(S&& selector) const;

        CT m_e;
        coordinate_type m_coordinate;
        const dimension_type& m_dimension_mapping;
        shape_type m_shape;
        data_type m_data;
    };

    template <class CT>
    std::ostream& operator<<(std::ostream& out, const xreindex_view<CT>& view);

    /*************************
     * reindex_view builders *
     *************************/

    template <class E>
    auto reindex(E&& e, const typename std::decay_t<E>::coordinate_map& new_coord);

    template <class E>
    auto reindex(E&& e, typename std::decay_t<E>::coordinate_map&& new_coord);

    template <class E1, class E2>
    auto reindex_like(E1&& e1, const E2& e2);

    template <class Join, class E1, class... E>
    auto align(E1&& e1, E&&... e);

    /********************************
     * xreindex_view implementation *
     ********************************/

    template <class CT>
    inline xreindex_view<CT>::xreindex_view(self_type&& rhs)
        : m_e(std::forward<decltype(rhs.m_e)>(rhs.m_e)),
          m_coordinate(std::move(rhs.m_coordinate)),
          m_dimension_mapping(m_e.dimension_mapping()),
          m_data(*this)
    {
        init_shape();
    }

    template <class CT>
    inline xreindex_view<CT>::xreindex_view(const self_type& rhs)
        : m_e(rhs.m_e),
          m_coordinate(rhs.m_coordinate),
          m_dimension_mapping(m_e.dimension_mapping()),
          m_data(*this)
    {
        init_shape();
    }

    template <class CT>
    template <class E>
    inline xreindex_view<CT>::xreindex_view(E&& e, const coordinate_map& new_coord)
        : m_e(std::forward<E>(e)),
          m_coordinate(reindex(m_e.coordinates(), new_coord)),
          m_dimension_mapping(m_e.dimension_mapping()),
          m_data(*this)
    {
        init_shape();
    }

    template <class CT>
    template <class E>
    inline xreindex_view<CT>::xreindex_view(E&& e, coordinate_map&& new_coord)
        : m_e(std::forward<E>(e)),
          m_coordinate(reindex(m_e.coordinates(), std::move(new_coord))),
          m_dimension_mapping(m_e.dimension_mapping()),
          m_data(*this)
    {
        init_shape();
    }

    template <class CT>
    inline auto xreindex_view<CT>::missing() -> const_reference
    {
        return detail::static_missing<const_reference>();
    }

    template <class CT>
    inline auto xreindex_view<CT>::size() const noexcept -> size_type
    {
        return std::accumulate(m_coordinate.cbegin(), m_coordinate.cend(), size_type(1),
            [](size_type val, auto&& entry) { return val * entry.second.size(); });
    }

    template <class CT>
    inline auto xreindex_view<CT>::dimension() const noexcept -> size_type
    {
        return m_dimension_mapping.size();
    }

    template <class CT>
    inline auto xreindex_view<CT>::dimension_labels() const noexcept -> const dimension_list&
    {
        return m_dimension_mapping.labels();
    }

    template <class CT>
    inline auto xreindex_view<CT>::coordinates() const noexcept -> const coordinate_type&
    {
        return m_coordinate;
    }

    template <class CT>
    inline auto xreindex_view<CT>::dimension_mapping() const noexcept -> const dimension_type&
    {
        return m_dimension_mapping;
    }

    template <class CT>
    template <class Join, class C>
    inline xtrivial_broadcast xreindex_view<CT>::broadcast_coordinates(C& coords) const
    {
        return xf::broadcast_coordinates<Join>(coords, this->coordinates());
    }

    template <class CT>
    inline bool xreindex_view<CT>::broadcast_dimensions(dimension_type& dims, bool trivial_bc) const
    {
        bool ret = true;
        if (trivial_bc)
        {
            dims = this->dimension_mapping();
        }
        else
        {
            ret = xf::broadcast_dimensions(dims, this->dimension_mapping());
        }
        return ret;
    }

    template <class CT>
    inline auto xreindex_view<CT>::shape() const noexcept -> const shape_type&
    {
        return m_shape;
    }

    template <class CT>
    inline auto xreindex_view<CT>::data() const noexcept -> const data_type&
    {
        return m_data;
    }

    template <class CT>
    template <class... Args>
    inline auto xreindex_view<CT>::operator()(Args... args) const -> const_reference
    {
        constexpr std::size_t N = sizeof...(Args);
        using index_value_type = typename index_type<N>::value_type;
        return element({static_cast<index_value_type>(args)...});
    }

    template <class CT>
    template <std::size_t N>
    inline auto xreindex_view<CT>::element(const index_type<N>& index) const -> const_reference
    {
        return element_impl<N>(index);
    }

    template <class CT>
    template <std::size_t N>
    inline auto xreindex_view<CT>::element(index_type<N>&& index) const -> const_reference
    {
        return element_impl<N>(std::move(index));
    }

    template <class CT>
    template <class... Args>
    inline auto xreindex_view<CT>::locate(Args&&... args) const -> const_reference
    {
        constexpr std::size_t N = sizeof...(Args);
        using loc_value_type = typename locator_sequence_type<N>::value_type;
        return locate_element_impl<N>(locator_sequence_type<N>{loc_value_type(args)...});
    }

    template <class CT>
    template <std::size_t N>
    inline auto xreindex_view<CT>::locate_element(const locator_sequence_type<N>& locator) const -> const_reference
    {
        return locate_element_impl<N>(locator);
    }

    template <class CT>
    template <std::size_t N>
    inline auto xreindex_view<CT>::locate_element(locator_sequence_type<N>&& locator) const -> const_reference
    {
        return locate_element_impl<N>(std::move(locator));
    }

    template <class CT>
    template <class Join, std::size_t N>
    inline auto xreindex_view<CT>::select(const selector_sequence_type<N>& selector) const -> const_reference
    {
        return select_join<Join>(selector);
    }

    template <class CT>
    template <class Join, std::size_t N>
    inline auto xreindex_view<CT>::select(selector_sequence_type<N>&& selector) const -> const_reference
    {
        return select_join<Join>(std::move(selector));
    }

    template <class CT>
    template <std::size_t N>
    inline auto xreindex_view<CT>::iselect(const iselector_sequence_type<N>& selector) const -> const_reference
    {
        return iselect_impl<N>(selector);
    }

    template <class CT>
    template <std::size_t N>
    inline auto xreindex_view<CT>::iselect(iselector_sequence_type<N>&& selector) const -> const_reference
    {
        return iselect_impl<N>(std::move(selector));
    }

    template <class CT>
    inline void xreindex_view<CT>::init_shape()
    {
        size_type dim = dimension();
        m_shape.resize(dim);
        for(size_type i = 0; i < dim; ++i)
        {
            m_shape[i] = coordinates().find(dimension_labels()[i])->second.size();
        }
    }

    template <class CT>
    template <std::size_t N, class IDX>
    inline auto xreindex_view<CT>::element_impl(IDX&& index) const -> const_reference
    {
        bool contained = true;
        for(std::size_t i = 0; i < index.size(); ++i)
        {
            auto dim_name = m_dimension_mapping.label(i);
            auto iter = m_coordinate.reindex_map().find(dim_name);
            if(iter != m_coordinate.reindex_map().end())
            {
                auto label = (iter->second).label(index[i]);
                auto subiter = m_coordinate.initial_coordinates().find(dim_name);
                auto subindex = (subiter->second).find(label);
                if(subindex == (subiter->second).end())
                {
                    contained = false;
                    break;
                }
                else
                {
                    index[i] = subindex->second;
                }
            }
        }
        return contained ? m_e.template element<N>(std::forward<IDX>(index)) : missing();
    }

    template <class CT>
    template <std::size_t N, class L>
    inline auto xreindex_view<CT>::locate_element_impl(L&& locator) const -> const_reference
    {
        for(std::size_t i = 0; i < locator.size(); ++i)
        {
            auto dim_name = m_dimension_mapping.label(i);
            bool contained = m_coordinate.is_reindexed(dim_name, locator[i]);
            bool sub_contained = m_coordinate.initial_coordinates().contains(dim_name, locator[i]);
            if(contained && !sub_contained)
            {
                return missing();
            }
        }
        return m_e.template locate_element<N>(std::forward<L>(locator));
    }

    template <class CT>
    template <class S>
    inline auto xreindex_view<CT>::select_impl(S&& selector) const -> const_reference
    {
        for(const auto& c: selector)
        {
            bool contained = m_coordinate.is_reindexed(c.first, c.second);
            bool sub_contained = m_e.coordinates().contains(c.first, c.second);
            if(contained && !sub_contained)
            {
                return missing();
            }
        }
        return m_e.select(std::forward<S>(selector));
    }

    template <class CT>
    template <class Join, class S>
    inline auto xreindex_view<CT>::select_join(S&& selector) const -> const_reference
    {
        return xtl::mpl::static_if<Join::id() == join::inner::id()>([&](auto self)
        {
            return self(*this).select_impl(std::forward<S>(selector));
        }, /*else*/ [&](auto self)
        {
            return self(*this).m_e.template select<join::outer>(std::forward<S>(selector));
        });
    }

    template <class CT>
    template <std::size_t N, class S>
    inline auto xreindex_view<CT>::iselect_impl(S&& iselector) const -> const_reference
    {
        auto outer_index = build_iselect_index<N>(std::forward<S>(iselector));
        return outer_index.second ? m_e.element(outer_index.first) : missing();
    }

    template <class CT>
    template <std::size_t N, class S>
    inline auto xreindex_view<CT>::build_iselect_index(S&& selector) const -> std::pair<index_type<N>, bool>
    {
        auto res = std::make_pair(xtl::make_sequence<index_type<N>>(dimension(), size_type(0)), true);
        auto reindex_end = m_coordinate.reindex_map().end();
        for(const auto& c: selector)
        {
            auto iter = m_coordinate.reindex_map().find(c.first);
            if(iter != reindex_end)
            {
                auto label = (iter->second).label(c.second);
                auto subiter = m_coordinate.initial_coordinates().find(c.first);
                auto subindex = (subiter->second).find(label);
                if(subindex == (subiter->second).end())
                {
                    res.second = false;
                    break;
                }
                else
                {
                    res.first[m_dimension_mapping[c.first]] = subindex->second;
                }
            }
            else
            {
                res.first[m_dimension_mapping[c.first]] = c.second;
            }
        }
        return res;
    }

    /****************************************
     * reindex_view builders implementation *
     ****************************************/

    template <class E>
    inline auto reindex(E&& e, const typename std::decay_t<E>::coordinate_map& new_coord)
    {
        using view_type = xreindex_view<xtl::closure_type_t<E>>;
        return view_type(std::forward<E>(e), new_coord);
    }

    template <class E>
    inline auto reindex(E&& e, typename std::decay_t<E>::coordinate_map&& new_coord)
    {
        using view_type = xreindex_view<xtl::closure_type_t<E>>;
        return view_type(std::forward<E>(e), std::move(new_coord));
    }

    namespace detail
    {
        template <class E1, class C>
        inline auto reindex_like_coord(E1&& e1, const C& coords)
        {
            using view_type = xreindex_view<xtl::closure_type_t<E1>>;
            using coordinate_map = typename view_type::coordinate_map;

            coordinate_map new_coord;
            for(auto iter = e1.coordinates().begin(); iter != e1.coordinates().end(); ++iter)
            {
                const auto& axis = coords[iter->first];
                if(axis != iter->second)
                {
                    new_coord.insert(std::make_pair(iter->first, axis));
                }
            }
            return view_type(std::forward<E1>(e1), std::move(new_coord));
        }
    }

    template <class E1, class E2>
    inline auto reindex_like(E1&& e1, const E2& e2)
    {
        return detail::reindex_like_coord(std::forward<E1>(e1), e2.coordinates());
    }

    template <class Join, class E1, class... E>
    inline auto align(E1&& e1, E&&... e)
    {
        auto coord = e1.coordinates();
        broadcast_coordinates<Join>(coord, e.coordinates()...);
        return std::make_tuple(detail::reindex_like_coord(std::forward<E1>(e1), coord),
                               detail::reindex_like_coord(std::forward<E>(e), coord)...);
    }

    template <class CT>
    inline std::ostream& operator<<(std::ostream& out, const xreindex_view<CT>& view)
    {
        return print_variable_expression(out, view);
    }
}

#endif
