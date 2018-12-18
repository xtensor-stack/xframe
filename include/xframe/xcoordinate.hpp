/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XCOORDINATE_HPP
#define XFRAME_XCOORDINATE_HPP

#include "xtensor/xutils.hpp"
#include "xframe_config.hpp"
#include "xcoordinate_view.hpp"
#include "xnamed_axis.hpp"

namespace xf
{
    namespace join
    {
        enum class join_id
        {
            outer_id,
            inner_id
        };

        struct outer
        {
            static constexpr join_id id() { return join_id::outer_id; }
        };

        struct inner
        {
            static constexpr join_id id() { return join_id::inner_id; }
        };
    }

    class xfull_coordinate {};

    struct xtrivial_broadcast
    {
        xtrivial_broadcast() = default;
        xtrivial_broadcast(bool xtensor_trivial, bool xframe_trivial);
        bool m_xtensor_trivial;
        bool m_xframe_trivial;
    };

    xtrivial_broadcast operator&&(const xtrivial_broadcast& lhs, const xtrivial_broadcast& rhs) noexcept;

    template <class K, class S, class MT = hash_map_tag, class L = DEFAULT_LABEL_LIST>
    class xcoordinate : public xcoordinate_base<K, xaxis_variant<L, S, MT>>
    {
    public:

        using self_type = xcoordinate<K, S, MT, L>;
        using base_type = xcoordinate_base<K, xaxis_variant<L, S, MT>>;
        using label_list = L;
        using axis_type = typename base_type::axis_type;
        using map_type = typename base_type::map_type;
        using key_type = typename base_type::key_type;
        using mapped_type = typename base_type::mapped_type;
        using index_type = typename base_type::index_type;
        using value_type = typename base_type::value_type;
        using reference = typename base_type::reference;
        using const_reference = typename base_type::const_reference;
        using pointer = typename base_type::pointer;
        using const_pointer = typename base_type::const_pointer;
        using size_type = typename base_type::size_type;
        using difference_type = typename base_type::difference_type;
        using iterator = typename base_type::iterator;
        using const_iterator = typename base_type::const_iterator;
        using key_iterator = typename base_type::key_iterator;

        explicit xcoordinate(const map_type& axes);
        explicit xcoordinate(map_type&& axes);
        xcoordinate(std::initializer_list<value_type> init);
        template <class... K1, class... LT>
        explicit xcoordinate(xnamed_axis<K1, S, MT, L, LT>... axes);

        void clear();

        template <class Join, class... Args>
        xtrivial_broadcast broadcast(const Args&... coordinates);

    private:

        using coordinate_view_type = xcoordinate_view < K, S, MT, L>;

        template <class Join, class... Args>
        xtrivial_broadcast broadcast_impl(const self_type& c, const Args&... coordinates);
        template <class Join, class... Args>
        xtrivial_broadcast broadcast_impl(const coordinate_view_type& c, const Args&... coordinates);
        template <class Join, class... Args>
        xtrivial_broadcast broadcast_impl(const xfull_coordinate& c, const Args&... coordinates);
        template <class Join>
        xtrivial_broadcast broadcast_impl();

        template <class Join, class... Args>
        xtrivial_broadcast broadcast_empty(const self_type& c, const Args&... coordinates);
        template <class Join, class... Args>
        xtrivial_broadcast broadcast_empty(const coordinate_view_type& c, const Args&... coordinates);
        template <class Join, class... Args>
        xtrivial_broadcast broadcast_empty(const xfull_coordinate& c, const Args&... coordinates);
        template <class Join>
        xtrivial_broadcast broadcast_empty();
    };

    template <class K, class S = std::size_t, class MT = hash_map_tag, class L = DEFAULT_LABEL_LIST>
    xcoordinate<K, S, MT, L> coordinate(const std::map<K, xaxis_variant<L, S, MT>>& axes);

    template <class K, class S = std::size_t, class MT = hash_map_tag, class L = DEFAULT_LABEL_LIST>
    xcoordinate<K, S, MT, L> coordinate(std::map<K, xaxis_variant<L, S, MT>>&& axes);

    template <class K, class... K1, class S, class MT, class L, class LT, class... LT1>
    xcoordinate<K, S, MT, L> coordinate(xnamed_axis<K, S, MT, L, LT> axis, xnamed_axis<K1, S, MT, L, LT1>... axes);

    template <class Join, class K, class S, class MT, class L, class... Args>
    xtrivial_broadcast broadcast_coordinates(xcoordinate<K, S, MT, L>& output, const Args&... coordinates);

    /****************************
     * coordinate metafunctions *
     ****************************/

    template <class C>
    class xcoordinate_chain;

    namespace detail
    {
        template <class T>
        struct is_coordinate_impl : std::false_type
        {
        };

        template <class K, class S, class MT, class L>
        struct is_coordinate_impl<xcoordinate<K, S, MT, L>> : std::true_type
        {
        };

        template <class K, class S, class MT, class L>
        struct is_coordinate_impl<xcoordinate_view<K, S, MT, L>> : std::true_type
        {
        };

        template <class C>
        struct is_coordinate_impl<xcoordinate_chain<C>> : std::true_type
        {
        };

        template <class T>
        struct is_coordinate_map_impl : std::false_type
        {
        };

        template <class K, class L, class S, class MT>
        struct is_coordinate_map_impl<std::map<K, xaxis_variant<L, S, MT>>>
            : std::true_type
        {
        };

        template <class T>
        struct get_coordinate_type_impl
        {
            using type = T;
        };

        template <class K, class L, class S, class MT>
        struct get_coordinate_type_impl<std::map<K, xaxis_variant<L, S, MT>>>
        {
            using type = xcoordinate<K, S, MT, L>;
        };
    }

    template <class T>
    struct is_coordinate : detail::is_coordinate_impl<std::decay_t<T>>
    {
    };

    template <class T>
    struct is_coordinate_map : detail::is_coordinate_map_impl<std::decay_t<T>>
    {
    };

    template <class T>
    struct enable_coordinate_map
        : std::enable_if<is_coordinate_map<T>::value, xt::void_t<>>
    {
    };

    template <class T>
    using enable_coordinate_map_t = typename enable_coordinate_map<T>::type;

    template <class T>
    struct get_coordinate_type : detail::get_coordinate_type_impl<std::decay_t<T>>
    {
    };

    template <class T>
    using get_coordinate_type_t = typename get_coordinate_type<T>::type;

    /******************************
     * xcoordinate implementation *
     ******************************/

    inline xtrivial_broadcast operator&&(const xtrivial_broadcast& lhs, const xtrivial_broadcast& rhs) noexcept
    {
        return xtrivial_broadcast(lhs.m_xtensor_trivial && rhs.m_xtensor_trivial,
                                  lhs.m_xframe_trivial && rhs.m_xframe_trivial);
    }

    inline xtrivial_broadcast::xtrivial_broadcast(bool xtensor_trivial, bool xframe_trivial)
        : m_xtensor_trivial(xtensor_trivial), m_xframe_trivial(xframe_trivial)
    {
    }

    template <class K, class S, class MT, class L>
    inline xcoordinate<K, S, MT, L>::xcoordinate(const map_type& axes)
        : base_type(axes)
    {
    }

    template <class K, class S, class MT, class L>
    inline xcoordinate<K, S, MT, L>::xcoordinate(map_type&& axes)
        : base_type(std::move(axes))
    {
    }

    template <class K, class S, class MT, class L>
    inline xcoordinate<K, S, MT, L>::xcoordinate(std::initializer_list<value_type> init)
        : base_type(init)
    {
    }

    template <class K, class S, class MT, class L>
    template <class... K1, class... LT>
    inline xcoordinate<K, S, MT, L>::xcoordinate(xnamed_axis<K1, S, MT, L, LT>... axes)
        : base_type({ value_type(std::move(axes).name(), std::move(axes).axis())... })
    {
    }

    template <class K, class S, class MT, class L>
    inline void xcoordinate<K, S, MT, L>::clear()
    {
        this->coordinate().clear();
    }

    template <class K, class S, class MT, class L>
    template <class Join, class... Args>
    inline xtrivial_broadcast xcoordinate<K, S, MT, L>::broadcast(const Args&... coordinates)
    {
        return this->empty() ? broadcast_empty<Join>(coordinates...) : broadcast_impl<Join>(coordinates...);
    }

    namespace detail
    {
        template <class Join>
        struct axis_broadcast;

        template <>
        struct axis_broadcast<join::outer>
        {
            template <class A>
            static bool apply(A& output, const A& input)
            {
                return output.merge(input);
            }
        };

        template <>
        struct axis_broadcast<join::inner>
        {
            template <class A>
            static bool apply(A& output, const A& input)
            {
                return output.intersect(input);
            }
        };
    }

    template <class K, class S, class MT, class L>
    template <class Join, class... Args>
    inline xtrivial_broadcast xcoordinate<K, S, MT, L>::broadcast_impl(const self_type& c, const Args&... coordinates)
    {
        auto res = broadcast_impl<Join>(coordinates...);
        XFRAME_TRACE_BROADCAST_COORDINATES(*this, c);
        for(auto iter = c.begin(); iter != c.end(); ++iter)
        {
            auto inserted = this->coordinate().insert(*iter);
            if(inserted.second)
            {
                res.m_xtensor_trivial = false;
            }
            else
            {
                auto& axis = inserted.first->second;
                res.m_xframe_trivial &= detail::axis_broadcast<Join>::apply(axis, iter->second);
            }
        }
        XFRAME_TRACE_COORDINATES_RESULT(*this, res);
        return res;
    }

    template <class K, class S, class MT, class L>
    template <class Join, class... Args>
    inline xtrivial_broadcast xcoordinate<K, S, MT, L>::broadcast_impl(const coordinate_view_type& c, const Args&... coordinates)
    {
        auto res = broadcast_impl<Join>(coordinates...);
        XFRAME_TRACE_BROADCAST_COORDINATES(*this, c);
        for (auto iter = c.begin(); iter != c.end(); ++iter)
        {
            mapped_type axis = mapped_type(iter->second);
            auto it = this->coordinate().find(iter->first);
            if (it == this->coordinate().end())
            {
                this->coordinate().insert(std::make_pair(iter->first, std::move(axis)));
                res.m_xtensor_trivial = false;
            }
            else
            {
                res.m_xframe_trivial &= detail::axis_broadcast<Join>::apply(it->second, axis);
            }
        }
        XFRAME_TRACE_COORDINATES_RESULT(*this, res);
        return res;
    }

    template <class K, class S, class MT, class L>
    template <class Join, class... Args>
    inline xtrivial_broadcast xcoordinate<K, S, MT, L>::broadcast_impl(const xfull_coordinate& /*c*/, const Args&... coordinates)
    {
        return broadcast_impl<Join>(coordinates...);
    }

    template <class K, class S, class MT, class L>
    template <class Join>
    inline xtrivial_broadcast xcoordinate<K, S, MT, L>::broadcast_impl()
    {
        return xtrivial_broadcast(true, true);
    }

    template <class K, class S, class MT, class L>
    template <class Join, class... Args>
    inline xtrivial_broadcast xcoordinate<K, S, MT, L>::broadcast_empty(const self_type& c, const Args&... coordinates)
    {
        map_type& m = this->coordinate();
        for (auto iter = c.data().cbegin(); iter != c.data().cend(); ++iter)
        {
            m.insert(std::make_pair(iter->first, mapped_type(iter->second.as_xaxis())));
        }
        return broadcast_impl<Join>(coordinates...);
    }

    template <class K, class S, class MT, class L>
    template <class Join, class... Args>
    inline xtrivial_broadcast xcoordinate<K, S, MT, L>::broadcast_empty(const coordinate_view_type& c, const Args&... coordinates)
    {
        map_type& m = this->coordinate();
        for (auto iter = c.data().cbegin(); iter != c.data().cend(); ++iter)
        {
            m.insert(std::make_pair(iter->first, mapped_type(iter->second.as_xaxis())));
        }
        return broadcast_impl<Join>(coordinates...);
    }

    template <class K, class S, class MT, class L>
    template <class Join, class... Args>
    inline xtrivial_broadcast xcoordinate<K, S, MT, L>::broadcast_empty(const xfull_coordinate& /*c*/, const Args&... coordinates)
    {
        return broadcast_empty<Join>(coordinates...);
    }

    template <class K, class S, class MT, class L>
    template <class Join>
    inline xtrivial_broadcast xcoordinate<K, S, MT, L>::broadcast_empty()
    {
        return broadcast_impl<Join>();
    }

    template <class K, class S, class MT, class L>
    xcoordinate<K, S, MT, L> coordinate(const std::map<K, xaxis_variant<L, S, MT>>& axes)
    {
        return xcoordinate<K, S, MT, L>(axes);
    }

    template <class K, class S, class MT, class L>
    xcoordinate<K, S, MT, L> coordinate(std::map<K, xaxis_variant<L, S, MT>>&& axes)
    {
        return xcoordinate<K, S, MT, L>(std::move(axes));
    }

    template <class K, class... K1, class S, class MT, class L, class LT, class... LT1>
    xcoordinate<K, S, MT, L> coordinate(xnamed_axis<K, S, MT, L, LT> axis, xnamed_axis<K1, S, MT, L, LT1>... axes)
    {
        return xcoordinate<K, S, MT, L>(axis, axes...);
    };

    template <class Join, class K, class S, class MT, class L, class... Args>
    inline xtrivial_broadcast broadcast_coordinates(xcoordinate<K, S, MT, L>& output, const Args&... coordinates)
    {
        return output.template broadcast<Join>(coordinates...);
    }
}

#endif
