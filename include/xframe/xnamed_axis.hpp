/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay Wolf Vollprecht and    *
* Martin Renou                                                             *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XNAMED_AXIS_HPP
#define XFRAME_XNAMED_AXIS_HPP

#include "xaxis_base.hpp"
#include "xaxis.hpp"
#include "xaxis_default.hpp"
#include "xaxis_variant.hpp"

namespace xf
{

    /***************
     * xnamed_axis *
     ***************/

    template <class K, class T, class MT = hash_map_tag, class L = DEFAULT_LABEL_LIST>
    class xnamed_axis
    {
    public:

        using name_type = K;
        using axis_variant_type = xaxis_variant<L, T, MT>;

        template <class A>
        xnamed_axis(const name_type& name, A&& axis);
        template <class A>
        xnamed_axis(name_type&& name, A&& axis);

        const name_type& name() const & noexcept;

        const axis_variant_type& axis() const & noexcept;

    private:

        name_type m_name;
        axis_variant_type m_axis;
    };

    /******************************
     * xnamed_axis implementation *
     ******************************/

    template <class K, class T, class MT, class L>
    template <class A>
    inline xnamed_axis<K, T, MT, L>::xnamed_axis(const name_type& name, A&& axis)
        : m_name(name), m_axis(std::forward<A>(axis))
    {
    }

    template <class K, class T, class MT, class L>
    template <class A>
    inline xnamed_axis<K, T, MT, L>::xnamed_axis(name_type&& name, A&& axis)
        : m_name(std::move(name)), m_axis(std::forward<A>(axis))
    {
    }

    template <class K, class T, class MT, class L>
    inline auto xnamed_axis<K, T, MT, L>::name() const & noexcept -> const name_type&
    {
        return m_name;
    }

    template <class K, class T, class MT, class L>
    inline auto xnamed_axis<K, T, MT, L>::axis() const & noexcept -> const axis_variant_type&
    {
        return m_axis;
    }

    template <class K, class A>
    inline auto named_axis(const K& name, A&& axis)
    {
        static_assert(is_axis<std::decay_t<A>>::value, "axis must be an axis type");

        using T = typename A::mapped_type;

        return xnamed_axis<K, T>(name, axis);
    }

    template <class A>
    inline auto named_axis(const char* name, A&& axis)
    {
        static_assert(is_axis<std::decay_t<A>>::value, "axis must be an axis type");

        using T = typename A::mapped_type;

        return xnamed_axis<const char*, T>(name, axis);
    }
}

#endif
