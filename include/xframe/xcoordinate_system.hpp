/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XCOORDINATE_SYSTEM_HPP
#define XFRAME_XCOORDINATE_SYSTEM_HPP

#include <algorithm>
#include "xcoordinate.hpp"
#include "xdimension.hpp"

namespace xf
{

    /**********************
     * xcoordinate_system *
     **********************/

    template <class D>
    class xcoordinate_system
    {
    public:

        using derived_type = D;
        using inner_types = xt::xcontainer_inner_types<D>;
        using coordinate_type = typename inner_types::coordinate_type;
        using coordinate_closure_type = typename inner_types::coordinate_closure_type;
        using dimension_type = typename inner_types::dimension_type;
        using dimension_list = typename dimension_type::label_list;
        using size_type = typename coordinate_type::size_type;

        size_type size() const noexcept;
        size_type dimension() const noexcept;
        const dimension_list& dimension_labels() const noexcept;
        const coordinate_type& coordinates() const noexcept;
        const dimension_type& dimension_mapping() const noexcept;

        template <class Join = XFRAME_DEFAULT_JOIN, class C = coordinate_type>
        xtrivial_broadcast broadcast_coordinates(C& coords) const;
        bool broadcast_dimensions(dimension_type& dims, bool trivial_bc = false) const;

    protected:

        xcoordinate_system() = default;
        template <class C, class DM>
        xcoordinate_system(C&& coords, DM&& dims);
        ~xcoordinate_system() = default;

        xcoordinate_system(const xcoordinate_system&) = default;
        xcoordinate_system& operator=(const xcoordinate_system&) = default;

        xcoordinate_system(xcoordinate_system&&) = default;
        xcoordinate_system& operator=(xcoordinate_system&&) = default;

        template <class C, class DM>
        void resize(C&& coords, DM&& dims);

    private:

        coordinate_closure_type m_coordinate;
        dimension_type m_dimension_mapping;
    };

    /*************************************
     * xcoordinate_system implementation *
     *************************************/

    template <class D>
    template <class C, class DM>
    inline xcoordinate_system<D>::xcoordinate_system(C&& coords, DM&& dims)
        : m_coordinate(std::forward<C>(coords)),
          m_dimension_mapping(std::forward<DM>(dims))
    {
    }

    template <class D>
    template <class C, class DM>
    inline void xcoordinate_system<D>::resize(C&& coords, DM&& dims)
    {
        m_coordinate = std::forward<C>(coords);
        m_dimension_mapping = std::forward<DM>(dims);
    }

    template <class D>
    inline auto xcoordinate_system<D>::size() const noexcept -> size_type
    {
        return std::accumulate(m_coordinate.cbegin(), m_coordinate.cend(), size_type(1),
            [](size_type val, auto&& entry) { return val * entry.second.size(); });
    }

    template <class D>
    inline auto xcoordinate_system<D>::dimension() const noexcept -> size_type
    {
        return m_dimension_mapping.size();
    }

    template <class D>
    inline auto xcoordinate_system<D>::dimension_labels() const noexcept -> const dimension_list&
    {
        return m_dimension_mapping.labels();
    }

    template <class D>
    inline auto xcoordinate_system<D>::coordinates() const noexcept -> const coordinate_type&
    {
        return m_coordinate;
    }

    template <class D>
    inline auto xcoordinate_system<D>::dimension_mapping() const noexcept -> const dimension_type&
    {
        return m_dimension_mapping;
    }

    template <class D>
    template <class Join, class C>
    inline xtrivial_broadcast xcoordinate_system<D>::broadcast_coordinates(C& coords) const
    {
        return xf::broadcast_coordinates<Join>(coords, this->coordinates());
    }

    template <class D>
    inline bool xcoordinate_system<D>::broadcast_dimensions(dimension_type& dims, bool trivial_bc) const
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

}

#endif
