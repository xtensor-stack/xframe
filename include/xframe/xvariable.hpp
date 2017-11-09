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

    template <class VE, class FE, class K, class L = DEFAULT_LABEL_LIST>
    class xvariable
    {
    public:

        using self_type = xvariable<VE, FE, K, L>;
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

    private:

        static dimension_type make_dimension_mapping(coordinate_initializer coord);

        data_type m_data;
        coordinate_type m_coordinate;
        dimension_type m_dimension_mapping;
    };

    template <class VE, class FE, class K, class L>
    bool operator==(const xvariable<VE, FE, K, L>& lhs, const xvariable<VE, FE, K, L>& rhs) noexcept;

    template <class VE, class FE, class K, class L>
    bool operator!=(const xvariable<VE, FE, K, L>& lhs, const xvariable<VE, FE, K, L>& rhs) noexcept;

    /****************************
     * xvariable implementation *
     ****************************/

    template <class VE, class FE, class K, class L>
    template <class D, class C, class DM, class>
    inline xvariable<VE, FE, K, L>::xvariable(D&& data, C&& coords, DM&& dims)
        : m_data(std::forward<D>(data)),
          m_coordinate(std::forward<C>(coords)),
          m_dimension_mapping(std::forward<DM>(dims))
    {
        // TODO: add assertion on data.shape, coordinate and dimension_mapping
        // consistency
    }

    template <class VE, class FE, class K, class L>
    template <class D, class DM>
    inline xvariable<VE, FE, K, L>::xvariable(D&& data, const coordinate_map& coords, DM&& dims)
        : xvariable(std::forward<D>(data), coordinate_type(coords), std::forward<DM>(dims))
    {
    }

    template <class VE, class FE, class K, class L>
    template <class D, class DM>
    inline xvariable<VE, FE, K, L>::xvariable(D&& data, coordinate_map&& coords, DM&& dims)
        : xvariable(std::forward<D>(data), coordinate_type(std::move(coords)), std::forward<DM>(dims))
    {
    }

    template <class VE, class FE, class K, class L>
    template <class D>
    inline xvariable<VE, FE, K, L>::xvariable(D&& data, coordinate_initializer coords)
        : xvariable(std::forward<D>(data), coordinate_type(coords), make_dimension_mapping(coords))
    {
    }

    template <class VE, class FE, class K, class L>
    inline auto xvariable<VE, FE, K, L>::size() const noexcept -> size_type
    {
        return m_data.size();
    }

    template <class VE, class FE, class K, class L>
    inline constexpr auto xvariable<VE, FE, K, L>::dimension() const noexcept -> size_type
    {
        return m_data.dimension();
    }

    template <class VE, class FE, class K, class L>
    inline auto xvariable<VE, FE, K, L>::dimension_labels() const noexcept -> const dimension_list&
    {
        return m_dimension_mapping.labels();
    }

    template <class VE, class FE, class K, class L>
    template <class... Args>
    inline auto xvariable<VE, FE, K, L>::operator()(Args... args) -> reference
    {
        return m_data(args...);
    }

    template <class VE, class FE, class K, class L>
    template <class... Args>
    inline auto xvariable<VE, FE, K, L>::operator()(Args... args) const -> const_reference
    {
        return m_data(args...);
    }

    template <class VE, class FE, class K, class L>
    inline auto xvariable<VE, FE, K, L>::data() const noexcept -> const data_type&
    {
        return m_data;
    }

    template <class VE, class FE, class K, class L>
    inline auto xvariable<VE, FE, K, L>::coordinates() const noexcept -> const coordinate_type&
    {
        return m_coordinate;
    }

    template <class VE, class FE, class K, class L>
    inline auto xvariable<VE, FE, K, L>::dimension_mapping() const noexcept -> const dimension_type&
    {
        return m_dimension_mapping;
    }

    template <class VE, class FE, class K, class L>
    inline auto xvariable<VE, FE, K, L>::make_dimension_mapping(coordinate_initializer coord) -> dimension_type
    {
        dimension_list tmp(coord.size());
        std::transform(coord.begin(), coord.end(), tmp.begin(), [](const auto& p) { return p.first; });
        return dimension_type(std::move(tmp));
    }

    template <class VE, class FE, class K, class L>
    inline bool operator==(const xvariable<VE, FE, K, L>& lhs, const xvariable<VE, FE, K, L>& rhs) noexcept
    {
        return lhs.data() == rhs.data() &&
               lhs.coordinates() == rhs.coordinates() &&
               lhs.dimension_mapping() == rhs.dimension_mapping();
    }

    template <class VE, class FE, class K, class L>
    inline bool operator!=(const xvariable<VE, FE, K, L>& lhs, const xvariable<VE, FE, K, L>& rhs) noexcept
    {
        return !(lhs == rhs);
    }

}

#endif

