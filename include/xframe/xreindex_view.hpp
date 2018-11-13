/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XREINDEX_VIEW_HPP
#define XREINDEX_VIEW_HPP

#include "xtensor/xexpression.hpp"

#include "xcoordinate_chain.hpp"
#include "xvariable.hpp"

namespace xf
{
    
    /*****************
     * xreindex_view *
     *****************/

    template <class CT>
    class xreindex_view;

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

        using subcoordinate_type = typename xexpression_type::coordinate_type;
        using coordinate_type = xcoordinate_chain<subcoordinate_type>;
        using dimension_type = typename xexpression_type::dimension_type;
        using coordinate_system = xcoordinate_system_impl<coordinate_type, const dimension_type&>;
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

        template <class E>
        xreindex_view(E&& e, const coordinate_map& new_coord);
        
        template <class E>
        xreindex_view(E&& e, coordinate_map&& new_coord);

        size_type size() const noexcept;
        size_type dimension() const noexcept;
        const dimension_list& dimension_labels() const noexcept;
        const coordinate_type& coordinates() const noexcept;
        const dimension_type& dimension_mapping() const noexcept;

        template <class Join = DEFAULT_JOIN, class C = coordinate_type>
        xtrivial_broadcast broadcast_coordinates(C& coords) const;
        bool broadcast_dimensions(dimension_type& dims, bool trivial_bc = false) const;

        const shape_type& shape() const noexcept;

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

        template <class Join = DEFAULT_JOIN, std::size_t N = std::numeric_limits<size_type>::max()>
        const_reference select(const selector_sequence_type<N>& selector) const;

        template <class Join = DEFAULT_JOIN, std::size_t N = dynamic()>
        const_reference select(selector_sequence_type<N>&& selector) const;

        template <std::size_t N = dynamic()>
        const_reference iselect(const iselector_sequence_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        const_reference iselect(iselector_sequence_type<N>&& selector) const;

    private:

        void init_shape();

        CT m_e;
        coordinate_system m_coordinate_system;
        shape_type m_shape;
    };

    template <class CT>
    std::ostream& operator<<(std::ostream& out, const xreindex_view<CT>& view);

    /*************************
     * reindex_view builders *
     *************************/

    template <class E, class M>
    auto reindex(E&& e, const M& new_coord);

    template <class E, class M>
    auto reindex(E&& e, M&& new_coord);

    template <class E1, class E2>
    auto reindex_like(E1&& e1, const E2& e2);

    /********************************
     * xreindex_view implmeentation *
     ********************************/

    template <class CT>
    template <class E>
    inline xreindex_view<CT>::xreindex_view(E&& e, const coordinate_map& new_coord)
        : m_e(std::forward<E>(e)),
          m_coordinate_system(reindex(m_e.cordinates(), new_coord), m_e.dimension_mapping())
    {
        init_shape();
    }

    template <class CT>
    template <class E>
    inline xreindex_view<CT>::xreindex_view(E&& e, coordinate_map&& new_coord)
        : m_e(std::forward<E>(e)),
          m_coordinate_system(reindex(m_e.coordinates(), std::move(new_coord)), m_e.dimension_mapping())
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
        return m_coordinate_system.size();
    }

    template <class CT>
    inline auto xreindex_view<CT>::dimension() const noexcept -> size_type
    {
        return m_coordinate_system.dimension();
    }

    template <class CT>
    inline auto xreindex_view<CT>::dimension_labels() const noexcept -> const dimension_list&
    {
        return m_coordinate_system.dimension_labels();
    }

    template <class CT>
    inline auto xreindex_view<CT>::coordinates() const noexcept -> const coordinate_type&
    {
        return m_coordinate_system.coordinates();
    }

    template <class CT>
    inline auto xreindex_view<CT>::dimension_mapping() const noexcept -> const dimension_type&
    {
        return m_coordinate_system.dimension_mapping();
    }

    template <class CT>
    template <class Join, class C>
    inline xtrivial_broadcast xreindex_view<CT>::broadcast_coordinates(C& coords) const
    {
        return m_coordinate_system.broadcast_coordinates(coords);
    }

    template <class CT>
    inline bool xreindex_view<CT>::broadcast_dimensions(dimension_type& dims, bool trivial_bc) const
    {
        return m_coordinate_system.broadcast_dimensions(dims, trivial_bc);
    }

    template <class CT>
    inline auto xreindex_view<CT>::shape() const noexcept -> const shape_type&
    {
        return m_shape;
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

    /****************************************
     * reindex_view builders implementation *
     ****************************************/

    template <class E, class M>
    auto reindex(E&& e, const M& new_coord)
    {
        using view_type = xreindex_view<xtl::closure_type_t<E>>;
        return view_type(std::forward<E>(e), new_coord);
    }

    template <class E, class M>
    auto reindex(E&& e, M&& new_coord)
    {
        using view_type = xreindex_view<xtl::closure_type_t<E>>;
        return view_type(std::forward<E>(e), std::move(new_coord));
    }

    template <class E1, class E2>
    auto reindex_like(E1&& e1, const E2& e2)
    {
        using view_type = xreindex_view<xtl::closure_type_t<E1>>;
        using coordinate_map = typename view_type::coordinate_map;

        coordinate_map new_coord;
        for(auto iter = e1.coordinates().begin(); iter != e1.coordinates().end(); ++iter)
        {
            const auto& axis = e2.coordinates()[iter->first];
            if(axis != iter->second)
            {
                new_coord.insert(std::make_pair(iter->first, axis));
            }
        }
        return view_type(std::forward<E1>(e1), std::move(new_coord));
    }
}

#endif
