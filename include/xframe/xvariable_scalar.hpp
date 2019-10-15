/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVARIABLE_SCALAR_HPP
#define XFRAME_XVARIABLE_SCALAR_HPP

#include "xtensor/xscalar.hpp"
#include "xcoordinate.hpp"

namespace xf
{
    template <class CT>
    class xvariable_scalar
    {
    public:

        using data_type = xt::xscalar<CT>;
        using value_type = typename data_type::value_type;
        using reference = typename data_type::reference;
        using const_reference = typename data_type::const_reference;
        using pointer = typename data_type::pointer;
        using const_pointer = typename data_type::const_pointer;
        using size_type = typename data_type::size_type;
        using difference_type = typename data_type::difference_type;

        using shape_type = std::array<size_type, 0>;

        using coordinate_type = xfull_coordinate;
        using dimension_type = xfull_coordinate;

        using expression_tag = xvariable_expression_tag;

        xvariable_scalar(const xt::xscalar<CT>& v) noexcept;

        size_type size() const noexcept;
        size_type dimension() const noexcept;
        coordinate_type coordinates() const noexcept;
        dimension_type dimension_mapping() const noexcept;
        const shape_type& shape() const noexcept;

        template <class... Args>
        const_reference operator()(Args... args) const noexcept;

        template <class Join = XFRAME_DEFAULT_JOIN, class C>
        xtrivial_broadcast broadcast_coordinates(C& coords) const noexcept;

        template <class D>
        bool broadcast_dimensions(D& dims, bool trivial_bc = false) const noexcept;

        data_type data() const noexcept;

        template <class Join = XFRAME_DEFAULT_JOIN, std::size_t N = dynamic(), class S = std::size_t>
        const_reference select(const S&) const noexcept;

    private:

        data_type m_data;
    };

    template <class T>
    struct is_xvariable_scalar : std::false_type
    {
    };

    template <class CT>
    struct is_xvariable_scalar<xvariable_scalar<CT>> : std::true_type
    {
    };

    /***********************************
     * xvariable_scalar implementation *
     ***********************************/

    template <class CT>
    xvariable_scalar<CT>::xvariable_scalar(const xt::xscalar<CT>& v) noexcept
        : m_data(v)
    {
    }

    template <class CT>
    inline auto xvariable_scalar<CT>::size() const noexcept -> size_type
    {
        return data().size();
    }

    template <class CT>
    inline auto xvariable_scalar<CT>::dimension() const noexcept -> size_type
    {
        return data().dimension();
    }

    template <class CT>
    inline auto xvariable_scalar<CT>::coordinates() const noexcept -> coordinate_type
    {
        return xfull_coordinate();
    }

    template <class CT>
    inline auto xvariable_scalar<CT>::dimension_mapping() const noexcept -> dimension_type
    {
        return xfull_coordinate();
    }

    template <class CT>
    inline auto xvariable_scalar<CT>::shape() const noexcept -> const shape_type&
    {
        static shape_type zero_shape;
        return zero_shape;
    }

    template <class CT>
    template <class... Args>
    inline auto xvariable_scalar<CT>::operator()(Args... args) const noexcept -> const_reference
    {
        return data()(args...);
    }

    template <class CT>
    template <class Join, class C>
    inline xtrivial_broadcast xvariable_scalar<CT>::broadcast_coordinates(C& /*coords*/) const noexcept
    {
        return xtrivial_broadcast(true, true);
    }

    template <class CT>
    template <class D>
    bool xvariable_scalar<CT>::broadcast_dimensions(D& /*dims*/, bool /*trivial_bc*/) const noexcept
    {
        return true;
    }

    template <class CT>
    inline auto xvariable_scalar<CT>::data() const noexcept -> data_type
    {
        return m_data;
    }

    template <class CT>
    template <class Join, std::size_t N, class S>
    inline auto xvariable_scalar<CT>::select(const S&) const noexcept -> const_reference
    {
        // TODO: should return data()() but this requires update of xtensor/xoptional.hpp
        // (split_optional_expression_impl needs to catch ref and const ref on xscalar)
        return m_data();
    }
}

#endif
