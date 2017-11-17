/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVARIABLE_BASE_HPP
#define XFRAME_XVARIABLE_BASE_HPP

#include "xcoordinate.hpp"
#include "xselecting.hpp"

namespace xf
{

    template <class C , class DM>
    struct enable_xvariable
        : std::enable_if<is_coordinate<C>::value && is_axis<DM>::value,
                         xt::void_t<>
          >
    {
    };

    template <class C, class DM>
    using enable_xvariable_t = typename enable_xvariable<C, DM>::type;

    constexpr std::size_t dynamic()
    {
        return std::numeric_limits<std::size_t>::max();
    }

    template <class D>
    struct xvariable_inner_types;

    template <class D>
    class xvariable_base
    {
    public:

        using self_type = xvariable_base<D>;
        using derived_type = D;
        using inner_types = xvariable_inner_types<D>;

        using data_type = typename inner_types::data_type;
        using value_type = typename data_type::value_type;
        using reference = typename data_type::reference;
        using const_reference = typename data_type::const_reference;
        using pointer = typename data_type::pointer;
        using const_pointer = typename data_type::const_pointer;
        using size_type = typename data_type::size_type;
        using difference_type = typename data_type::difference_type;

        using coordinate_type = typename inner_types::coordinate_type;
        using dimension_type = typename inner_types::dimension_type;
        using dimension_list = typename dimension_type::label_list;

        using coordinate_map = typename coordinate_type::map_type;
        using coordinate_initializer = std::initializer_list<typename coordinate_type::value_type>;

        size_type size() const noexcept;
        constexpr size_type dimension() const noexcept;
        const dimension_list& dimension_labels() const noexcept;
        
        template <class... Args>
        reference operator()(Args... args);

        template <class... Args>
        const_reference operator()(Args... args) const;

        data_type& data() noexcept;
        const data_type& data() const noexcept;
        const coordinate_type& coordinates() const noexcept;
        const dimension_type& dimension_mapping() const noexcept;

        template <std::size_t N = dynamic()>
        using selector_type = xselector<coordinate_type, N>;
        template <std::size_t N = dynamic()>
        using selector_map_type = typename selector_type<N>::map_type;
        template <std::size_t N = dynamic()>
        using iselector_type = xiselector<coordinate_type, N>;
        template <std::size_t N = dynamic()>
        using iselector_map_type = typename iselector_type<N>::map_type;
        template <std::size_t N = dynamic()>
        using locator_type = xlocator<coordinate_type, N>;
        template <std::size_t N = dynamic()>
        using locator_map_type = typename locator_type<N>::map_type;

        template <std::size_t N = dynamic()>
        reference select(const selector_map_type<N>& selector);

        template <std::size_t N = std::numeric_limits<size_type>::max()>
        const_reference select(const selector_map_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        reference select(selector_map_type<N>&& selector);

        template <std::size_t N = dynamic()>
        const_reference select(selector_map_type<N>&& selector) const;

        template <std::size_t N = dynamic()>
        reference iselect(const iselector_map_type<N>& selector);

        template <std::size_t N = dynamic()>
        const_reference iselect(const iselector_map_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        reference iselect(iselector_map_type<N>&& selector);

        template <std::size_t N = dynamic()>
        const_reference iselect(iselector_map_type<N>&& selector) const;

        template <std::size_t N = dynamic()>
        reference locate(const locator_map_type<N>& locator);

        template <std::size_t N = dynamic()>
        const_reference locate(const locator_map_type<N>& locator) const;

        template <std::size_t N = dynamic()>
        reference locate(locator_map_type<N>&& locator);

        template <std::size_t N = dynamic()>
        const_reference locate(locator_map_type<N>&& locator) const;

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

    private:

        static dimension_type make_dimension_mapping(coordinate_initializer coord);

        template <class S>
        reference select_impl(const S& selector);

        template <class S>
        const_reference select_impl(const S& selector) const;

        derived_type& derived_cast() noexcept;
        const derived_type& derived_cast() const noexcept;

        coordinate_type m_coordinate;
        dimension_type m_dimension_mapping;
    };

    template <class D>
    bool operator==(const xvariable_base<D>& lhs, const xvariable_base<D>& rhs) noexcept;

    template <class D>
    bool operator!=(const xvariable_base<D>& lhs, const xvariable_base<D>& rhs) noexcept;

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
    template <std::size_t N>
    inline auto xvariable_base<D>::select(const selector_map_type<N>& selector) -> reference
    {
        return select_impl(selector_type<N>(selector));
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::select(const selector_map_type<N>& selector) const -> const_reference
    {
        return select_impl(selector_type<N>(selector));
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::select(selector_map_type<N>&& selector) -> reference
    {
        return select_impl(selector_type<N>(std::move(selector)));
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::select(selector_map_type<N>&& selector) const -> const_reference
    {
        return select_impl(selector_type<N>(std::move(selector)));
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
    template <std::size_t N>
    inline auto xvariable_base<D>::locate(const locator_map_type<N>& locator) -> reference
    {
        return select_impl(locator_type<N>(locator));
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::locate(const locator_map_type<N>& locator) const -> const_reference
    {
        return select_impl(locator_type<N>(locator));
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::locate(locator_map_type<N>&& locator) -> reference
    {
        return select_impl(locator_type<N>(std::move(locator)));
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::locate(locator_map_type<N>&& locator) const -> const_reference
    {
        return select_impl(locator_type<N>(std::move(locator)));
    }
    
    template <class D>
    inline auto xvariable_base<D>::make_dimension_mapping(coordinate_initializer coord) -> dimension_type
    {
        dimension_list tmp(coord.size());
        std::transform(coord.begin(), coord.end(), tmp.begin(), [](const auto& p) { return p.first; });
        return dimension_type(std::move(tmp));
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
    inline bool operator==(const xvariable_base<D>& lhs, const xvariable_base<D>& rhs) noexcept
    {
        return lhs.data() == rhs.data() &&
               lhs.coordinates() == rhs.coordinates() &&
               lhs.dimension_mapping() == rhs.dimension_mapping();
    }

    template <class D>
    inline bool operator!=(const xvariable_base<D>& lhs, const xvariable_base<D>& rhs) noexcept
    {
        return !(lhs == rhs);
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

