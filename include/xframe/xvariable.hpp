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

#include "xvariable_assign.hpp"
#include "xvariable_base.hpp"
#include "xvariable_math.hpp"

namespace xf
{
    /*************
     * xvariable *
     *************/

    template <class CCT, class ECT>
    class xvariable;

    template <class CCT, class ECT>
    struct xvariable_inner_types<xvariable<CCT, ECT>>
    {
        using data_closure_type = ECT;
        using coordinate_closure_type = CCT;
        using data_type = std::remove_reference_t<ECT>;
        using coordinate_type = std::decay_t<CCT>;
        using key_type = typename coordinate_type::key_type;
        using size_type = typename coordinate_type::size_type;
        using dimension_type = xdimension<key_type, size_type>;
        using dimension_closure_type = dimension_type;
    };
}

namespace xt
{
    template <class CCT, class ECT>
    struct xcontainer_inner_types<xf::xvariable<CCT, ECT>>
        : xf::xvariable_inner_types<xf::xvariable<CCT, ECT>>
    {
        using temporary_type = xf::xvariable<std::decay_t<CCT>, std::decay_t<ECT>>;
    };
}

namespace xf
{
    template <class CCT, class ECT>
    class xvariable : public xvariable_base<xvariable<CCT, ECT>>,
                      public xt::xcontainer_semantic<xvariable<CCT, ECT>>
    {
    public:

        using self_type = xvariable<CCT, ECT>;
        using base_type = xvariable_base<self_type>;
        using semantic_base = xt::xcontainer_semantic<self_type>;

        using data_type = typename base_type::data_type;
        using data_closure_type = typename base_type::data_closure_type;
        using coordinate_map = typename base_type::coordinate_map;
        using coordinate_initializer = typename base_type::coordinate_initializer;
        using dimension_list = typename base_type::dimension_list;
        using temporary_type = typename semantic_base::temporary_type;

        using expression_tag = xvariable_expression_tag;

        xvariable() = default;

        template <class C, class DM, class = enable_xvariable<C, DM>>
        explicit xvariable(C&& coords, DM&& dims);
        explicit xvariable(const coordinate_map& coords, const dimension_list& dims);
        explicit xvariable(coordinate_map&& coords, dimension_list&& dims);
        explicit xvariable(coordinate_initializer coords);

        template <class D, class C, class DM, class = enable_xvariable_t<C, DM>>
        explicit xvariable(D&& data, C&& coords, DM&& dims);

        template <class D>
        explicit xvariable(D&& data, const coordinate_map& coords, const dimension_list& dims);

        template <class D>
        explicit xvariable(D&& data, coordinate_map&& coords, dimension_list&& dims);

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

        data_closure_type m_data;

        data_type& data_impl() noexcept;
        const data_type& data_impl() const noexcept;

        friend class xvariable_base<xvariable<CCT, ECT>>;
    };

    template <class CCT, class ECT>
    std::ostream& operator<<(std::ostream& out, const xvariable<CCT, ECT>& v);

    /********************************
     * variable generator functions *
     ********************************/

    namespace detail
    {
        template <class T>
        struct variable_data_type
        {
            using type = xt::xoptional_assembly<xt::xarray<T>, xt::xarray<bool>>;
        };

        template <class T>
        using variable_data_type_t = typename variable_data_type<T>::type;

        template <class D, class C, class DM>
        struct xvariable_type1 : std::enable_if<is_coordinate_system<C, DM>::value,
            xvariable<xtl::closure_type_t<C>, xtl::closure_type_t<D>>>
        {
        };

        template <class D, class C, class DM>
        using xvariable_type1_t = typename xvariable_type1<D, C, DM>::type;

        template <class D, class C, class L>
        struct xvariable_type2 : std::enable_if<is_coordinate_map<C>::value && is_dimension_list<L>::value,
            xvariable<get_coordinate_type_t<C>, xtl::closure_type_t<D>>>
        {
        };

        template <class D, class C, class L>
        using xvariable_type2_t = typename xvariable_type2<D, C, L>::type;

        template <class T, class C, class DM>
        struct xvariable_type3 : std::enable_if<is_coordinate_system<C, DM>::value,
            xvariable<xtl::closure_type_t<C>, variable_data_type_t<T>>>
        {
        };

        template <class T, class C, class DM>
        using xvariable_type3_t = typename xvariable_type3<T, C, DM>::type;

        template <class T, class C, class L>
        struct xvariable_type4 : std::enable_if<is_coordinate_map<C>::value && is_dimension_list<L>::value,
            xvariable<get_coordinate_type_t<C>, variable_data_type_t<T>>>
        {
        };

        template <class T, class C, class L>
        using xvariable_type4_t = typename xvariable_type4<T, C, L>::type;
    }

    template <class D, class C, class DM>
    detail::xvariable_type1_t<D, C, DM> variable(D&& data, C&& coord, DM&& dims);

    template <class D, class C, class L>
    detail::xvariable_type2_t<D, C, L> variable(D&& data, C&& coord_map, L&& dim_list);

    template <class T, class C, class DM>
    detail::xvariable_type3_t<T, C, DM> variable(C&& coord, DM&& dims);

    template <class T, class C, class L>
    detail::xvariable_type4_t<T, C, L> variable(C&& coord_map, L&& dim_list);

    /****************************
     * xvariable implementation *
     ****************************/

    template <class CCT, class ECT>
    template <class C, class DM, class>
    inline xvariable<CCT, ECT>::xvariable(C&& coords, DM&& dims)
        : base_type(std::forward<C>(coords), std::forward<DM>(dims)),
          m_data(base_type::compute_shape())
    {
    }

    template <class CCT, class ECT>
    inline xvariable<CCT, ECT>::xvariable(const coordinate_map& coords, const dimension_list& dims)
        : base_type(coords, dims),
          m_data(base_type::compute_shape())
    {
    }

    template <class CCT, class ECT>
    inline xvariable<CCT, ECT>::xvariable(coordinate_map&& coords, dimension_list&& dims)
        : base_type(std::move(coords), std::move(dims)),
          m_data(base_type::compute_shape())
    {
    }

    template <class CCT, class ECT>
    inline xvariable<CCT, ECT>::xvariable(coordinate_initializer coords)
        : base_type(coords),
          m_data(base_type::compute_shape())
    {
    }

    template <class CCT, class ECT>
    template <class D, class C, class DM, class>
    inline xvariable<CCT, ECT>::xvariable(D&& data, C&& coords, DM&& dims)
        : base_type(std::forward<C>(coords), std::forward<DM>(dims)),
          m_data(std::forward<D>(data))
    {
    }

    template <class CCT, class ECT>
    template <class D>
    inline xvariable<CCT, ECT>::xvariable(D&& data, const coordinate_map& coords, const dimension_list& dims)
        : base_type(coords, dims),
          m_data(std::forward<D>(data))
    {
    }

    template <class CCT, class ECT>
    template <class D>
    inline xvariable<CCT, ECT>::xvariable(D&& data, coordinate_map&& coords, dimension_list&& dims)
        : base_type(std::move(coords), std::move(dims)),
          m_data(std::forward<D>(data))
    {
    }

    template <class CCT, class ECT>
    template <class D>
    inline xvariable<CCT, ECT>::xvariable(D&& data, coordinate_initializer coords)
        : base_type(coords),
          m_data(std::forward<D>(data))
    {
    }

    template <class CCT, class ECT>
    template <class E>
    inline xvariable<CCT, ECT>::xvariable(const xt::xexpression<E>& e)
        : base_type()
    {
        semantic_base::assign(e);
    }

    template <class CCT, class ECT>
    template <class E>
    inline auto xvariable<CCT, ECT>::operator=(const xt::xexpression<E>& e) -> self_type&
    {
        return semantic_base::assign(e);
    }

    template <class CCT, class ECT>
    inline auto xvariable<CCT, ECT>::data_impl() noexcept -> data_type&
    {
        return m_data;
    }

    template <class CCT, class ECT>
    inline auto xvariable<CCT, ECT>::data_impl() const noexcept -> const data_type&
    {
        return m_data;
    }

   /**************************************
    * generator functions implementation *
    **************************************/

    template <class D, class C, class DM>
    inline detail::xvariable_type1_t<D, C, DM> variable(D&& data, C&& coord, DM&& dims)
    {
        using type = detail::xvariable_type1_t<D, C, DM>;
        return type(std::forward<D>(data), std::forward<C>(coord), std::forward<DM>(dims));
    }

    template <class D, class C, class L>
    inline detail::xvariable_type2_t<D, C, L> variable(D&& data, C&& coord_map, L&& dim_list)
    {
        using type = detail::xvariable_type2_t<D, C, L>;
        using axis_type = typename std::decay_t<C>::mapped_type;
        using mapped_type = typename axis_type::mapped_type;
        using dimension_type = xdimension<typename std::decay_t<L>::value_type, mapped_type>;
        return type(std::forward<D>(data),
                    coordinate(std::forward<C>(coord_map)),
                    dimension_type(std::forward<L>(dim_list)));
    }

    template <class T, class C, class DM>
    inline detail::xvariable_type3_t<T, C, DM> variable(C&& coord, DM&& dims)
    {
        using type = detail::xvariable_type3_t<T, C, DM>;
        return type(std::forward<C>(coord), std::forward<DM>(dims));
    }

    template <class T, class C, class L>
    inline detail::xvariable_type4_t<T, C, L> variable(C&& coord_map, L&& dim_list)
    {
        using type = detail::xvariable_type4_t<T, C, L>;
        using axis_type = typename std::decay_t<C>::mapped_type;
        using mapped_type = typename axis_type::mapped_type;
        using dimension_type = xdimension<typename std::decay_t<L>::value_type, mapped_type>;
        return type(coordinate(std::forward<C>(coord_map)),
                    dimension_type(std::forward<L>(dim_list)));
    }

    template <class CCT, class ECT>
    inline std::ostream& operator<<(std::ostream& out, const xvariable<CCT, ECT>& v)
    {
        return print_variable_expression(out, v);
    }
}

#endif
