/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
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

    // T should be one of the following options:
    // - xtl::any
    // - xtl::variant<T0, T1, ... Tn>
    // - a simple value type
    template <class C, class DM, class T = xtl::any>
    class xdynamic_variable
    {
    public:

        using self_type = xdynamic_variable<C, DM, T>;
        using wrapper_type = xvariable_wrapper<C, DM, T>;
        template <std::size_t N = dynamic()>
        using index_type = typename wrapper_type::template index_type<N>;
        template <std::size_t N = dynamic()>
        using selector_sequence_type = typename wrapper_type::template selector_sequence_type<N>;
        template <std::size_t N = dynamic()>
        using iselector_sequence_type = typename wrapper_type::template iselector_sequence_type<N>;
        template <std::size_t N = dynamic()>
        using locator_sequence_type = typename wrapper_type::template locator_sequence_type<N>;

        using value_type = typename wrapper_type::value_type;
        using reference = typename wrapper_type::reference;
        using const_reference = typename wrapper_type::const_reference;
        using pointer = typename wrapper_type::pointer;
        using const_pointer = typename wrapper_type::const_pointer;
        using size_type = typename wrapper_type::size_type;
        using difference_type = typename wrapper_type::difference_type;

        using coordinate_type = typename wrapper_type::coordinate_type;
        using dimension_type = typename wrapper_type::dimension_type;
        using dimension_list = typename wrapper_type::dimension_list;
        using shape_type = typename wrapper_type::shape_type;

        template <class V, class = std::enable_if_t<!std::is_same<std::decay_t<V>, self_type>::value, void>>
        explicit xdynamic_variable(V&&);

        ~xdynamic_variable();

        xdynamic_variable(const self_type&);
        xdynamic_variable(self_type&&);

        self_type& operator=(const self_type&);
        self_type& operator=(self_type&&);

        size_type size() const;
        size_type dimension() const;
        const dimension_list& dimension_labels() const;
        const coordinate_type& coordinates() const;
        const dimension_type& dimension_mapping() const;

        template <class Join>
        xtrivial_broadcast broadcast_coordinates(coordinate_type& coords) const;

        bool broadcast_dimensions(dimension_type& dims, bool trivial_bc = false) const;

        const shape_type& shape() const noexcept;

        template <class... Args>
        reference operator()(Args... args);

        template <class... Args>
        const_reference operator()(Args... args) const;

        template <std::size_t N = dynamic()>
        reference element(const index_type<N>& index);

        template <std::size_t N = dynamic()>
        const_reference element(const index_type<N>& index) const;

        template <std::size_t N = dynamic()>
        reference element(index_type<N>&& index);

        template <std::size_t N = dynamic()>
        const_reference element(index_type<N>&& index) const;

        template <class... Args>
        reference locate(Args&&... args);

        template <class... Args>
        const_reference locate(Args&&... args) const;

        template <std::size_t N = dynamic()>
        reference locate_element(const locator_sequence_type<N>& loc);

        template <std::size_t N = dynamic()>
        const_reference locate_element(const locator_sequence_type<N>& loc) const;

        template <std::size_t N = dynamic()>
        reference locate_element(locator_sequence_type<N>&& loc);

        template <std::size_t N = dynamic()>
        const_reference locate_element(locator_sequence_type<N>&& loc) const;

        template <std::size_t N = dynamic()>
        reference select(const selector_sequence_type<N>& sel);

        template <class Join = XFRAME_DEFAULT_JOIN, std::size_t N = dynamic()>
        const_reference select(const selector_sequence_type<N>& sel) const;

        template <std::size_t N = dynamic()>
        reference select(selector_sequence_type<N>&& sel);

        template <class Join = XFRAME_DEFAULT_JOIN, std::size_t N = dynamic()>
        const_reference select(selector_sequence_type<N>&& sel) const;

        template <std::size_t N = dynamic()>
        reference iselect(const iselector_sequence_type<N>& sel);

        template <std::size_t N = dynamic()>
        const_reference iselect(const iselector_sequence_type<N>& sel) const;

        template <std::size_t N = dynamic()>
        reference iselect(iselector_sequence_type<N>&& sel);

        template <std::size_t N = dynamic()>
        const_reference iselect(iselector_sequence_type<N>&& sel) const;

        std::ostream& print(std::ostream& out) const;

    private:

        template <class... Args>
        index_type<> make_index(Args... args);

        wrapper_type* p_wrapper;
    };

    template <class C, class DM, class T>
    std::ostream& operator<<(std::ostream& out, const xdynamic_variable<C, DM, T>& v);

    template < class T = xtl::any, class V>
    auto make_dynamic(V&& variable);

    /************************************
     * xdynamic_variable implementation *
     ************************************/

    template <class C, class DM, class T>
    template <class V, class>
    inline xdynamic_variable<C, DM, T>::xdynamic_variable(V&& variable)
        : p_wrapper(new xdynamic_leaf<std::decay_t<V>, T>(std::forward<V>(variable)))
    {
    }

    template <class C, class DM, class T>
    inline xdynamic_variable<C, DM, T>::~xdynamic_variable()
    {
        delete p_wrapper;
        p_wrapper = nullptr;
    }

    template <class C, class DM, class T>
    inline xdynamic_variable<C, DM, T>::xdynamic_variable(const self_type& rhs)
        : p_wrapper(rhs.p_wrapper->clone())
    {
    }

    template <class C, class DM, class T>
    inline xdynamic_variable<C, DM, T>::xdynamic_variable(self_type&& rhs)
        : p_wrapper(rhs.p_wrapper)
    {
        rhs.p_wrapper = nullptr;
    }

    template <class C, class DM, class T>
    inline auto xdynamic_variable<C, DM, T>::operator=(const self_type& rhs) -> self_type&
    {
        if (this != &rhs)
        {
            self_type tmp(rhs);
            std::swap(p_wrapper, tmp.p_wrapper);
        }
        return *this;
    }

    template <class C, class DM, class T>
    inline auto xdynamic_variable<C, DM, T>::operator=(self_type&& rhs) -> self_type&
    {
        std::swap(p_wrapper, rhs.p_wrapper);
        return *this;
    }

    template <class C, class DM, class T>
    inline auto xdynamic_variable<C, DM, T>::size() const -> size_type
    {
        return p_wrapper->size();
    }

    template <class C, class DM, class T>
    inline auto xdynamic_variable<C, DM, T>::dimension() const -> size_type
    {
        return p_wrapper->dimension();
    }

    template <class C, class DM, class T>
    inline auto xdynamic_variable<C, DM, T>::dimension_labels() const -> const dimension_list&
    {
        return p_wrapper->dimension_labels();
    }

    template <class C, class DM, class T>
    inline auto xdynamic_variable<C, DM, T>::coordinates() const -> const coordinate_type&
    {
        return p_wrapper->coordinates();
    }

    template <class C, class DM, class T>
    inline auto xdynamic_variable<C, DM, T>::dimension_mapping() const -> const dimension_type&
    {
        return p_wrapper->dimension_mapping();
    }

    template <class C, class DM, class T>
    template <class Join>
    inline xtrivial_broadcast xdynamic_variable<C, DM, T>::broadcast_coordinates(coordinate_type& coords) const
    {
        return p_wrapper->broadcast_coordinates(coords, Join());
    }

    template <class C, class DM, class T>
    inline bool xdynamic_variable<C, DM, T>::broadcast_dimensions(dimension_type& dims, bool trivial_bc) const
    {
        return p_wrapper->broadcast_dimensions(dims, trivial_bc);
    }

    template <class C, class DM, class T>
    inline auto xdynamic_variable<C, DM, T>::shape() const noexcept -> const shape_type&
    {
        return p_wrapper->shape();
    }

    template <class C, class DM, class T>
    template <class... Args>
    inline auto xdynamic_variable<C, DM, T>::operator()(Args... args) -> reference
    {
        return element(make_index(args...));
    }

    template <class C, class DM, class T>
    template <class... Args>
    inline auto xdynamic_variable<C, DM, T>::operator()(Args... args) const -> const_reference
    {
        return element(make_index(args...));
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xdynamic_variable<C, DM, T>::element(const index_type<N>& index) -> reference
    {
        return p_wrapper->template element<N>(index);
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xdynamic_variable<C, DM, T>::element(const index_type<N>& index) const -> const_reference
    {
        return p_wrapper->template element<N>(index);
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xdynamic_variable<C, DM, T>::element(index_type<N>&& index) -> reference
    {
        return p_wrapper->template element<N>(std::move(index));
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xdynamic_variable<C, DM, T>::element(index_type<N>&& index) const -> const_reference
    {
        return p_wrapper->template element<N>(std::move(index));
    }

    template <class C, class DM, class T>
    template <class... Args>
    inline auto xdynamic_variable<C, DM, T>::locate(Args&&... args) -> reference
    {
        locator_sequence_type<> loc = { std::forward<Args>(args)... };
        return locate_element(std::move(loc));
    }

    template <class C, class DM, class T>
    template <class... Args>
    inline auto xdynamic_variable<C, DM, T>::locate(Args&&... args) const -> const_reference
    {
        locator_sequence_type<> loc = { std::forward<Args>(args)... };
        return locate_element(std::move(loc));
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xdynamic_variable<C, DM, T>::locate_element(const locator_sequence_type<N>& loc) -> reference
    {
        return p_wrapper->template locate_element<N>(loc);
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xdynamic_variable<C, DM, T>::locate_element(const locator_sequence_type<N>& loc) const -> const_reference
    {
        return p_wrapper->template locate_element<N>(loc);
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xdynamic_variable<C, DM, T>::locate_element(locator_sequence_type<N>&& loc) -> reference
    {
        return p_wrapper->template locate_element<N>(std::move(loc));
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xdynamic_variable<C, DM, T>::locate_element(locator_sequence_type<N>&& loc) const -> const_reference
    {
        return p_wrapper->template locate_element<N>(std::move(loc));
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xdynamic_variable<C, DM, T>::select(const selector_sequence_type<N>& sel) -> reference
    {
        return p_wrapper->template select<N>(sel);
    }

    template <class C, class DM, class T>
    template <class Join, std::size_t N>
    inline auto xdynamic_variable<C, DM, T>::select(const selector_sequence_type<N>& sel) const -> const_reference
    {
        return p_wrapper->template select<Join, N>(sel);
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xdynamic_variable<C, DM, T>::select(selector_sequence_type<N>&& sel) -> reference
    {
        return p_wrapper->template select<N>(std::move(sel));
    }

    template <class C, class DM, class T>
    template <class Join, std::size_t N>
    inline auto xdynamic_variable<C, DM, T>::select(selector_sequence_type<N>&& sel) const -> const_reference
    {
        return p_wrapper->template select<Join, N>(std::move(sel));
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xdynamic_variable<C, DM, T>::iselect(const iselector_sequence_type<N>& sel) -> reference
    {
        return p_wrapper->template iselect<N>(sel);
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xdynamic_variable<C, DM, T>::iselect(const iselector_sequence_type<N>& sel) const -> const_reference
    {
        return p_wrapper->template iselect<N>(sel);
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xdynamic_variable<C, DM, T>::iselect(iselector_sequence_type<N>&& sel) -> reference
    {
        return p_wrapper->template iselect<N>(std::move(sel));
    }

    template <class C, class DM, class T>
    template <std::size_t N>
    inline auto xdynamic_variable<C, DM, T>::iselect(iselector_sequence_type<N>&& sel) const -> const_reference
    {
        return p_wrapper->template iselect<N>(std::move(sel));
    }

    template <class C, class DM, class T>
    inline std::ostream& xdynamic_variable<C, DM, T>::print(std::ostream& out) const
    {
        return p_wrapper->print(out);
    }

    template <class C, class DM, class T>
    template <class... Args>
    inline auto xdynamic_variable<C, DM, T>::make_index(Args... args) -> index_type<>
    {
        using size_type = typename index_type<>::value_type;
        index_type<> res = { static_cast<size_type>(args)... };
        return res;
    }

    template <class C, class DM, class T>
    inline std::ostream& operator<<(std::ostream& out, const xdynamic_variable<C, DM, T>& v)
    {
        return v.print(out);
    }

    template <class T, class V>
    inline auto make_dynamic(V&& variable)
    {
        using coordinate_type = typename std::decay_t<V>::coordinate_type;
        using dimension_type = typename std::decay_t<V>::dimension_type;
        return xdynamic_variable<coordinate_type, dimension_type, T>(std::forward<V>(variable));
    }
}

#endif
