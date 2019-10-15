/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XNAMED_AXIS_HPP
#define XFRAME_XNAMED_AXIS_HPP

#include "xaxis_math.hpp"
#include "xaxis.hpp"
#include "xaxis_default.hpp"
#include "xaxis_variant.hpp"

namespace xf
{

    /**
     * @class xnamed_axis
     * @brief Class modeling a dimension name - axis pair in a coordinate system.
     *
     * The xnamed_axis class stores a dimension name and an xaxis as an
     * xaxis_variant.
     *
     * @tparam K the dimension name type.
     * @tparam T the integer type used to represent positions in the underlying
     *           xaxis_variant.
     * @sa xaxis_variant, xaxis
     */
    template <class K, class T, class MT = hash_map_tag, class L = XFRAME_DEFAULT_LABEL_LIST, class LT = xtl::mpl::cast_t<L, xtl::variant>>
    class xnamed_axis : public xt::xexpression<xnamed_axis<K, T, MT, L, LT>>
    {
    public:

        using name_type = K;
        using axis_variant_type = xaxis_variant<L, T, MT>;
        using size_type = typename axis_variant_type::size_type;

        using value_type = LT;
        using reference = value_type;
        using const_reference = const value_type;
        using pointer = value_type*;
        using const_pointer = const value_type*;

        using expression_tag = xaxis_expression_tag;

        template <class A>
        xnamed_axis(const name_type& name, A&& axis);
        template <class A>
        xnamed_axis(name_type&& name, A&& axis);

        const name_type& name() const & noexcept;
        const axis_variant_type& axis() const & noexcept;
        const value_type& label(size_type i) const & noexcept;

    private:

        name_type m_name;
        axis_variant_type m_axis;
    };

    /******************************
     * xnamed_axis implementation *
     ******************************/

    /**
     * Builds an xnamed_axis.
     * @param name the dimension name.
     * @param axis the underlying xaxis.
     */
    template <class K, class T, class MT, class L, class LT>
    template <class A>
    inline xnamed_axis<K, T, MT, L, LT>::xnamed_axis(const name_type& name, A&& axis)
        : m_name(name), m_axis(std::forward<A>(axis))
    {
    }

    /**
     * Builds an xnamed_axis.
     * @param name the dimension name.
     * @param axis the underlying xaxis.
     */
    template <class K, class T, class MT, class L, class LT>
    template <class A>
    inline xnamed_axis<K, T, MT, L, LT>::xnamed_axis(name_type&& name, A&& axis)
        : m_name(std::move(name)), m_axis(std::forward<A>(axis))
    {
    }

    /**
     * Returns the name of the xnamed_axis.
     * @return the dimension name.
     */
    template <class K, class T, class MT, class L, class LT>
    inline auto xnamed_axis<K, T, MT, L, LT>::name() const & noexcept -> const name_type&
    {
        return m_name;
    }

    /**
     * Returns the underlying xaxis_variant.
     * @return the xaxis_variant.
     * @sa xaxis_variant
     */
    template <class K, class T, class MT, class L, class LT>
    inline auto xnamed_axis<K, T, MT, L, LT>::axis() const & noexcept -> const axis_variant_type&
    {
        return m_axis;
    }

    /**
     * Returns the label at the given position of the underlying xaxis.
     * @param i the position.
     * @return the label at the given position of the underlying xaxis.
     */
    template <class K, class T, class MT, class L, class LT>
    inline auto xnamed_axis<K, T, MT, L, LT>::label(size_type i) const & noexcept -> const LT&
    {
        return get_labels<LT>(m_axis)[i];
    }

    /******************
     * xaxis builders *
     ******************/

    /**
     * Builder function which creates an xnamed_axis.
     * @param name the dimension name.
     * @param axis the xaxis that you want to store in the xnamed_axis.
     * @tparam K the dimension name type.
     * @tparam A the axis type.
     * @return the created xnamed_axis.
     */
    template <class K, class A>
    inline auto named_axis(const K& name, A&& axis)
    {
        static_assert(is_axis<std::decay_t<A>>::value, "axis must be an axis type");

        using key_type = typename std::decay_t<A>::key_type;
        using mapped_type = typename std::decay_t<A>::mapped_type;

        return xnamed_axis<K, mapped_type, hash_map_tag, XFRAME_DEFAULT_LABEL_LIST, key_type>(name, axis);
    }

    template <class A>
    inline auto named_axis(const char* name, A&& axis)
    {
        static_assert(is_axis<std::decay_t<A>>::value, "axis must be an axis type");

        using key_type = typename std::decay_t<A>::key_type;
        using mapped_type = typename std::decay_t<A>::mapped_type;

        return xnamed_axis<const char*, mapped_type, hash_map_tag, XFRAME_DEFAULT_LABEL_LIST, key_type>(name, axis);
    }

    template <class LB, class K, class T, class MT = hash_map_tag, class L = XFRAME_DEFAULT_LABEL_LIST>
    auto get_labels(const xnamed_axis<K, T, MT, L, LB>& n_axis) -> const typename xaxis<LB, T, MT>::label_list&
    {
        return get_labels<LB>(n_axis.axis());
    }
}

#endif
