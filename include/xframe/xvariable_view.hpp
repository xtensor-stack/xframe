/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVARIABLE_VIEW_HPP
#define XFRAME_XVARIABLE_VIEW_HPP

#include "xvariable.hpp"
#include "xcoordinate_system.hpp"
#include "xcoordinate_view.hpp"

namespace xf
{

    /*******************
     * xvariable_view  *
     *******************/

    template <class CT>
    class xvariable_view;

    template <class CT>
    struct xvariable_inner_types<xvariable_view<CT>>
    {
        using xexpression_type = std::decay_t<CT>;
        using coordinate_type = xcoordinate_view_type_t<typename xexpression_type::coordinate_type>;
        using coordinate_closure_type = coordinate_type;
        using dimension_type = typename xexpression_type::dimension_type;
        using key_type = typename coordinate_type::key_type;
        using size_type = typename coordinate_type::size_type;
    };

}

namespace xt
{
    template <class CT>
    struct xcontainer_inner_types<xf::xvariable_view<CT>>
        : xf::xvariable_inner_types<xf::xvariable_view<CT>>
    {
        using base_type = xf::xvariable_inner_types<xf::xvariable_view<CT>>;
        using xexpression_type = typename base_type::xexpression_type;
        using optional_type = typename xexpression_type::value_type;
        using temporary_coordinate_type = xf::xcoordinate<typename base_type::key_type, typename base_type::size_type>;
        using temporary_data_type = xoptional_assembly<xarray<typename optional_type::value_type>, xarray<bool>>;
        using temporary_type = xf::xvariable<temporary_coordinate_type, temporary_data_type>;
    };
}

namespace xf
{

    template <class CT>
    class xvariable_view : public xt::xview_semantic<xvariable_view<CT>>,
                           private xcoordinate_system<xvariable_view<CT>>
    {
    public:

        using self_type = xvariable_view<CT>;
        using semantic_base = xt::xview_semantic<self_type>;
        using coordinate_base = xcoordinate_system<xvariable_view<CT>>;
        using inner_types = xt::xcontainer_inner_types<self_type>;
        using xexpression_type = typename inner_types::xexpression_type;
        using data_type = typename xexpression_type::data_type;

        static constexpr bool is_const = std::is_const<std::remove_reference_t<CT>>::value;
        using value_type = typename xexpression_type::value_type;
        using reference = std::conditional_t<is_const,
                                             typename xexpression_type::const_reference,
                                             typename xexpression_type::reference>;
        using const_reference = typename xexpression_type::const_reference;
        using pointer = std::conditional_t<is_const,
                                           typename xexpression_type::const_pointer,
                                           typename xexpression_type::pointer>;
        using const_pointer = typename xexpression_type::const_pointer;
        using size_type = typename xexpression_type::size_type;
        using difference_type = typename xexpression_type::difference_type;

        using coordinate_type = typename coordinate_base::coordinate_type;
        using dimension_type = typename coordinate_base::dimension_type;
        using dimension_list = typename dimension_type::label_list;
        using squeeze_map = std::map<typename dimension_type::mapped_type, typename coordinate_type::index_type>;
        using temporary_type = typename inner_types::temporary_type;

        using expression_tag = xvariable_expression_tag;

        template <std::size_t N = dynamic()>
        using selector_traits = xselector_traits<coordinate_type, dimension_type, N>;
        template <std::size_t N = dynamic()>
        using selector_type = typename selector_traits<N>::selector_type;
        template <std::size_t N = dynamic()>
        using selector_map_type = typename selector_traits<N>::selector_map_type;
        template <std::size_t N = dynamic()>
        using iselector_type = typename selector_traits<N>::iselector_type;
        template <std::size_t N = dynamic()>
        using iselector_map_type = typename selector_traits<N>::iselector_map_type;
        template <std::size_t N = dynamic()>
        using locator_type = typename selector_traits<N>::locator_type;
        template <std::size_t N = dynamic()>
        using locator_sequence_type = typename selector_traits<N>::locator_sequence_type;

        static const_reference missing();

        template <class E>
        xvariable_view(E&& e, coordinate_type&& coord, dimension_type&& dim, squeeze_map&& squeeze);

        xvariable_view(const xvariable_view&) = default;
        xvariable_view& operator=(const xvariable_view&);

        template <class E>
        xvariable_view& operator=(const xt::xexpression<E>& e);

        using coordinate_base::size;
        using coordinate_base::dimension;
        using coordinate_base::dimension_labels;
        using coordinate_base::coordinates;
        using coordinate_base::dimension_mapping;
        using coordinate_base::broadcast_coordinates;
        using coordinate_base::broadcast_dimensions;

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
        reference locate_element(const locator_sequence_type<N>& locator);

        template <std::size_t N = dynamic()>
        const_reference locate_element(const locator_sequence_type<N>& locator) const;

        template <std::size_t N = dynamic()>
        reference locate_element(locator_sequence_type<N>&& locator);

        template <std::size_t N = dynamic()>
        const_reference locate_element(locator_sequence_type<N>&& locator) const;

        template <std::size_t N = dynamic()>
        reference select(const selector_map_type<N>& selector);

        template <class Join = DEFAULT_JOIN, std::size_t N = std::numeric_limits<size_type>::max()>
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

    private:

        using index_type = std::vector<size_type>;

        template <std::size_t... I, class... Args>
        reference access_impl(std::index_sequence<I...>, Args... args);

        template <std::size_t... I, class... Args>
        const_reference access_impl(std::index_sequence<I...>, Args... args) const;

        template <class... Args>
        index_type build_accessor(Args&&... args) const;

        template <std::size_t I, class T, class... Args>
        void fill_accessor(index_type& accessor, T idx, Args... args) const;

        template <std::size_t I>
        void fill_accessor(index_type& accessor) const;

        template <class It>
        index_type build_element_accessor(It first, It last) const;

        template <std::size_t... I, class... Args>
        reference locate_impl(std::index_sequence<I...>, Args&&... args);

        template <std::size_t... I, class... Args>
        const_reference locate_impl(std::index_sequence<I...>, Args&&... args) const;

        template <class... Args>
        index_type build_locator(Args&&... args) const;

        template <std::size_t I, class T, class... Args>
        void fill_locator(index_type& locator, T idx, Args&&... args) const;

        template <std::size_t I>
        void fill_locator(index_type& locator) const;

        template <std::size_t N>
        index_type build_element_locator(locator_sequence_type<N>&& locator) const;

        template <class S>
        reference select_impl(const S& selector);

        template <class S>
        const_reference select_impl(const S& selector) const;

        template <class S>
        const_reference select_outer(const S& selector) const;

        template <class Join, class S>
        const_reference select_join(const S& selector) const;

        template <class Idx>
        void fill_squeeze(Idx& index) const;

        template <std::size_t N>
        void adapt_iselector(iselector_map_type<N>& selector) const;

        void assign_temporary_impl(temporary_type&& tmp);

        CT m_e;
        squeeze_map m_squeeze;

        friend class xt::xview_semantic<xvariable_view<CT>>;
    };

    /***************************
     * xvariable_view builders *
     ***************************/

    template <class E, class... S>
    auto ilocate(E&& e, S&&... slices);

    template <class L = DEFAULT_LABEL_LIST, class E, class... S>
    auto locate(E&& e, S&&... slices);

    template <class E, class L = DEFAULT_LABEL_LIST>
    auto select(E&& e, std::map<typename std::decay_t<E>::key_type, xaxis_slice<L>>&& slices);

    template <class E, class T = typename std::decay_t<E>::size_type>
    auto iselect(E&& e, std::map<typename std::decay_t<E>::key_type, xaxis_extended_islice<T>> && slices);

    /*********************************
     * xvariable_view implementation *
     *********************************/

    template <class CT>
    template <class E>
    inline xvariable_view<CT>::xvariable_view(E&& e, coordinate_type&& coord, dimension_type&& dim, squeeze_map&& squeeze)
        : coordinate_base(std::move(coord), std::move(dim)),
          m_e(std::forward<E>(e)),
          m_squeeze(std::move(squeeze))
    {
    }

    template <class CT>
    inline auto xvariable_view<CT>::missing() -> const_reference
    {
        return detail::static_missing<const_reference>();
    }

    template <class CT>
    inline xvariable_view<CT>& xvariable_view<CT>::operator=(const xvariable_view& rhs)
    {
        temporary_type tmp(rhs);
        return this->assign_temporary(std::move(tmp));
    }

    template <class CT>
    template <class E>
    inline xvariable_view<CT>& xvariable_view<CT>::operator=(const xt::xexpression<E>& e)
    {
        using root_semantic = typename semantic_base::base_type;
        return root_semantic::operator=(e);
    }

    template <class CT>
    inline auto xvariable_view<CT>::data() noexcept -> data_type&
    {
        return m_e.data();
    }

    template <class CT>
    inline auto xvariable_view<CT>::data() const noexcept -> const data_type&
    {
        return m_e.data();
    }

    template <class CT>
    template <class... Args>
    inline auto xvariable_view<CT>::operator()(Args... args) ->reference
    {
        if (m_squeeze.empty())
        {
            return access_impl(std::make_index_sequence<sizeof...(Args)>(), args...);
        }
        else
        {
            auto idx = build_accessor(std::forward<Args>(args)...);
            return m_e.element(idx.cbegin(), idx.cend());
        }
    }

    template <class CT>
    template <class... Args>
    inline auto xvariable_view<CT>::operator()(Args... args) const -> const_reference
    {
        if (m_squeeze.empty())
        {
            return access_impl(std::make_index_sequence<sizeof...(Args)>(), args...);
        }
        else
        {
            auto idx = build_accessor(std::forward<Args>(args)...);
            return m_e.element(idx.cbegin(), idx.cend());
        }
    }

    template <class CT>
    template <class It>
    inline auto xvariable_view<CT>::element(It first, It last) -> reference
    {
        auto idx = build_element_accessor(first, last);
        return m_e.element(idx.cbegin(), idx.cend());
    }

    template <class CT>
    template <class It>
    inline auto xvariable_view<CT>::element(It first, It last) const -> const_reference
    {
        auto idx = build_element_accessor(first, last);
        return m_e.element(idx.cbegin(), idx.cend());
    }

    template <class CT>
    template <class... Args>
    inline auto xvariable_view<CT>::locate(Args&&... args) -> reference
    {
        if (m_squeeze.empty())
        {
            return locate_impl(std::make_index_sequence<sizeof...(Args)>(), std::forward<Args>(args)...);
        }
        else
        {
            auto idx = build_locator(std::forward<Args>(args)...);
            return m_e.element(idx.cbegin(), idx.cend());
        }
    }

    template <class CT>
    template <class... Args>
    inline auto xvariable_view<CT>::locate(Args&&... args) const -> const_reference
    {
        if (m_squeeze.empty())
        {
            return locate_impl(std::make_index_sequence<sizeof...(Args)>(), std::forward<Args>(args)...);
        }
        else
        {
            auto idx = build_locator(std::forward<Args>(args)...);
            return m_e.element(idx.cbegin(), idx.cend());
        }
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::locate_element(const locator_sequence_type<N>& locator) -> reference
    {
        locator_sequence_type<N> tmp(locator);
        auto idx = build_element_locator<N>(std::move(tmp));
        return m_e.element(idx.cbegin(), idx.cend());
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::locate_element(const locator_sequence_type<N>& locator) const -> const_reference
    {
        locator_sequence_type<N> tmp(locator);
        auto idx = build_element_locator<N>(std::move(locator));
        return m_e.element(idx.cbegin(), idx.cend());
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::locate_element(locator_sequence_type<N>&& locator) -> reference
    {
        auto idx = build_element_locator<N>(std::move(locator));
        return m_e.element(idx.cbegin(), idx.cend());
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::locate_element(locator_sequence_type<N>&& locator) const -> const_reference
    {
        auto idx = build_element_locator<N>(std::move(locator));
        return m_e.element(idx.cbegin(), idx.cend());
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::select(const selector_map_type<N>& selector) -> reference
    {
        return select_impl(selector_type<N>(selector));
    }

    template <class CT>
    template <class Join, std::size_t N>
    inline auto xvariable_view<CT>::select(const selector_map_type<N>& selector) const -> const_reference
    {
        return select_join<Join>(selector_type<N>(selector));
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::select(selector_map_type<N>&& selector) -> reference
    {
        return select_impl(selector_type<N>(std::move(selector)));
    }

    template <class CT>
    template <class Join, std::size_t N>
    inline auto xvariable_view<CT>::select(selector_map_type<N>&& selector) const -> const_reference
    {
        return select_join<Join>(selector_type<N>(std::move(selector)));
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::iselect(const iselector_map_type<N>& selector) -> reference
    {
        return iselect(iselector_map_type<N>(selector));
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::iselect(const iselector_map_type<N>& selector) const -> const_reference
    {
        return iselect(iselector_map_type<N>(selector));
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::iselect(iselector_map_type<N>&& selector) -> reference
    {
        iselector_map_type<N> tmp_selector(std::move(selector));
        adapt_iselector<N>(tmp_selector);
        return select_impl(iselector_type<N>(std::move(tmp_selector)));
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::iselect(iselector_map_type<N>&& selector) const -> const_reference
    {
        iselector_map_type<N> tmp_selector(std::move(selector));
        adapt_iselector<N>(tmp_selector);
        return select_impl(iselector_type<N>(std::move(tmp_selector)));
    }

    template <class CT>
    template <std::size_t... I, class... Args>
    inline auto xvariable_view<CT>::access_impl(std::index_sequence<I...>, Args... args) -> reference
    {
        return m_e(coordinates()[dimension_labels()[I]].index(args)...);
    }

    template <class CT>
    template <std::size_t... I, class... Args>
    inline auto xvariable_view<CT>::access_impl(std::index_sequence<I...>, Args... args) const -> const_reference
    {
        return m_e(coordinates()[dimension_labels()[I]].index(args)...);
    }

    template <class CT>
    template <class... Args>
    inline auto xvariable_view<CT>::build_accessor(Args&&... args) const -> index_type
    {
        index_type accessor(m_e.dimension());
        fill_accessor<0>(accessor, std::forward<Args>(args)...);
        fill_squeeze(accessor);
        return accessor;
    }

    template <class CT>
    template <std::size_t I, class T, class... Args>
    inline void xvariable_view<CT>::fill_accessor(index_type& accessor, T idx, Args... args) const
    {
        std::size_t new_index = m_e.dimension_mapping()[dimension_labels()[I]];
        accessor[new_index] = coordinates()[dimension_labels()[I]].index(idx);
        fill_accessor<I + 1>(accessor, std::forward<Args>(args)...);
    }

    template <class CT>
    template <std::size_t I>
    inline void xvariable_view<CT>::fill_accessor(index_type& /*accessor*/) const
    {
    }

    template <class CT>
    template <class It>
    inline auto xvariable_view<CT>::build_element_accessor(It first, It last) const -> index_type
    {
        index_type res(m_e.dimension());
        size_type current_index = 0;
        size_type i = 0;
        while (first != last)
        {
            auto iter = m_squeeze.find(current_index);
            if (iter != m_squeeze.end())
            {
                res[current_index++] = iter->second;
            }
            else
            {
                res[current_index++] = coordinates()[dimension_labels()[i++]].index(*first++);
            }
        }
        while (current_index != res.size())
        {
            res[current_index] = m_squeeze.find(current_index)->second;
            ++current_index;
        }
        return res;
    }

    template <class CT>
    template <std::size_t... I, class... Args>
    inline auto xvariable_view<CT>::locate_impl(std::index_sequence<I...>, Args&&... args) -> reference
    {
        return m_e(coordinates()[dimension_mapping().labels()[I]][args]...);
    }

    template <class CT>
    template <std::size_t... I, class... Args>
    inline auto xvariable_view<CT>::locate_impl(std::index_sequence<I...>, Args&&... args) const -> const_reference
    {
        m_e(coordinates()[dimension_mapping().labels()[I]][args]...);
    }

    template <class CT>
    template <class... Args>
    inline auto xvariable_view<CT>::build_locator(Args&&... args) const -> index_type
    {
        index_type locator(m_e.dimension());
        fill_locator<0>(locator, std::forward<Args>(args)...);
        fill_squeeze(locator);
        return locator;
    }

    template <class CT>
    template <std::size_t I, class T, class... Args>
    inline void xvariable_view<CT>::fill_locator(index_type& locator, T idx, Args&&... args) const
    {
        std::size_t new_index = m_e.dimension_mapping()[dimension_labels()[I]];
        locator[new_index] = coordinates()[dimension_labels()[I]][idx];
        fill_locator<I + 1>(locator, std::forward<Args>(args)...);
    }

    template <class CT>
    template <std::size_t I>
    inline void xvariable_view<CT>::fill_locator(index_type&) const
    {
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::build_element_locator(locator_sequence_type<N>&& locator) const -> index_type
    {
        if (m_squeeze.empty())
        {
            locator_type<N> loc(std::move(locator));
            return loc.get_index(coordinates(), dimension_mapping());
        }
        else
        {
            index_type res(m_e.dimension());
            size_type current_index = size_type(0);
            size_type i = size_type(0);
            const auto& coord = m_e.coordinates();
            const auto& dims = m_e.dimension_mapping();
            while (i != locator.size())
            {
                auto iter = m_squeeze.find(current_index);
                if (iter != m_squeeze.cend())
                {
                    res[current_index] = iter->second;
                }
                else
                {
                    res[current_index] = coord[dims.labels()[current_index]][locator[i]];
                    ++i;
                }
                ++current_index;
            }
            while (current_index != res.size())
            {
                res[current_index] = m_squeeze.find(current_index)->second;
                ++current_index;
            }
            return res;
        }
    }

    template <class CT>
    template <class S>
    inline auto xvariable_view<CT>::select_impl(const S& selector) -> reference
    {
        typename S::index_type idx = selector.get_index(coordinates(), m_e.dimension_mapping());
        fill_squeeze(idx);
        return m_e.element(idx.cbegin(), idx.cend());
    }

    template <class CT>
    template <class S>
    inline auto xvariable_view<CT>::select_impl(const S& selector) const -> const_reference
    {
        typename S::index_type idx = selector.get_index(coordinates(), m_e.dimension_mapping());
        fill_squeeze(idx);
        return m_e.element(idx.cbegin(), idx.cend());
    }

    template <class CT>
    template <class S>
    inline auto xvariable_view<CT>::select_outer(const S& selector) const -> const_reference
    {
        typename S::outer_index_type idx = selector.get_outer_index(coordinates(), m_e.dimension_mapping());
        if (idx.second)
        {
            fill_squeeze(idx.first);
            return m_e.element(idx.first.cbegin(), idx.first.cend());
        }
        else
        {
            return missing();
        }
    }

    template <class CT>
    template <class Join, class S>
    inline auto xvariable_view<CT>::select_join(const S& selector) const -> const_reference
    {
        return xtl::mpl::static_if<Join::id() == join::inner::id()>([&](auto self)
        {
            return self(*this).select_impl(selector);
        }, /*else*/ [&](auto self)
        {
            return self(*this).select_outer(selector);
        });
    }

    template <class CT>
    template <class Idx>
    inline void xvariable_view<CT>::fill_squeeze(Idx& index) const
    {
        for (const auto& sq : m_squeeze)
        {
            index[sq.first] = sq.second;
        }
    }

    template <class CT>
    template <std::size_t N>
    inline void xvariable_view<CT>::adapt_iselector(iselector_map_type<N>& selector) const
    {
        for (auto& sel : selector)
        {
            sel.second = coordinates()[sel.first].index(sel.second);
        }
    }

    template <class CT>
    inline void xvariable_view<CT>::assign_temporary_impl(temporary_type&& tmp)
    {
        // TODO: improve this with iterators when they are available
        const temporary_type& tmp2 = tmp;
        const auto& dim_label = dimension_labels();
        const auto& coords = coordinates();
        std::vector<size_type> index(dim_label.size(), size_type(0));
        selector_map_type<> selector(index.size());
        bool end = false;
        do
        {
            for (size_type i = 0; i < index.size(); ++i)
            {
                selector[i] = std::make_pair(dim_label[i], coords[dim_label[i]].label(index[i]));
            }
            this->select(selector) = tmp2.select(selector);
            end = xt::detail::increment_index(tmp2.data().shape(), index);
        } while (!end);
    }

    /******************************************
     * xvariable_view builders implementation *
     ******************************************/
    
    namespace detail
    {
        template <class E>
        struct view_params
        {
            using coordinate_type = typename std::decay_t<E>::coordinate_type;
            using dimension_type = typename std::decay_t<E>::dimension_type;
            using dimension_label_list = typename dimension_type::label_list;
            using view_type = xvariable_view<xtl::closure_type_t<E>>;
            using squeeze_map = typename view_type::squeeze_map;
            using coordinate_view_type = typename view_type::coordinate_type;
            using map_type = typename coordinate_view_type::map_type;
            using axis_type = typename coordinate_view_type::axis_type;

            map_type coord_map;
            squeeze_map sq_map;
            dimension_label_list dim_label_list;
        };

        template <std::size_t I, class V, class E>
        inline void fill_locate_view_params(V& /*param*/, E& /*e*/)
        {
        }

        template <std::size_t I, class V, class E, class SL, class... S>
        inline void fill_locate_view_params(V& param, E& e, SL&& sl, S&&... slices)
        {
            using axis_type = typename view_params<E>::axis_type;
            const auto& dim_label = e.dimension_labels()[I];
            const auto& axis = e.coordinates()[dim_label];
            if (auto* sq = sl.get_squeeze())
            {
                param.sq_map[I] = axis[*sq];
            }
            else
            {
                param.coord_map.emplace(dim_label, axis_type(axis, sl.build_islice(axis)));
                param.dim_label_list.push_back(dim_label);
            }
            fill_locate_view_params<I + 1>(param, e, std::forward<S>(slices)...);
        }

        template <std::size_t I, class V, class E>
        inline void fill_view_params(V& /*param*/, E& /*e*/)
        {
        }

        template <std::size_t I, class V, class E, class SL, class... S>
        inline void fill_view_params(V& param, E& e, SL&& sl, S&&... slices)
        {
            using axis_type = typename view_params<E>::axis_type;
            using size_type = typename std::decay_t<E>::size_type;

            const auto& dim_label = e.dimension_labels()[I];
            const auto& axis = e.coordinates()[dim_label];
            if (auto* sq = sl.get_squeeze())
            {
                param.sq_map[I] = *sq;
            }
            else if (auto* sq = sl.get_all())
            {
                param.coord_map.emplace(dim_label, axis_type(axis, xt::xall<size_type>(axis.size())));
                param.dim_label_list.push_back(dim_label);
            }
            else
            {
                param.coord_map.emplace(dim_label, axis_type(axis, *(sl.get_slice())));
                param.dim_label_list.push_back(dim_label);
            }
            fill_view_params<I + 1>(param, e, std::forward<S>(slices)...);
        }
    }

    template <class E, class... S>
    inline auto ilocate(E&& e, S&&... slices)
    {
        using view_param_type = detail::view_params<E>;
        using coordinate_view_type = typename view_param_type::coordinate_view_type;
        using dimension_type = typename view_param_type::dimension_type;
        using view_type = typename view_param_type::view_type;

        view_param_type params;
        detail::fill_view_params<0>(params, e, xaxis_extended_islice<typename std::decay_t<E>::size_type>(std::forward<S>(slices))...);

        coordinate_view_type coordinate_view(std::move(params.coord_map));
        dimension_type view_dimension(std::move(params.dim_label_list));

        return view_type(std::forward<E>(e),
                         std::move(coordinate_view),
                         std::move(view_dimension),
                         std::move(params.sq_map));
    }

    template <class L, class E, class... S>
    inline auto locate(E&& e, S&&... slices)
    {
        using view_param_type = detail::view_params<E>;
        using coordinate_view_type = typename view_param_type::coordinate_view_type;
        using dimension_type = typename view_param_type::dimension_type;
        using view_type = typename view_param_type::view_type;

        view_param_type params;
        detail::fill_locate_view_params<0>(params, e, xaxis_slice<L>(std::forward<S>(slices))...);

        coordinate_view_type coordinate_view(std::move(params.coord_map));
        dimension_type view_dimension(std::move(params.dim_label_list));

        return view_type(std::forward<E>(e),
                         std::move(coordinate_view),
                         std::move(view_dimension),
                         std::move(params.sq_map));
    }

    template <class E, class L>
    inline auto select(E&& e, std::map<typename std::decay_t<E>::key_type, xaxis_slice<L>>&& slices)
    {
        using coordinate_type = typename std::decay_t<E>::coordinate_type;
        using dimension_type = typename std::decay_t<E>::dimension_type;
        using dimension_label_list = typename dimension_type::label_list;
        using view_type = xvariable_view<xtl::closure_type_t<E>>;
        using squeeze_map = typename view_type::squeeze_map;
        using coordinate_view_type = typename view_type::coordinate_type;
        using map_type = typename coordinate_view_type::map_type;
        using axis_type = typename coordinate_view_type::axis_type;
        using size_type = typename std::decay_t<E>::size_type;

        const coordinate_type& underlying_coords = e.coordinates();
        map_type coord_map;
        squeeze_map sq_map;
        dimension_label_list dim_label_list;

        for(const auto& dim_label: e.dimension_labels())
        {
            const auto& axis = underlying_coords[dim_label];
            auto slice_iter = slices.find(dim_label);
            if (slice_iter != slices.end())
            {
                if (auto* sq = (slice_iter->second).get_squeeze())
                {
                    sq_map[e.dimension_mapping()[dim_label]] = axis[*sq];
                }
                else
                {
                    coord_map.emplace(dim_label, axis_type(axis, (slice_iter->second).build_islice(axis)));
                    dim_label_list.push_back(dim_label);
                }
            }
            else
            {
                coord_map.emplace(dim_label, axis_type(axis, xt::xall<size_type>(axis.size())));
                dim_label_list.push_back(dim_label);
            }
        }

        coordinate_view_type coordinate_view(std::move(coord_map));
        dimension_type view_dimension(std::move(dim_label_list));

        return view_type(std::forward<E>(e),
                         std::move(coordinate_view),
                         std::move(view_dimension),
                         std::move(sq_map));
    }

    template <class E, class T>
    inline auto iselect(E&& e, std::map<typename std::decay_t<E>::key_type, xaxis_extended_islice<T>>&& slices)
    {
        using coordinate_type = typename std::decay_t<E>::coordinate_type;
        using dimension_type = typename std::decay_t<E>::dimension_type;
        using dimension_label_list = typename dimension_type::label_list;
        using view_type = xvariable_view<xtl::closure_type_t<E>>;
        using squeeze_map = typename view_type::squeeze_map;
        using coordinate_view_type = typename view_type::coordinate_type;
        using map_type = typename coordinate_view_type::map_type;
        using axis_type = typename coordinate_view_type::axis_type;
        using size_type = typename std::decay_t<E>::size_type;

        const coordinate_type& underlying_coords = e.coordinates();
        map_type coord_map;
        squeeze_map sq_map;
        dimension_label_list dim_label_list;

        for (const auto& dim_label : e.dimension_labels())
        {
            const auto& axis = underlying_coords[dim_label];
            auto slice_iter = slices.find(dim_label);
            if (slice_iter != slices.end())
            {
                if (auto* sq = (slice_iter->second).get_all())
                {
                    coord_map.emplace(dim_label, axis_type(axis, xt::xall<size_type>(axis.size())));
                    dim_label_list.push_back(dim_label);
                }
                else if (auto* sq = (slice_iter->second).get_squeeze())
                {
                    sq_map[e.dimension_mapping()[dim_label]] = *sq;
                }
                else
                {
                    auto* sl = (slice_iter->second).get_slice();
                    coord_map.emplace(dim_label, axis_type(axis, *sl));
                    dim_label_list.push_back(dim_label);
                }
            }
            else
            {
                coord_map.emplace(dim_label, axis_type(axis, xt::xall<size_type>(axis.size())));
                dim_label_list.push_back(dim_label);
            }
        }

        coordinate_view_type coordinate_view(std::move(coord_map));
        dimension_type view_dimension(std::move(dim_label_list));

        return view_type(std::forward<E>(e),
                         std::move(coordinate_view),
                         std::move(view_dimension),
                         std::move(sq_map));
    }
}

#endif