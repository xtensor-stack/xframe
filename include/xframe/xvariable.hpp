/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVARIABLE_HPP
#define XFRAME_XVARIABLE_HPP

#include "xtensor/xoptional_assembly.hpp"

#include "xvariable_base.hpp"

namespace xf
{
    /*************
     * xvariable *
     *************/

    template <class K, class VE, class FE, class L = DEFAULT_LABEL_LIST>
    class xvariable;

    template <class K, class VE, class FE, class L>
    struct xvariable_inner_types<xvariable<K, VE, FE, L>>
    {
        using data_type = xt::xoptional_assembly<VE, FE>;
        using size_type = typename data_type::size_type;
        using coordinate_type = xcoordinate<K, size_type, L>;
        using dimension_type = xaxis<K, size_type>;
    };

    template <class K, class VE, class FE, class L>
    class xvariable : public xvariable_base<xvariable<K, VE, FE, L>>,
                      public xexpression<xvariable<K, VE, FE, L>>
    {
    public:

        using self_type = xvariable<K, VE, FE, L>;
        using base_type = xvariable_base<self_type>;

        using data_type = typename base_type::data_type;
        using coordinate_map = typename base_type::coordinate_map;
        using coordinate_initializer = typename base_type::coordinate_initializer;

        xvariable() = default;

        template <class D, class C, class DM, class = enable_xvariable_t<C, DM>>
        explicit xvariable(D&& data, C&& coords, DM&& dims);

        template <class D, class DM>
        explicit xvariable(D&& data, const coordinate_map& coords, DM&& dims);

        template <class D, class DM>
        explicit xvariable(D&& data, coordinate_map&& coords, DM&& dims);

        template <class D>
        explicit xvariable(D&& data, coordinate_initializer coords);

        xvariable(const xvariable&) = default;
        xvariable& operator=(const xvariable&) = default;

        xvariable(xvariable&&) = default;
        xvariable& operator=(xvariable&&) = default;

    private:

        data_type m_data;

        data_type& data_impl() noexcept;
        const data_type& data_impl() const noexcept;

        friend class xvariable_base<xvariable<K, VE, FE, L>>;
    };

    /****************************
     * xvariable implementation *
     ****************************/

    template <class K, class VE, class FE, class L>
    template <class D, class C, class DM, class>
    inline xvariable<K, VE, FE, L>::xvariable(D&& data, C&& coords, DM&& dims)
        : base_type(std::forward<C>(coords), std::forward<DM>(dims)),
          m_data(std::forward<D>(data))
    {
    }

    template <class K, class VE, class FE, class L>
    template <class D, class DM>
    inline xvariable<K, VE, FE, L>::xvariable(D&& data, const coordinate_map& coords, DM&& dims)
        : base_type(coords, std::forward<DM>(dims)),
          m_data(std::forward<D>(data))
    {
    }

    template <class K, class VE, class FE, class L>
    template <class D, class DM>
    inline xvariable<K, VE, FE, L>::xvariable(D&& data, coordinate_map&& coords, DM&& dims)
        : base_type(std::move(coords), std::forward<DM>(dims)),
          m_data(std::forward<D>(data))
    {
    }

    template <class K, class VE, class FE, class L>
    template <class D>
    inline xvariable<K, VE, FE, L>::xvariable(D&& data, coordinate_initializer coords)
        : base_type(coords),
          m_data(std::forward<D>(data))
    {
    }

    template <class K, class VE, class FE, class L>
    inline auto xvariable<K, VE, FE, L>::data_impl() noexcept -> data_type&
    {
        return m_data;
    }

    template <class K, class VE, class FE, class L>
    inline auto xvariable<K, VE, FE, L>::data_impl() const noexcept -> const data_type&
    {
        return m_data;
    }
}

#endif
