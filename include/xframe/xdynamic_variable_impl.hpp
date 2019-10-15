/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XDYNAMIC_VARIABLE_IMPL_HPP
#define XFRAME_XDYNAMIC_VARIABLE_IMPL_HPP

#include "xtl/xany.hpp"
#include "xtl/xhierarchy_generator.hpp"
#include "xtl/xvariant.hpp"

#include "xvariable.hpp"

namespace xf
{
    /********************************
     * traits for dynamic variables *
     ********************************/

    namespace detail
    {
        template <class T>
        struct xdynamic_traits_impl
        {
            using value_type = xtl::xoptional<T, bool>;
            using reference = xtl::xoptional<T&, bool&>;
            using const_reference = xtl::xoptional<const T&, const bool&>;
            using pointer = xtl::xclosure_pointer<reference>;
            using const_pointer = xtl::xclosure_pointer<const_reference>;
        };

        template <class T>
        struct xdynamic_traits_impl<xtl::xoptional<T, bool>>
            : xdynamic_traits_impl<T>
        {
        };

        template <>
        struct xdynamic_traits_impl<xtl::any>
        {
            using value_type = xtl::any;
            using reference = xtl::any;
            using const_reference = xtl::any;
            using pointer = xtl::xclosure_pointer<reference>;
            using const_pointer = xtl::xclosure_pointer<const_reference>;
        };

        template <class... T>
        struct xdynamic_traits_impl<xtl::variant<T...>>
        {
            using value_type = xtl::variant<xtl::xoptional<T, bool>...>;
            using reference = xtl::variant<xtl::xoptional<T&, bool&>...>;
            using const_reference = xtl::variant<xtl::xoptional<const T&, const bool&>...>;
            using pointer = xtl::xclosure_pointer<reference>;
            using const_pointer = xtl::xclosure_pointer<const_reference>;
        };
    }

    template <class C, class DM, class T, std::size_t N>
    struct xdynamic_traits : xselector_traits<C, DM, N>
    {
        using return_type = T;
        using traits_type = detail::xdynamic_traits_impl<T>;
        using value_type = typename traits_type::value_type;
        using reference = typename traits_type::reference;
        using const_reference = typename traits_type::const_reference;
        using pointer = typename traits_type::pointer;
        using const_pointer = typename traits_type::const_pointer;
    };

    namespace detail
    {
        template <class C, class DM, class T, class L>
        struct xdynamic_traits_list_impl;

        template <class C, class DM, class T, std::size_t... I>
        struct xdynamic_traits_list_impl<C, DM, T, std::index_sequence<I...>>
        {
            using type = xtl::mpl::vector<xdynamic_traits<C, DM, T, I + 1>..., xdynamic_traits<C, DM, T, dynamic()>>;
        };
    }

    template <class C, class DM, class T, std::size_t N>
    using xdynamic_traits_list = typename detail::xdynamic_traits_list_impl<C, DM, T, std::make_index_sequence<N>>::type;

    /*****************
     * xdynamic_base *
     *****************/

    template <class T>
    class xdynamic_base
    {
    public:

        using index_type = typename T::index_type;
        using selector_sequence_type = typename T::selector_sequence_type;
        using iselector_sequence_type = typename T::iselector_sequence_type;
        using locator_sequence_type = typename T::locator_sequence_type;

        using value_type = typename T::value_type;
        using reference = typename T::reference;
        using const_reference = typename T::const_reference;

        virtual ~xdynamic_base() {}

        virtual reference do_element(const index_type&) = 0;
        virtual const_reference do_element(const index_type&) const = 0;
        virtual reference do_element(index_type&&) = 0;
        virtual const_reference do_element(index_type&&) const = 0;

        virtual reference do_locate_element(const locator_sequence_type&) = 0;
        virtual const_reference do_locate_element(const locator_sequence_type&) const = 0;
        virtual reference do_locate_element(locator_sequence_type&&) = 0;
        virtual const_reference do_locate_element(locator_sequence_type&&) const = 0;

        virtual reference do_select(const selector_sequence_type&) = 0;
        virtual const_reference do_select(const selector_sequence_type&, join::outer) const = 0;
        virtual const_reference do_select(const selector_sequence_type&, join::inner) const = 0;
        virtual reference do_select(selector_sequence_type&&) = 0;
        virtual const_reference do_select(selector_sequence_type&&, join::outer) const = 0;
        virtual const_reference do_select(selector_sequence_type&&, join::inner) const = 0;

        virtual reference do_iselect(const iselector_sequence_type&) = 0;
        virtual const_reference do_iselect(const iselector_sequence_type&) const = 0;
        virtual reference do_iselect(iselector_sequence_type&&) = 0;
        virtual const_reference do_iselect(iselector_sequence_type&&) const = 0;
    };

    /*********************
     * xvariable_wrapper *
     *********************/

    template <class C, class DM, class T>
    class xvariable_wrapper : public xtl::xscatter_hierarchy_generator<xdynamic_traits_list<C, DM, T, XFRAME_STATIC_DIMENSION_LIMIT>, xdynamic_base>
    {
    public:

        using self_type = xvariable_wrapper<C, DM, T>;

        template <std::size_t N = dynamic()>
        using traits_type = xdynamic_traits<C, DM, T, N>;
        template <std::size_t N = dynamic()>
        using index_type = typename traits_type<N>::index_type;
        template <std::size_t N = dynamic()>
        using selector_sequence_type = typename traits_type<N>::selector_sequence_type;
        template <std::size_t N = dynamic()>
        using iselector_sequence_type = typename traits_type<N>::iselector_sequence_type;
        template <std::size_t N = dynamic()>
        using locator_sequence_type = typename traits_type<N>::locator_sequence_type;

        using coordinate_type = C;
        using dimension_type = DM;
        using dimension_list = typename dimension_type::label_list;

        using value_type = typename traits_type<>::value_type;
        using reference = typename traits_type<>::reference;
        using const_reference = typename traits_type<>::const_reference;
        using pointer = typename traits_type<>::pointer;
        using const_pointer = typename traits_type<>::const_pointer;
        using size_type = typename coordinate_type::size_type;
        using difference_type = typename coordinate_type::difference_type;

        using shape_type = xt::svector<size_type>;

        virtual ~xvariable_wrapper() {}

        xvariable_wrapper(self_type&&) = delete;
        xvariable_wrapper& operator=(const self_type&) = delete;
        xvariable_wrapper& operator=(self_type&&) = delete;

        virtual self_type* clone() const = 0;

        virtual size_type size() const = 0;
        virtual size_type dimension() const = 0;
        virtual const dimension_list& dimension_labels() const = 0;
        virtual const coordinate_type& coordinates() const = 0;
        virtual const dimension_type& dimension_mapping() const = 0;

        virtual xtrivial_broadcast broadcast_coordinates(coordinate_type& coords, join::outer) const = 0;
        virtual xtrivial_broadcast broadcast_coordinates(coordinate_type& coords, join::inner) const = 0;
        virtual bool broadcast_dimensions(dimension_type& dims, bool trivial_bc) const = 0;

        virtual const shape_type& shape() const noexcept = 0;

        template <std::size_t N>
        reference element(const index_type<N>& index);

        template <std::size_t N>
        const_reference element(const index_type<N>& index) const;

        template <std::size_t N>
        reference element(index_type<N>&& index);

        template <std::size_t N>
        const_reference element(index_type<N>&& index) const;

        template <std::size_t N>
        reference locate_element(const locator_sequence_type<N>& loc);

        template <std::size_t N>
        const_reference locate_element(const locator_sequence_type<N>& loc) const;

        template <std::size_t N>
        reference locate_element(locator_sequence_type<N>&& loc);

        template <std::size_t N>
        const_reference locate_element(locator_sequence_type<N>&& loc) const;

        template <std::size_t N>
        reference select(const selector_sequence_type<N>& sel);

        template <class Join, std::size_t N>
        const_reference select(const selector_sequence_type<N>& sel) const;

        template <std::size_t N>
        reference select(selector_sequence_type<N>&& sel);

        template <class Join, std::size_t N>
        const_reference select(selector_sequence_type<N>&& sel) const;

        template <std::size_t N>
        reference iselect(const iselector_sequence_type<N>& sel);

        template <std::size_t N>
        const_reference iselect(const iselector_sequence_type<N>& sel) const;

        template <std::size_t N>
        reference iselect(iselector_sequence_type<N>&& sel);

        template <std::size_t N>
        const_reference iselect(iselector_sequence_type<N>&& sel) const;

        virtual std::ostream& print(std::ostream& out) const = 0;

    protected:

        xvariable_wrapper() = default;
        xvariable_wrapper(const self_type&) = default;
    };

    /**************************
     * xvariable_wrapper_impl *
     **************************/

    template <class V, class T>
    class xvariable_wrapper_impl : public xvariable_wrapper<typename V::coordinate_type, typename V::dimension_type, T>
    {
    public:

        using variable_type = V;
        using self_type = xvariable_wrapper_impl<variable_type, T>;
        using base_type = xvariable_wrapper<typename V::coordinate_type, typename V::dimension_type, T>;
        using size_type = typename base_type::size_type;
        using coordinate_type = typename base_type::coordinate_type;
        using dimension_type = typename base_type::dimension_type;
        using dimension_list = typename base_type::dimension_list;
        using shape_type = typename base_type::shape_type;

        virtual ~xvariable_wrapper_impl() {}

        xvariable_wrapper_impl(self_type&&) = delete;
        xvariable_wrapper_impl& operator=(const self_type&) = delete;
        xvariable_wrapper_impl& operator=(self_type&&) = delete;

        size_type size() const override;
        size_type dimension() const override;
        const dimension_list& dimension_labels() const override;
        const coordinate_type& coordinates() const override;
        const dimension_type& dimension_mapping() const override;

        xtrivial_broadcast broadcast_coordinates(coordinate_type& coords, join::outer) const override;
        xtrivial_broadcast broadcast_coordinates(coordinate_type& coords, join::inner) const override;
        bool broadcast_dimensions(dimension_type& dims, bool trivial_bc) const override;

        const shape_type& shape() const noexcept override;

        std::ostream& print(std::ostream& out) const override;

    protected:

        xvariable_wrapper_impl(const variable_type& variable);
        xvariable_wrapper_impl(variable_type&& variable);
        xvariable_wrapper_impl(const self_type& rhs) = default;

        variable_type& get_variable();
        const variable_type& get_variable() const;

    private:

        variable_type m_variable;
    };

    /***************************
     * xdynamic_implementation *
     ***************************/

    template <class T, class B>
    class xdynamic_implementation : public B
    {
    public:

        using self_type = xdynamic_implementation<T, B>;
        using base_type = B;
        using index_type = typename T::index_type;
        using selector_sequence_type = typename T::selector_sequence_type;
        using iselector_sequence_type = typename T::iselector_sequence_type;
        using locator_sequence_type = typename T::locator_sequence_type;

        using value_type = typename T::value_type;
        using reference = typename T::reference;
        using const_reference = typename T::const_reference;

        static constexpr std::size_t static_dimension = T::static_dimension;

        virtual ~xdynamic_implementation() {}

        xdynamic_implementation(self_type&&) = delete;
        xdynamic_implementation& operator=(const self_type&) = delete;
        xdynamic_implementation& operator=(self_type&&) = delete;

        reference do_element(const index_type&) override;
        const_reference do_element(const index_type&) const override;
        reference do_element(index_type&&) override;
        const_reference do_element(index_type&&) const override;

        reference do_locate_element(const locator_sequence_type&) override;
        const_reference do_locate_element(const locator_sequence_type&) const override;
        reference do_locate_element(locator_sequence_type&&) override;
        const_reference do_locate_element(locator_sequence_type&&) const override;

        reference do_select(const selector_sequence_type&) override;
        const_reference do_select(const selector_sequence_type&, join::outer) const override;
        const_reference do_select(const selector_sequence_type&, join::inner) const override;
        reference do_select(selector_sequence_type&&) override;
        const_reference do_select(selector_sequence_type&&, join::outer) const override;
        const_reference do_select(selector_sequence_type&&, join::inner) const override;

        reference do_iselect(const iselector_sequence_type&) override;
        const_reference do_iselect(const iselector_sequence_type&) const override;
        reference do_iselect(iselector_sequence_type&&) override;
        const_reference do_iselect(iselector_sequence_type&&) const override;

    protected:

        template <class V>
        xdynamic_implementation(V&& variable);

        xdynamic_implementation(const self_type&) = default;
    };

    /*****************
     * xdynamic_leaf *
     *****************/

    namespace detail
    {
        template <class V, class T>
        using xdynamic_linear_hierarchy = xtl::xlinear_hierarchy_generator<
            xdynamic_traits_list<typename V::coordinate_type,
            typename V::dimension_type,
            T,
            XFRAME_STATIC_DIMENSION_LIMIT>,
            xdynamic_implementation,
            xvariable_wrapper_impl<V, T>>;
    }

    template <class V, class T>
    class xdynamic_leaf : public detail::xdynamic_linear_hierarchy<V, T>
    {
    public:

        using self_type = xdynamic_leaf<V, T>;
        using base_type = detail::xdynamic_linear_hierarchy<V, T>;

        xdynamic_leaf(const V&);
        xdynamic_leaf(V&&);
        virtual ~xdynamic_leaf() {}

        self_type* clone() const override;

        xdynamic_leaf(self_type&&) = delete;
        xdynamic_leaf& operator=(const self_type&) = delete;
        xdynamic_leaf& operator=(self_type&&) = delete;

    private:

        xdynamic_leaf(const self_type&) = default;

    };

    /************************************
     * xvariable_wrapper implementation *
     ************************************/

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xvariable_wrapper<C, DM, T>::element(const index_type<N>& index) -> reference
    {
        xdynamic_base<xdynamic_traits<C, DM, T, N>>& base = *this;
        return base.do_element(index);
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xvariable_wrapper<C, DM, T>::element(const index_type<N>& index) const -> const_reference
    {
        const xdynamic_base<xdynamic_traits<C, DM, T, N>>& base = *this;
        return base.do_element(index);
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xvariable_wrapper<C, DM, T>::element(index_type<N>&& index) -> reference
    {
        xdynamic_base<xdynamic_traits<C, DM, T, N>>& base = *this;
        return base.do_element(std::move(index));
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xvariable_wrapper<C, DM, T>::element(index_type<N>&& index) const -> const_reference
    {
        const xdynamic_base<xdynamic_traits<C, DM, T, N>>& base = *this;
        return base.do_element(std::move(index));
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xvariable_wrapper<C, DM, T>::locate_element(const locator_sequence_type<N>& loc) -> reference
    {
        xdynamic_base<xdynamic_traits<C, DM, T, N>>& base = *this;
        return base.do_locate_element(loc);
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xvariable_wrapper<C, DM, T>::locate_element(const locator_sequence_type<N>& loc) const -> const_reference
    {
        const xdynamic_base<xdynamic_traits<C, DM, T, N>>& base = *this;
        return base.do_locate_element(loc);
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xvariable_wrapper<C, DM, T>::locate_element(locator_sequence_type<N>&& loc) -> reference
    {
        xdynamic_base<xdynamic_traits<C, DM, T, N>>& base = *this;
        return base.do_locate_element(std::move(loc));
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xvariable_wrapper<C, DM, T>::locate_element(locator_sequence_type<N>&& loc) const -> const_reference
    {
        const xdynamic_base<xdynamic_traits<C, DM, T, N>>& base = *this;
        return base.do_locate_element(std::move(loc));
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xvariable_wrapper<C, DM, T>::select(const selector_sequence_type<N>& sel) -> reference
    {
        xdynamic_base<xdynamic_traits<C, DM, T, N>>& base = *this;
        return base.do_select(sel);
    }

    template <class C, class DM, class T>
    template <class Join, std::size_t N>
    inline auto xvariable_wrapper<C, DM, T>::select(const selector_sequence_type<N>& sel) const -> const_reference
    {
        const xdynamic_base<xdynamic_traits<C, DM, T, N>>& base = *this;
        return base.do_select(sel, Join());
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xvariable_wrapper<C, DM, T>::select(selector_sequence_type<N>&& sel) -> reference
    {
        xdynamic_base<xdynamic_traits<C, DM, T, N>>& base = *this;
        return base.do_select(std::move(sel));
    }

    template <class C, class DM, class T>
    template <class Join, std::size_t N>
    inline auto xvariable_wrapper<C, DM, T>::select(selector_sequence_type<N>&& sel) const -> const_reference
    {
        const xdynamic_base<xdynamic_traits<C, DM, T, N>>& base = *this;
        return base.do_select(std::move(sel), Join());
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xvariable_wrapper<C, DM, T>::iselect(const iselector_sequence_type<N>& sel) -> reference
    {
        xdynamic_base<xdynamic_traits<C, DM, T, N>>& base = *this;
        return base.do_iselect(sel);
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xvariable_wrapper<C, DM, T>::iselect(const iselector_sequence_type<N>& sel) const -> const_reference
    {
        const xdynamic_base<xdynamic_traits<C, DM, T, N>>& base = *this;
        return base.do_iselect(sel);
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xvariable_wrapper<C, DM, T>::iselect(iselector_sequence_type<N>&& sel) -> reference
    {
        xdynamic_base<xdynamic_traits<C, DM, T, N>>& base = *this;
        return base.do_iselect(std::move(sel));
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xvariable_wrapper<C, DM, T>::iselect(iselector_sequence_type<N>&& sel) const -> const_reference
    {
        const xdynamic_base<xdynamic_traits<C, DM, T, N>>& base = *this;
        return base.do_iselect(std::move(sel));
    }

    /*****************************************
     * xvariable_wrapper_impl implementation *
     *****************************************/

    template <class V, class T>
    inline xvariable_wrapper_impl<V, T>::xvariable_wrapper_impl(const variable_type& variable)
        : m_variable(variable)
    {
    }

    template <class V, class T>
    inline xvariable_wrapper_impl<V, T>::xvariable_wrapper_impl(variable_type&& variable)
        : m_variable(std::move(variable))
    {
    }

    template <class V, class T>
    inline auto xvariable_wrapper_impl<V, T>::get_variable() -> variable_type&
    {
        return m_variable;
    }

    template <class V, class T>
    inline auto xvariable_wrapper_impl<V, T>::get_variable() const -> const variable_type&
    {
        return m_variable;
    }

    template <class V, class T>
    auto xvariable_wrapper_impl<V, T>::size() const -> size_type
    {
        return m_variable.size();
    }

    template <class V, class T>
    auto xvariable_wrapper_impl<V, T>::dimension() const -> size_type
    {
        return m_variable.dimension();
    }

    template <class V, class T>
    auto xvariable_wrapper_impl<V, T>::dimension_labels() const -> const dimension_list&
    {
        return m_variable.dimension_labels();
    }

    template <class V, class T>
    auto xvariable_wrapper_impl<V, T>::coordinates() const -> const coordinate_type&
    {
        return m_variable.coordinates();
    }

    template <class V, class T>
    auto xvariable_wrapper_impl<V, T>::dimension_mapping() const -> const dimension_type&
    {
        return m_variable.dimension_mapping();
    }

    template <class V, class T>
    xtrivial_broadcast xvariable_wrapper_impl<V, T>::broadcast_coordinates(coordinate_type& coords, join::outer) const
    {
        return m_variable.template broadcast_coordinates<join::outer>(coords);
    }

    template <class V, class T>
    xtrivial_broadcast xvariable_wrapper_impl<V, T>::broadcast_coordinates(coordinate_type& coords, join::inner) const
    {
        return m_variable.template broadcast_coordinates<join::inner>(coords);
    }

    template <class V, class T>
    bool xvariable_wrapper_impl<V, T>::broadcast_dimensions(dimension_type& dims, bool trivial_bc) const
    {
        return m_variable.broadcast_dimensions(dims, trivial_bc);
    }

    template <class V, class T>
    auto xvariable_wrapper_impl<V, T>::shape() const noexcept -> const shape_type&
    {
        return xtl::forward_sequence<shape_type, decltype(m_variable.shape())>(m_variable.shape());
    }

    template <class V, class T>
    std::ostream& xvariable_wrapper_impl<V, T>::print(std::ostream& out) const
    {
        return out << m_variable;
    }

   /***************************
    * xdynamic_implementation *
    ***************************/

    template <class T, class B>
    template <class V>
    inline xdynamic_implementation<T, B>::xdynamic_implementation(V&& variable)
        : base_type(std::forward<V>(variable))
    {
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_element(const index_type& index) -> reference
    {
        return this->get_variable().template element<static_dimension>(index);
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_element(const index_type& index) const -> const_reference
    {
        return this->get_variable().template element<static_dimension>(index);
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_element(index_type&& index) -> reference
    {
        return this->get_variable().template element<static_dimension>(std::move(index));
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_element(index_type&& index) const -> const_reference
    {
        return this->get_variable().template element<static_dimension>(std::move(index));
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_locate_element(const locator_sequence_type& loc) -> reference
    {
        return this->get_variable().template locate_element<static_dimension>(loc);
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_locate_element(const locator_sequence_type& loc) const -> const_reference
    {
        return this->get_variable().template locate_element<static_dimension>(loc);
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_locate_element(locator_sequence_type&& loc) -> reference
    {
        return this->get_variable().template locate_element<static_dimension>(std::move(loc));
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_locate_element(locator_sequence_type&& loc) const -> const_reference
    {
        return this->get_variable().template locate_element<static_dimension>(std::move(loc));
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_select(const selector_sequence_type& sel) -> reference
    {
        return this->get_variable().template select<static_dimension>(sel);
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_select(const selector_sequence_type& sel, join::outer) const -> const_reference
    {
        return this->get_variable().template select<join::outer, static_dimension>(sel);
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_select(const selector_sequence_type& sel, join::inner) const -> const_reference
    {
        return this->get_variable().template select<join::inner, static_dimension>(sel);
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_select(selector_sequence_type&& sel) -> reference
    {
        return this->get_variable().template select<static_dimension>(std::move(sel));
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_select(selector_sequence_type&& sel, join::outer) const -> const_reference
    {
        return this->get_variable().template select<join::outer, static_dimension>(std::move(sel));
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_select(selector_sequence_type&& sel, join::inner) const -> const_reference
    {
        return this->get_variable().template select<join::inner, static_dimension>(std::move(sel));
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_iselect(const iselector_sequence_type& sel) -> reference
    {
        return this->get_variable().template iselect<static_dimension>(sel);
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_iselect(const iselector_sequence_type& sel) const -> const_reference
    {
        return this->get_variable().template iselect<static_dimension>(sel);
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_iselect(iselector_sequence_type&& sel) -> reference
    {
        return this->get_variable().template iselect<static_dimension>(std::move(sel));
    }

    template <class T, class B>
    auto xdynamic_implementation<T, B>::do_iselect(iselector_sequence_type&& sel) const -> const_reference
    {
        return this->get_variable().template iselect<static_dimension>(std::move(sel));
    }

    /********************************
     * xdynamic_leaf implementation *
     ********************************/

    template <class V, class T>
    inline xdynamic_leaf<V, T>::xdynamic_leaf(const V& variable)
        : base_type(variable)
    {
    }

    template <class V, class T>
    inline xdynamic_leaf<V, T>::xdynamic_leaf(V&& variable)
        : base_type(std::move(variable))
    {
    }

    template <class V, class T>
    inline auto xdynamic_leaf<V, T>::clone() const -> self_type*
    {
        return new self_type(*this);
    }
}

#endif
