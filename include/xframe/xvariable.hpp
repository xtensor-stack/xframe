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

    template <class K, class VE, class FE, class L = DEFAULT_LABEL_LIST>
    class xvariable
    {
    public:

        using self_type = xvariable<K, VE, FE, L>;
        using data_type = xt::xoptional_assembly<VE, FE>;
        using value_type = typename data_type::value_type;
        using reference = typename data_type::reference;
        using const_reference = typename data_type::const_reference;
        using pointer = typename data_type::pointer;
        using const_pointer = typename data_type::const_pointer;
        using size_type = typename data_type::size_type;
        using difference_type = typename data_type::difference_type;

        using coordinate_type = xcoordinate<K, size_type, L>;
        using dimension_type = xaxis<K, size_type>;
        using dimension_list = typename dimension_type::label_list;

        using coordinate_map = typename coordinate_type::map_type;
        using coordinate_initializer = std::initializer_list<typename coordinate_type::value_type>;

        xvariable() = default;

        template <class D, class C, class DM, class = enable_xvariable_t<C, DM>>
        explicit xvariable(D&& data, C&& coords, DM&& dims);

        template <class D, class DM>
        explicit xvariable(D&& data, const coordinate_map& coords, DM&& dims);

        template <class D, class DM>
        explicit xvariable(D&& data, coordinate_map&& coords, DM&& dims);

        template <class D>
        xvariable(D&& data, coordinate_initializer coords);

        size_type size() const noexcept;
        constexpr size_type dimension() const noexcept;
        const dimension_list& dimension_labels() const noexcept;
        
        template <class... Args>
        reference operator()(Args... args);

        template <class... Args>
        const_reference operator()(Args... args) const;

        const data_type& data() const noexcept;
        const coordinate_type& coordinates() const noexcept;
        const dimension_type& dimension_mapping() const noexcept;

        template <std::size_t N>
        using selector_type = xselector<coordinate_type, N>;

        template <std::size_t N = std::numeric_limits<size_type>::max()>
        static constexpr selector_type<N> selection() noexcept;

        template <std::size_t N = std::numeric_limits<size_type>::max()>
        reference select(const selector_type<N>& selector);

        template <std::size_t N = std::numeric_limits<size_type>::max()>
        const_reference select(const selector_type<N>& selector) const;

    private:

        static dimension_type make_dimension_mapping(coordinate_initializer coord);

        data_type m_data;
        coordinate_type m_coordinate;
        dimension_type m_dimension_mapping;
    };

    template <class K, class VE, class FE, class L>
    bool operator==(const xvariable<K, VE, FE, L>& lhs, const xvariable<K, VE, FE, L>& rhs) noexcept;

    template <class K, class VE, class FE, class L>
    bool operator!=(const xvariable<K, VE, FE, L>& lhs, const xvariable<K, VE, FE, L>& rhs) noexcept;

    /****************************
     * xvariable implementation *
     ****************************/

    template <class K, class VE, class FE, class L>
    template <class D, class C, class DM, class>
    inline xvariable<K, VE, FE, L>::xvariable(D&& data, C&& coords, DM&& dims)
        : m_data(std::forward<D>(data)),
          m_coordinate(std::forward<C>(coords)),
          m_dimension_mapping(std::forward<DM>(dims))
    {
        // TODO: add assertion on data.shape, coordinate and dimension_mapping
        // consistency
    }

    template <class K, class VE, class FE, class L>
    template <class D, class DM>
    inline xvariable<K, VE, FE, L>::xvariable(D&& data, const coordinate_map& coords, DM&& dims)
        : xvariable(std::forward<D>(data), coordinate_type(coords), std::forward<DM>(dims))
    {
    }

    template <class K, class VE, class FE, class L>
    template <class D, class DM>
    inline xvariable<K, VE, FE, L>::xvariable(D&& data, coordinate_map&& coords, DM&& dims)
        : xvariable(std::forward<D>(data), coordinate_type(std::move(coords)), std::forward<DM>(dims))
    {
    }

    template <class K, class VE, class FE, class L>
    template <class D>
    inline xvariable<K, VE, FE, L>::xvariable(D&& data, coordinate_initializer coords)
        : xvariable(std::forward<D>(data), coordinate_type(coords), make_dimension_mapping(coords))
    {
    }

    template <class K, class VE, class FE, class L>
    inline auto xvariable<K, VE, FE, L>::size() const noexcept -> size_type
    {
        return m_data.size();
    }

    template <class K, class VE, class FE, class L>
    inline constexpr auto xvariable<K, VE, FE, L>::dimension() const noexcept -> size_type
    {
        return m_data.dimension();
    }

    template <class K, class VE, class FE, class L>
    inline auto xvariable<K, VE, FE, L>::dimension_labels() const noexcept -> const dimension_list&
    {
        return m_dimension_mapping.labels();
    }

    template <class K, class VE, class FE, class L>
    template <class... Args>
    inline auto xvariable<K, VE, FE, L>::operator()(Args... args) -> reference
    {
        return m_data(args...);
    }

    template <class K, class VE, class FE, class L>
    template <class... Args>
    inline auto xvariable<K, VE, FE, L>::operator()(Args... args) const -> const_reference
    {
        return m_data(args...);
    }

    template <class K, class VE, class FE, class L>
    inline auto xvariable<K, VE, FE, L>::data() const noexcept -> const data_type&
    {
        return m_data;
    }

    template <class K, class VE, class FE, class L>
    inline auto xvariable<K, VE, FE, L>::coordinates() const noexcept -> const coordinate_type&
    {
        return m_coordinate;
    }

    template <class K, class VE, class FE, class L>
    inline auto xvariable<K, VE, FE, L>::dimension_mapping() const noexcept -> const dimension_type&
    {
        return m_dimension_mapping;
    }

    template <class K, class VE, class FE, class L>
    template <std::size_t N>
    constexpr auto xvariable<K, VE, FE, L>::selection() noexcept -> selector_type<N>
    {
        return selector_type<N>();
    }

    template <class K, class VE, class FE, class L>
    template <std::size_t N>
    inline auto xvariable<K, VE, FE, L>::select(const selector_type<N>& selector) -> reference
    {
        typename selector_type<N>::index_type idx = selector.get_index(coordinates(), dimension_mapping());
        return m_data.element(idx.cbegin(), idx.cend());
    }

    template <class K, class VE, class FE, class L>
    template <std::size_t N>
    inline auto xvariable<K, VE, FE, L>::select(const selector_type<N>& selector) const -> const_reference
    {
        typename selector_type<N>::index_type idx = selector.get_index(coordinates(), dimension_mapping());
        return m_data.element(idx.cbegin(), idx.cend());
    }

    template <class K, class VE, class FE, class L>
    inline auto xvariable<K, VE, FE, L>::make_dimension_mapping(coordinate_initializer coord) -> dimension_type
    {
        dimension_list tmp(coord.size());
        std::transform(coord.begin(), coord.end(), tmp.begin(), [](const auto& p) { return p.first; });
        return dimension_type(std::move(tmp));
    }

    template <class K, class VE, class FE, class L>
    inline bool operator==(const xvariable<K, VE, FE, L>& lhs, const xvariable<K, VE, FE, L>& rhs) noexcept
    {
        return lhs.data() == rhs.data() &&
               lhs.coordinates() == rhs.coordinates() &&
               lhs.dimension_mapping() == rhs.dimension_mapping();
    }

    template <class K, class VE, class FE, class L>
    inline bool operator!=(const xvariable<K, VE, FE, L>& lhs, const xvariable<K, VE, FE, L>& rhs) noexcept
    {
        return !(lhs == rhs);
    }

}

#endif

