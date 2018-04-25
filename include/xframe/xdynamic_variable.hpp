/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XDYNAMIC_VARIABLE_HPP
#define XFRAME_XDYNAMIC_VARIABLE_HPP

#include "xtl/xany.hpp"
#include "xtl/xhierarchy_generator.hpp"

#include "xvariable.hpp"

namespace xf
{
    namespace detail
    {
        template <class C, class DM, class L>
        struct xdynamic_traits_list_impl;

        template <class C, class DM, std::size_t... I>
        struct xdynamic_traits_list_impl<C, DM, std::index_sequence<I...>>
        {
            using type = xtl::mpl::vector<xselector_traits<C, DM, I + 1>..., xselector_traits<C, DM, dynamic()>>;
        };
    }

    template <class C, class DM, std::size_t N>
    using xdynamic_traits_list = typename detail::xdynamic_traits_list_impl<C, DM, std::make_index_sequence<N>>::type;

    /**********************
     * xdynamic_interface *
     **********************/

    template <class T>
    class xdynamic_interface
    {
    public:

        using selector_sequence_type = typename T::selector_sequence_type;
        using iselector_sequence_type = typename T::iselector_sequence_type;

        virtual ~xdynamic_interface() {}

        virtual xtl::any do_select(const selector_sequence_type&, join::outer) const = 0;
        virtual xtl::any do_select(const selector_sequence_type&, join::inner) const = 0;

        virtual xtl::any do_iselect(const iselector_sequence_type&) const = 0;
    };

    /*********************
     * xvariable_wrapper *
     *********************/

    template <class C, class DM>
    class xvariable_wrapper : public xtl::xscatter_hierarchy_generator<xdynamic_traits_list<C, DM, STATIC_DIMENSION_LIMIT>, xdynamic_interface>
    {
    public:

        using self_type = xvariable_wrapper<C, DM>;

        template <std::size_t N = dynamic()>
        using traits_type = xselector_traits<C, DM, N>;
        template <std::size_t N = dynamic()>
        using selector_sequence_type = typename traits_type<N>::selector_sequence_type;
        template <std::size_t N = dynamic()>
        using iselector_sequence_type = typename traits_type<N>::iselector_sequence_type;

        virtual ~xvariable_wrapper() {}

        xvariable_wrapper(self_type&&) = delete;
        xvariable_wrapper& operator=(const self_type&) = delete;
        xvariable_wrapper& operator=(self_type&&) = delete;

        virtual self_type* clone() const = 0;

        template <class Join, std::size_t N>
        xtl::any select(const selector_sequence_type<N>& sel) const;

        template <std::size_t N>
        xtl::any iselect(const iselector_sequence_type<N>& sel) const;

    protected:

        xvariable_wrapper() = default;
        xvariable_wrapper(const self_type&) = default;
    };

    /*********************
     * xdynamic_variable *
     *********************/
    
    template <class C, class DM>
    class xdynamic_variable
    {
    public:

        using self_type = xdynamic_variable<C, DM>;
        using wrapper_type = xvariable_wrapper<C, DM>;
        template <std::size_t N = dynamic()>
        using selector_sequence_type = typename wrapper_type::template selector_sequence_type<N>;
        template <std::size_t N = dynamic()>
        using iselector_sequence_type = typename wrapper_type::template iselector_sequence_type<N>;

        template <class V, class = std::enable_if_t<!std::is_same<std::decay_t<V>, self_type>::value, void>>
        explicit xdynamic_variable(V&&);

        ~xdynamic_variable();

        xdynamic_variable(const self_type&);
        xdynamic_variable(self_type&&);

        xdynamic_variable& operator=(const xdynamic_variable&);
        xdynamic_variable& operator=(xdynamic_variable&&);

        template <class Join = DEFAULT_JOIN, std::size_t N = std::numeric_limits<std::size_t>::max()>
        xtl::any select(const selector_sequence_type<N>& sel) const;

        template <std::size_t N = std::numeric_limits<std::size_t>::max()>
        xtl::any iselect(const iselector_sequence_type<N>& sel) const;

    private:

        wrapper_type* p_wrapper;
    };
    
    /**************************
     * xvariable_wrapper_impl *
     **************************/

    template <class V>
    class xvariable_wrapper_impl : public xvariable_wrapper<typename V::coordinate_type, typename V::dimension_type>
    {
    public:

        using variable_type = V;
        using self_type = xvariable_wrapper_impl<variable_type>;

        virtual ~xvariable_wrapper_impl() {}

        xvariable_wrapper_impl(self_type&&) = delete;
        xvariable_wrapper_impl& operator=(const self_type&) = delete;
        xvariable_wrapper_impl& operator=(self_type&&) = delete;

    protected:

        xvariable_wrapper_impl(const variable_type& variable);
        xvariable_wrapper_impl(variable_type&& variable);
        xvariable_wrapper_impl(const self_type& rhs) = default;

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
        using selector_sequence_type = typename T::selector_sequence_type;
        using iselector_sequence_type = typename T::iselector_sequence_type;

        static constexpr std::size_t static_dimension = T::static_dimension;

        virtual ~xdynamic_implementation() {}

        xdynamic_implementation(self_type&&) = delete;
        xdynamic_implementation& operator=(const self_type&) = delete;
        xdynamic_implementation& operator=(self_type&&) = delete;

        xtl::any do_select(const selector_sequence_type&, join::outer) const override;
        xtl::any do_select(const selector_sequence_type&, join::inner) const override;

        xtl::any do_iselect(const iselector_sequence_type&) const override;

    protected:

        template <class V>
        xdynamic_implementation(V&& variable);

        xdynamic_implementation(const self_type&) = default;
    };

    /**************************
     * xdynamic_variable_impl *
     **************************/

    namespace detail
    {
        template <class V>
        using xdynamic_linear_hierarchy = xtl::xlinear_hierarchy_generator<
                                                   xdynamic_traits_list<typename V::coordinate_type,
                                                                        typename V::dimension_type,
                                                                        STATIC_DIMENSION_LIMIT>,
                                                   xdynamic_implementation,
                                                   xvariable_wrapper_impl<V>>;
    }

    template <class V>
    class xdynamic_variable_impl : public detail::xdynamic_linear_hierarchy<V>
    {
    public:

        using self_type = xdynamic_variable_impl<V>;
        using base_type = detail::xdynamic_linear_hierarchy<V>;

        xdynamic_variable_impl(const V&);
        xdynamic_variable_impl(V&&);
        virtual ~xdynamic_variable_impl() {}

        self_type* clone() const override;

        xdynamic_variable_impl(self_type&&) = delete;
        xdynamic_variable_impl& operator=(const self_type&) = delete;
        xdynamic_variable_impl& operator=(self_type&&) = delete;

    private:

        xdynamic_variable_impl(const self_type&) = default;

    };

    template <class V>
    auto make_dynamic(V&& variable);

    /************************************
     * xvariable_wrapper implementation *
     ************************************/

    template <class C, class DM>
    template <class Join, std::size_t N>
    inline xtl::any xvariable_wrapper<C, DM>::select(const selector_sequence_type<N>& sel) const
    {
        const xdynamic_interface<xselector_traits<C, DM, N>>& base = *this;
        return base.do_select(sel, Join());
    }

    template <class C, class DM>
    template <std::size_t N>
    inline xtl::any xvariable_wrapper<C, DM>::iselect(const iselector_sequence_type<N>& sel) const
    {
        const xdynamic_interface<xselector_traits<C, DM, N>>& base = *this;
        return base.do_iselect(sel);
    }

    /************************************
     * xdynamic_variable implementation *
     ************************************/

    template <class C, class DM>
    template <class V, class>
    inline xdynamic_variable<C, DM>::xdynamic_variable(V&& variable)
        : p_wrapper(new xdynamic_variable_impl<std::decay_t<V>>(std::forward<V>(variable)))
    {
    }

    template <class C, class DM>
    inline xdynamic_variable<C, DM>::~xdynamic_variable()
    {
        delete p_wrapper;
        p_wrapper = nullptr;
    }

    template <class C, class DM>
    inline xdynamic_variable<C, DM>::xdynamic_variable(const self_type& rhs)
        : p_wrapper(rhs.p_wrapper->clone())
    {
    }

    template <class C, class DM>
    inline xdynamic_variable<C, DM>::xdynamic_variable(self_type&& rhs)
        : p_wrapper(rhs.p_wrapper)
    {
        rhs.p_wrapper = nullptr;
    }

    template <class C, class DM>
    inline auto xdynamic_variable<C, DM>::operator=(const self_type& rhs) -> self_type&
    {
        if (this != &rhs)
        {
            self_type tmp(rhs);
            std::swap(p_wrapper, tmp.p_wrapper);
        }
        return *this;
    }

    template <class C, class DM>
    inline auto xdynamic_variable<C, DM>::operator=(self_type&& rhs) -> self_type&
    {
        std::swap(p_wrapper, rhs.p_wrapper);
        return *this;
    }

    template <class C, class DM>
    template <class Join, std::size_t N>
    inline xtl::any xdynamic_variable<C, DM>::select(const selector_sequence_type<N>& sel) const
    {
        return p_wrapper->template select<Join, N>(sel);
    }

    template <class C, class DM>
    template <std::size_t N>
    inline xtl::any xdynamic_variable<C, DM>::iselect(const iselector_sequence_type<N>& sel) const
    {
        return p_wrapper->template iselect<N>(sel);
    }

    /*****************************************
     * xvariable_wrapper_impl implementation *
     *****************************************/

    template <class V>
    inline xvariable_wrapper_impl<V>::xvariable_wrapper_impl(const variable_type& variable)
        : m_variable(variable)
    {
    }

    template <class V>
    inline xvariable_wrapper_impl<V>::xvariable_wrapper_impl(variable_type&& variable)
        : m_variable(std::move(variable))
    {
    }

    template <class V>
    inline auto xvariable_wrapper_impl<V>::get_variable() const -> const variable_type&
    {
        return m_variable;
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
    inline xtl::any
    xdynamic_implementation<T, B>::do_select(const selector_sequence_type& sel, join::outer) const
    {
        return this->get_variable().template select<join::outer, static_dimension>(sel);
    }

    template <class T, class B>
    inline xtl::any
    xdynamic_implementation<T, B>::do_select(const selector_sequence_type& sel, join::inner) const
    {
        return this->get_variable().template select<join::inner, static_dimension>(sel);
    }

    template <class T, class B>
    inline xtl::any
    xdynamic_implementation<T, B>::do_iselect(const iselector_sequence_type& sel) const
    {
        return this->get_variable().template iselect<static_dimension>(sel);
    }

    /*****************************************
     * xdynamic_variable_impl implementation *
     *****************************************/

    template <class V>
    inline xdynamic_variable_impl<V>::xdynamic_variable_impl(const V& variable)
        : base_type(variable)
    {
    }

    template <class V>
    inline xdynamic_variable_impl<V>::xdynamic_variable_impl(V&& variable)
        : base_type(std::move(variable))
    {
    }

    template <class V>
    inline auto xdynamic_variable_impl<V>::clone() const -> self_type*
    {
        return new self_type(*this);
    }

    template <class V>
    inline auto make_dynamic(V&& variable)
    {
        using coordinate_type = typename std::decay_t<V>::coordinate_type;
        using dimension_type = typename std::decay_t<V>::dimension_type;
        return xdynamic_variable<coordinate_type, dimension_type>(std::forward<V>(variable));
    }
}

#endif
