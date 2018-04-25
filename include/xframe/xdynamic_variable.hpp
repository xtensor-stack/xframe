/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XDYNAMIC_VARIABLE_HPP
#define XFRAME_XDYNAMIC_VARIABLE_HPP

#include "xdynamic_variable_impl.hpp"

namespace xf
{

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

        self_type& operator=(const self_type&);
        self_type& operator=(self_type&&);

        template <std::size_t N = dynamic()>
        xtl::any select(const selector_sequence_type<N>& sel);

        template <class Join = DEFAULT_JOIN, std::size_t N = dynamic()>
        xtl::any select(const selector_sequence_type<N>& sel) const;

        template <std::size_t N = dynamic()>
        xtl::any select(selector_sequence_type<N>&& sel);

        template <class Join = DEFAULT_JOIN, std::size_t N = dynamic()>
        xtl::any select(selector_sequence_type<N>&& sel) const;

        template <std::size_t N = dynamic()>
        xtl::any iselect(const iselector_sequence_type<N>& sel);

        template <std::size_t N = dynamic()>
        xtl::any iselect(const iselector_sequence_type<N>& sel) const;

        template <std::size_t N = dynamic()>
        xtl::any iselect(iselector_sequence_type<N>&& sel);

        template <std::size_t N = dynamic()>
        xtl::any iselect(iselector_sequence_type<N>&& sel) const;

    private:

        wrapper_type* p_wrapper;
    };

    template <class V>
    auto make_dynamic(V&& variable);

    /************************************
     * xdynamic_variable implementation *
     ************************************/

    template <class C, class DM>
    template <class V, class>
    inline xdynamic_variable<C, DM>::xdynamic_variable(V&& variable)
        : p_wrapper(new xdynamic_leaf<std::decay_t<V>>(std::forward<V>(variable)))
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
    template <std::size_t N>
    inline xtl::any xdynamic_variable<C, DM>::select(const selector_sequence_type<N>& sel)
    {
        return p_wrapper->template select<N>(sel);
    }

    template <class C, class DM>
    template <class Join, std::size_t N>
    inline xtl::any xdynamic_variable<C, DM>::select(const selector_sequence_type<N>& sel) const
    {
        return p_wrapper->template select<Join, N>(sel);
    }

    template <class C, class DM>
    template <std::size_t N>
    inline xtl::any xdynamic_variable<C, DM>::select(selector_sequence_type<N>&& sel)
    {
        return p_wrapper->template select<N>(std::move(sel));
    }

    template <class C, class DM>
    template <class Join, std::size_t N>
    inline xtl::any xdynamic_variable<C, DM>::select(selector_sequence_type<N>&& sel) const
    {
        return p_wrapper->template select<Join, N>(std::move(sel));
    }

    template <class C, class DM>
    template <std::size_t N>
    inline xtl::any xdynamic_variable<C, DM>::iselect(const iselector_sequence_type<N>& sel)
    {
        return p_wrapper->template iselect<N>(sel);
    }

    template <class C, class DM>
    template <std::size_t N>
    inline xtl::any xdynamic_variable<C, DM>::iselect(const iselector_sequence_type<N>& sel) const
    {
        return p_wrapper->template iselect<N>(sel);
    }

    template <class C, class DM>
    template <std::size_t N>
    inline xtl::any xdynamic_variable<C, DM>::iselect(iselector_sequence_type<N>&& sel)
    {
        return p_wrapper->template iselect<N>(std::move(sel));
    }

    template <class C, class DM>
    template <std::size_t N>
    inline xtl::any xdynamic_variable<C, DM>::iselect(iselector_sequence_type<N>&& sel) const
    {
        return p_wrapper->template iselect<N>(std::move(sel));
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
