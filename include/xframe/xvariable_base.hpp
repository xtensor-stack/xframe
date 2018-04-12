/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVARIABLE_BASE_HPP
#define XFRAME_XVARIABLE_BASE_HPP

#include "xtensor/xnoalias.hpp"

#include "xcoordinate.hpp"
#include "xdimension.hpp"
#include "xselecting.hpp"

namespace xf
{
    using xt::noalias;

    template <class C , class DM>
    struct enable_xvariable
        : std::enable_if<is_coordinate<C>::value && is_dimension<DM>::value,
                         xt::void_t<>
          >
    {
    };

    template <class C, class DM>
    using enable_xvariable_t = typename enable_xvariable<C, DM>::type;

    template <class D>
    struct xvariable_inner_types;

    template <class D>
    class xvariable_base
    {
    public:

        using self_type = xvariable_base<D>;
        using derived_type = D;
        using inner_types = xvariable_inner_types<D>;

        using data_closure_type = typename inner_types::data_closure_type;
        using coordinate_closure_type = typename inner_types::coordinate_closure_type;
        static constexpr bool is_data_const = std::is_const<std::remove_reference_t<data_closure_type>>::value;

        using data_type = typename inner_types::data_type;
        using value_type = typename data_type::value_type;
        using reference = std::conditional_t<is_data_const,
                                             typename data_type::const_reference,
                                             typename data_type::reference>;
        using const_reference = typename data_type::const_reference;
        using pointer = std::conditional_t<is_data_const,
                                           typename data_type::const_pointer,
                                           typename data_type::pointer>;
        using const_pointer = typename data_type::const_pointer;
        using size_type = typename data_type::size_type;
        using difference_type = typename data_type::difference_type;

        using coordinate_type = typename inner_types::coordinate_type;
        using dimension_type = typename inner_types::dimension_type;
        using dimension_list = typename dimension_type::label_list;

        using coordinate_map = typename coordinate_type::map_type;
        using coordinate_initializer = std::initializer_list<typename coordinate_type::value_type>;
        using key_type = typename coordinate_map::key_type;

        template <std::size_t N = dynamic()>
        using selector_traits = xselector_traits<coordinate_type, dimension_type, N>;
        template <std::size_t N = dynamic()>
        using locator_type = typename selector_traits<N>::locator_type;
        template <std::size_t N = dynamic()>
        using locator_map_type = typename selector_traits<N>::locator_map_type;
        template <std::size_t N = dynamic()>
        using selector_type = typename selector_traits<N>::selector_type;
        template <std::size_t N = dynamic()>
        using selector_map_type = typename selector_traits<N>::selector_map_type;
        template <std::size_t N = dynamic()>
        using iselector_type = typename selector_traits<N>::iselector_type;
        template <std::size_t N = dynamic()>
        using iselector_map_type = typename selector_traits<N>::iselector_map_type;

        static const_reference missing();

        size_type size() const noexcept;
        constexpr size_type dimension() const noexcept;
        const dimension_list& dimension_labels() const noexcept;        
        const coordinate_type& coordinates() const noexcept;
        const dimension_type& dimension_mapping() const noexcept;

        void resize(const coordinate_type& coords, const dimension_type& dims);
        void resize(coordinate_type&& coords, dimension_type&& dims);

        void reshape(const coordinate_type& coords, const dimension_type& dims);
        void reshape(coordinate_type&& coords, dimension_type&& dims);

        template <class Join = DEFAULT_JOIN>
        xtrivial_broadcast broadcast_coordinates(coordinate_type& coords) const;
        bool broadcast_dimensions(dimension_type& dims, bool trivial_bc = false) const;

        data_type& data() noexcept;
        const data_type& data() const noexcept;

        template <class... Args>
        reference operator()(Args... args);

        template <class... Args>
        const_reference operator()(Args... args) const;

        template <class It>
        reference element(It first, It last);
        template <class It>
        const_reference element(It first, It last) const;

        template <class... Args>
        reference locate(Args&&... args);

        template <class... Args>
        const_reference locate(Args&&... args) const;

        template <std::size_t N = dynamic()>
        reference select(const selector_map_type<N>& selector);

        template <class Join = DEFAULT_JOIN, std::size_t N = dynamic()>
        const_reference select(const selector_map_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        reference select(selector_map_type<N>&& selector);

        template <class Join = DEFAULT_JOIN, std::size_t N = dynamic()>
        const_reference select(selector_map_type<N>&& selector) const;

        template <std::size_t N = dynamic()>
        reference iselect(const iselector_map_type<N>& selector);

        template <std::size_t N = dynamic()>
        const_reference iselect(const iselector_map_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        reference iselect(iselector_map_type<N>&& selector);

        template <std::size_t N = dynamic()>
        const_reference iselect(iselector_map_type<N>&& selector) const;

    protected:

        xvariable_base() = default;
        xvariable_base(coordinate_initializer coords);
        
        template <class C, class DM>
        xvariable_base(C&& coords, DM&& dims);

        template <class DM>
        xvariable_base(const coordinate_map& coords, DM&& dims);

        template <class DM>
        xvariable_base(coordinate_map&& coords, DM&& dims);

        ~xvariable_base() = default;

        xvariable_base(const xvariable_base&) = default;
        xvariable_base& operator=(const xvariable_base&) = default;

        xvariable_base(xvariable_base&&) = default;
        xvariable_base& operator=(xvariable_base&&) = default;

        typename data_type::shape_type compute_shape() const;

    private:

        static dimension_type make_dimension_mapping(coordinate_initializer coord);

        template <class C, class DM>
        void resize_impl(C&& coords, DM&& dims);

        template <class C, class DM>
        void reshape_impl(C&& coords, DM&& dims);

        template <class... Args>
        locator_type<sizeof...(Args)> build_locator(Args&&... args) const;

        template <std::size_t N, class T, class... Args>
        void fill_locator(locator_map_type<N>& loc, std::size_t index, T idx, Args&&... args) const;

        template <std::size_t N>
        void fill_locator(locator_map_type<N>& loc, std::size_t index) const;

        template <class S>
        reference select_impl(const S& selector);

        template <class S>
        const_reference select_impl(const S& selector) const;

        template <class S>
        const_reference select_outer(const S& selector) const;

        template <class Join, class S>
        const_reference select_join(const S& selector) const;

        derived_type& derived_cast() noexcept;
        const derived_type& derived_cast() const noexcept;

        coordinate_closure_type m_coordinate;
        dimension_type m_dimension_mapping;
    };

    /****************************
     * xvariable_base implementation *
     ****************************/

    template <class D>
    template <class C, class DM>
    inline xvariable_base<D>::xvariable_base(C&& coords, DM&& dims)
        : m_coordinate(std::forward<C>(coords)),
          m_dimension_mapping(std::forward<DM>(dims))
    {
        // TODO: add assertion on data.shape, coordinate and dimension_mapping
        // consistency
    }

    template <class D>
    inline xvariable_base<D>::xvariable_base(coordinate_initializer coords)
        : xvariable_base(coordinate_type(coords), make_dimension_mapping(coords))
    {
    }

    template <class D>
    template <class DM>
    inline xvariable_base<D>::xvariable_base(const coordinate_map& coords, DM&& dims)
        : xvariable_base(coordinate_type(coords), std::forward<DM>(dims))
    {
    }

    template <class D>
    template <class DM>
    inline xvariable_base<D>::xvariable_base(coordinate_map&& coords, DM&& dims)
        : xvariable_base(coordinate_type(std::move(coords)), std::forward<DM>(dims))
    {
    }

    template <class D>
    inline auto xvariable_base<D>::missing() -> const_reference
    {
        return detail::static_missing<const_reference>();
    }

    template <class D>
    inline auto xvariable_base<D>::size() const noexcept -> size_type
    {
        return data().size();
    }

    template <class D>
    inline constexpr auto xvariable_base<D>::dimension() const noexcept -> size_type
    {
        return data().dimension();
    }

    template <class D>
    inline auto xvariable_base<D>::dimension_labels() const noexcept -> const dimension_list&
    {
        return m_dimension_mapping.labels();
    }

    template <class D>
    inline auto xvariable_base<D>::coordinates() const noexcept -> const coordinate_type&
    {
        return m_coordinate;
    }

    template <class D>
    inline auto xvariable_base<D>::dimension_mapping() const noexcept -> const dimension_type&
    {
        return m_dimension_mapping;
    }
    
    template <class D>
    inline void xvariable_base<D>::resize(const coordinate_type& coords, const dimension_type& dims)
    {
        resize_impl(coords, dims);
    }

    template <class D>
    inline void xvariable_base<D>::resize(coordinate_type&& coords, dimension_type&& dims)
    {
        resize_impl(std::move(coords), std::move(dims));
    }
    
    template <class D>
    inline void xvariable_base<D>::reshape(const coordinate_type& coords, const dimension_type& dims)
    {
        reshape_impl(coords, dims);
    }

    template <class D>
    inline void xvariable_base<D>::reshape(coordinate_type&& coords, dimension_type&& dims)
    {
        reshape_impl(std::move(coords), std::move(dims));
    }
    
    template <class D>
    template <class Join>
    inline xtrivial_broadcast xvariable_base<D>::broadcast_coordinates(coordinate_type& coords) const
    {
        return xf::broadcast_coordinates<Join>(coords, this->coordinates());
    }

    template <class D>
    inline bool xvariable_base<D>::broadcast_dimensions(dimension_type& dims, bool trivial_bc) const
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

    template <class D>
    inline auto xvariable_base<D>::data() noexcept -> data_type&
    {
        return derived_cast().data_impl();
    }

    template <class D>
    inline auto xvariable_base<D>::data() const noexcept -> const data_type&
    {
        return derived_cast().data_impl();
    }

    template <class D>
    template <class... Args>
    inline auto xvariable_base<D>::operator()(Args... args) -> reference
    {
        return data()(args...);
    }

    template <class D>
    template <class... Args>
    inline auto xvariable_base<D>::operator()(Args... args) const -> const_reference
    {
        return data()(args...);
    }

    template <class D>
    template <class It>
    inline auto xvariable_base<D>::element(It first, It last) -> reference
    {
        return data().element(first, last);
    }

    template <class D>
    template <class It>
    inline auto xvariable_base<D>::element(It first, It last) const -> const_reference
    {
        return data().element(first, last);
    }

    template <class D>
    template <class... Args>
    inline auto xvariable_base<D>::locate(Args&&... args) -> reference
    {
        return select_impl(build_locator(std::forward<Args>(args)...));
    }

    template <class D>
    template <class... Args>
    inline auto xvariable_base<D>::locate(Args&&... args) const -> const_reference
    {

        return select_impl(build_locator(std::forward<Args>(args)...));
    }

    template <class D>
    inline auto xvariable_base<D>::compute_shape() const -> typename data_type::shape_type
    {
        using shape_type = typename data_type::shape_type;
        shape_type shape(m_dimension_mapping.size());
        for (auto& c : m_coordinate)
        {
            shape[m_dimension_mapping[c.first]] = c.second.size();
        }
        return shape;
    }

    template <class D>
    template <class C, class DM>
    inline void xvariable_base<D>::resize_impl(C&& coords, DM&& dims)
    {
        m_coordinate = std::forward<C>(coords);
        m_dimension_mapping = std::forward<DM>(dims);
        data().resize(compute_shape());
    }

    template <class D>
    template <class C, class DM>
    inline void xvariable_base<D>::reshape_impl(C&& coords, DM&& dims)
    {
        m_coordinate = std::forward<C>(coords);
        m_dimension_mapping = std::forward<DM>(dims);
        data().reshape(compute_shape());
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::select(const selector_map_type<N>& selector) -> reference
    {
        return select_impl(selector_type<N>(selector));
    }

    template <class D>
    template <class Join, std::size_t N>
    inline auto xvariable_base<D>::select(const selector_map_type<N>& selector) const -> const_reference
    {
        return select_join<Join>(selector_type<N>(selector));
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::select(selector_map_type<N>&& selector) -> reference
    {
        return select_impl(selector_type<N>(std::move(selector)));
    }

    template <class D>
    template <class Join, std::size_t N>
    inline auto xvariable_base<D>::select(selector_map_type<N>&& selector) const -> const_reference
    {
        return select_join<Join>(selector_type<N>(std::move(selector)));
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::iselect(const iselector_map_type<N>& selector) -> reference
    {
        return select_impl(iselector_type<N>(selector));
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::iselect(const iselector_map_type<N>& selector) const -> const_reference
    {
        return select_impl(iselector_type<N>(selector));
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::iselect(iselector_map_type<N>&& selector) -> reference
    {
        return select_impl(iselector_type<N>(std::move(selector)));
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::iselect(iselector_map_type<N>&& selector) const -> const_reference
    {
        return select_impl(iselector_type<N>(std::move(selector)));
    }
    
    template <class D>
    inline auto xvariable_base<D>::make_dimension_mapping(coordinate_initializer coord) -> dimension_type
    {
        dimension_list tmp(coord.size());
        std::transform(coord.begin(), coord.end(), tmp.begin(), [](const auto& p) { return p.first; });
        return dimension_type(std::move(tmp));
    }

    template <class D>
    template <class... Args>
    inline auto xvariable_base<D>::build_locator(Args&&... args) const -> locator_type<sizeof...(Args)>
    {
        constexpr std::size_t nb_args = sizeof...(Args);
        locator_map_type<nb_args> locator = xtl::make_sequence<locator_map_type<nb_args>>(nb_args);
        fill_locator<nb_args>(locator, std::size_t(0), std::forward<Args>(args)...);
        return locator_type<nb_args>(std::move(locator));
    }

    template <class D>
    template <std::size_t N, class T, class... Args>
    inline void xvariable_base<D>::fill_locator(locator_map_type<N>& loc, std::size_t index, T idx, Args&&... args) const
    {
        loc[index] = std::make_pair(index, idx);
        fill_locator<N>(loc, index + 1, std::forward<Args>(args)...);
    }

    template <class D>
    template <std::size_t N>
    inline void xvariable_base<D>::fill_locator(locator_map_type<N>& loc, std::size_t index) const
    {
    }

    template <class D>
    template <class S>
    inline auto xvariable_base<D>::select_impl(const S& selector) -> reference
    {
        typename S::index_type idx = selector.get_index(coordinates(), dimension_mapping());
        return data().element(idx.cbegin(), idx.cend());
    }

    template <class D>
    template <class S>
    inline auto xvariable_base<D>::select_impl(const S& selector) const -> const_reference
    {
        typename S::index_type idx = selector.get_index(coordinates(), dimension_mapping());
        return data().element(idx.cbegin(), idx.cend());
    }
    
    template <class D>
    template <class S>
    inline auto xvariable_base<D>::select_outer(const S& selector) const -> const_reference
    {
        typename S::outer_index_type idx = selector.get_outer_index(coordinates(), dimension_mapping());
        return idx.second ? data().element(idx.first.cbegin(), idx.first.cend()) : missing();
    }
    
    template <class D>
    template <class Join, class S>
    inline auto xvariable_base<D>::select_join(const S& selector) const -> const_reference
    {
        return xtl::mpl::static_if<Join::id() == join::inner::id()>([&](auto self)
        {
            return self(*this).select_impl(selector);
        }, /*else*/ [&](auto self)
        {
            return self(*this).select_outer(selector);
        });
    }

    template <class D>
    inline auto xvariable_base<D>::derived_cast() noexcept -> derived_type&
    {
        return *static_cast<derived_type*>(this);
    }

    template <class D>
    inline auto xvariable_base<D>::derived_cast() const noexcept -> const derived_type&
    {
        return *static_cast<const derived_type*>(this);
    }
}

#endif

