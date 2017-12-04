/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVARIABLE_HPP
#define XFRAME_XVARIABLE_HPP

#include "xtensor/xoptional_assembly.hpp"

#include "xvariable_base.hpp"
#include "xvariable_assign.hpp"
#include "xvariable_math.hpp"

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
        using coordinate_type = xcoordinate<K, size_type, hash_map_tag, L>;
        using dimension_type = xdimension<K, size_type>;
    };
}

namespace xt
{
    template <class K, class VE, class FE, class L>
    struct xcontainer_inner_types<xf::xvariable<K, VE, FE, L>>
        : xf::xvariable_inner_types<xf::xvariable<K, VE, FE, L>>
    {
        using temporary_type = xf::xvariable<K, VE, FE, L>;
    };
}

namespace xf
{
    template <class K, class VE, class FE, class L>
    class xvariable : public xvariable_base<xvariable<K, VE, FE, L>>,
                      public xt::xcontainer_semantic<xvariable<K, VE, FE, L>>
    {
    public:

        using self_type = xvariable<K, VE, FE, L>;
        using base_type = xvariable_base<self_type>;
        using semantic_base = xt::xcontainer_semantic<self_type>;

        using data_type = typename base_type::data_type;
        using coordinate_map = typename base_type::coordinate_map;
        using coordinate_initializer = typename base_type::coordinate_initializer;
        using temporary_type = typename semantic_base::temporary_type;

        using expression_tag = xvariable_expression_tag;

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

        template <class E>
        xvariable(const xt::xexpression<E>& e);

        template <class E>
        xvariable& operator=(const xt::xexpression<E>& e);

    private:

        data_type m_data;

        data_type& data_impl() noexcept;
        const data_type& data_impl() const noexcept;

        friend class xvariable_base<xvariable<K, VE, FE, L>>;
    };

    // Temporary: xtensor operator== and operator!= should be enabled for xtensor_expression_tag
    // and xoptional_expression_tag only
    template <class K, class VE, class FE, class L>
    inline bool operator==(const xvariable<K, VE, FE, L>& lhs, const xvariable<K, VE, FE, L>& rhs)
    {
        using base_type = typename xvariable<K, VE, FE, L>::base_type;
        return operator==((const base_type&)lhs, (const base_type&)rhs);
    }

    template <class K, class VE, class FE, class L>
    inline bool operator!=(const xvariable<K, VE, FE, L>& lhs, const xvariable<K, VE, FE, L>& rhs)
    {
        return !(lhs == rhs);
    }

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
    template <class E>
    inline xvariable<K, VE, FE, L>::xvariable(const xt::xexpression<E>& e)
        : base_type()
    {
        semantic_base::assign(e);
    }

    template <class K, class VE, class FE, class L>
    template <class E>
    inline auto xvariable<K, VE, FE, L>::operator=(const xt::xexpression<E>& e) -> self_type&
    {
        return semantic_base::assign(e);
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
