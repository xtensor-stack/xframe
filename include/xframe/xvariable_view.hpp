/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVARIABLE_VIEW_HPP
#define XFRAME_XVARIABLE_VIEW_HPP

#include "xtensor/xdynamic_view.hpp"

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
        using dimension_closure_type = dimension_type;
        using key_type = typename coordinate_type::key_type;
        using size_type = typename coordinate_type::size_type;
        using label_list = typename coordinate_type::label_list;
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
        using temporary_coordinate_type = xf::xcoordinate<typename base_type::key_type,
                                                          typename base_type::label_list,
                                                          typename base_type::size_type>;
        using temporary_data_type = xoptional_assembly<xarray<typename optional_type::value_type>, xarray<bool>>;
        using temporary_type = xf::xvariable_container<temporary_coordinate_type, temporary_data_type>;
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
        using underlying_data_type = typename xexpression_type::data_type;
        using data_type = xt::xdynamic_view<xt::apply_cv_t<CT, underlying_data_type>&, typename underlying_data_type::shape_type>;
        using slice_vector = xt::xdynamic_slice_vector;

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
        using dimension_list = typename coordinate_base::dimension_list;
        using shape_type = typename data_type::shape_type;
        using squeeze_map = std::map<typename dimension_type::mapped_type, typename coordinate_type::index_type>;
        using temporary_type = typename inner_types::temporary_type;

        using expression_tag = xvariable_expression_tag;

        template <std::size_t N = dynamic()>
        using selector_traits = xselector_traits<coordinate_type, dimension_type, N>;
        template <std::size_t N = dynamic()>
        using index_type = typename selector_traits<N>::index_type;
        template <std::size_t N = dynamic()>
        using selector_type = typename selector_traits<N>::selector_type;
        template <std::size_t N = dynamic()>
        using selector_sequence_type = typename selector_traits<N>::selector_sequence_type;
        template <std::size_t N = dynamic()>
        using iselector_type = typename selector_traits<N>::iselector_type;
        template <std::size_t N = dynamic()>
        using iselector_sequence_type = typename selector_traits<N>::iselector_sequence_type;
        template <std::size_t N = dynamic()>
        using locator_type = typename selector_traits<N>::locator_type;
        template <std::size_t N = dynamic()>
        using locator_sequence_type = typename selector_traits<N>::locator_sequence_type;

        static const_reference missing();

        template <class E>
        xvariable_view(E&& e, coordinate_type&& coord, dimension_type&& dim, squeeze_map&& squeeze, slice_vector&& slices);

        xvariable_view(const xvariable_view&) = default;
        xvariable_view& operator=(const xvariable_view&);

        template <class E>
        xvariable_view& operator=(const xt::xexpression<E>& e);

        template <class E>
        xt::disable_xexpression<E, xvariable_view>& operator=(const E& e);

        using coordinate_base::size;
        using coordinate_base::dimension;
        using coordinate_base::dimension_labels;
        using coordinate_base::coordinates;
        using coordinate_base::dimension_mapping;
        using coordinate_base::broadcast_coordinates;
        using coordinate_base::broadcast_dimensions;

        const shape_type& shape() const noexcept;

        data_type& data() noexcept;
        const data_type& data() const noexcept;

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
        reference locate_element(const locator_sequence_type<N>& locator);

        template <std::size_t N = dynamic()>
        const_reference locate_element(const locator_sequence_type<N>& locator) const;

        template <std::size_t N = dynamic()>
        reference locate_element(locator_sequence_type<N>&& locator);

        template <std::size_t N = dynamic()>
        const_reference locate_element(locator_sequence_type<N>&& locator) const;

        template <std::size_t N = dynamic()>
        reference select(const selector_sequence_type<N>& selector);

        template <class Join = XFRAME_DEFAULT_JOIN, std::size_t N = std::numeric_limits<size_type>::max()>
        const_reference select(const selector_sequence_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        reference select(selector_sequence_type<N>&& selector);

        template <class Join = XFRAME_DEFAULT_JOIN, std::size_t N = dynamic()>
        const_reference select(selector_sequence_type<N>&& selector) const;

        template <std::size_t N = dynamic()>
        reference iselect(const iselector_sequence_type<N>& selector);

        template <std::size_t N = dynamic()>
        const_reference iselect(const iselector_sequence_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        reference iselect(iselector_sequence_type<N>&& selector);

        template <std::size_t N = dynamic()>
        const_reference iselect(iselector_sequence_type<N>&& selector) const;

    private:

        using internal_index_type = std::vector<size_type>;

        template <std::size_t... I, class... Args>
        reference access_impl(std::index_sequence<I...>, Args... args);

        template <std::size_t... I, class... Args>
        const_reference access_impl(std::index_sequence<I...>, Args... args) const;

        template <class... Args>
        internal_index_type build_accessor(Args&&... args) const;

        template <std::size_t I, class T, class... Args>
        void fill_accessor(internal_index_type& accessor, T idx, Args... args) const;

        template <std::size_t I>
        void fill_accessor(internal_index_type& accessor) const;

        template <class It>
        internal_index_type build_element_accessor(It first, It last) const;

        template <std::size_t... I, class... Args>
        reference locate_impl(std::index_sequence<I...>, Args&&... args);

        template <std::size_t... I, class... Args>
        const_reference locate_impl(std::index_sequence<I...>, Args&&... args) const;

        template <class... Args>
        internal_index_type build_locator(Args&&... args) const;

        template <std::size_t I, class T, class... Args>
        void fill_locator(internal_index_type& locator, T idx, Args&&... args) const;

        template <std::size_t I>
        void fill_locator(internal_index_type& locator) const;

        template <std::size_t N>
        internal_index_type build_element_locator(locator_sequence_type<N>&& locator) const;

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
        void adapt_iselector(iselector_sequence_type<N>& selector) const;

        void assign_temporary_impl(temporary_type&& tmp);

        CT m_e;
        squeeze_map m_squeeze;
        data_type m_data;

        friend class xt::xview_semantic<xvariable_view<CT>>;
    };

    template <class CT>
    std::ostream& operator<<(std::ostream& out, const xvariable_view<CT>& view);

    /***************************
     * xvariable_view builders *
     ***************************/

    template <class E, class... S>
    auto ilocate(E&& e, S&&... slices);

    template <class L = XFRAME_DEFAULT_LABEL_LIST, class E, class... S>
    auto locate(E&& e, S&&... slices);

    template <class E, class L = XFRAME_DEFAULT_LABEL_LIST>
    auto select(E&& e, std::map<typename std::decay_t<E>::key_type, xaxis_slice<L>>&& slices);

    template <class E, class T = typename std::decay_t<E>::difference_type>
    auto iselect(E&& e, std::map<typename std::decay_t<E>::key_type, xt::xdynamic_slice<T>>&& slices);

    /*********************************
     * xvariable_view implementation *
     *********************************/

    template <class CT>
    template <class E>
    inline xvariable_view<CT>::xvariable_view(E&& e, coordinate_type&& coord, dimension_type&& dim, squeeze_map&& squeeze,
                                              slice_vector&& slices)
        : coordinate_base(std::move(coord), std::move(dim)),
          m_e(std::forward<E>(e)),
          m_squeeze(std::move(squeeze)),
          m_data(xt::dynamic_view(m_e.data(), slices))
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
    template <class E>
    xt::disable_xexpression<E, xvariable_view<CT>>& xvariable_view<CT>::operator=(const E& e)
    {
        data().fill(e);
        return *this;
    }

    template <class CT>
    inline auto xvariable_view<CT>::shape() const noexcept -> const shape_type&
    {
        return data().shape();
    }

    template <class CT>
    inline auto xvariable_view<CT>::data() noexcept -> data_type&
    {
        return m_data;
    }

    template <class CT>
    inline auto xvariable_view<CT>::data() const noexcept -> const data_type&
    {
        return m_data;
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
            return m_e.element(idx);
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
            return m_e.element(idx);
        }
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::element(const index_type<N>& index) -> reference
    {
        auto idx = build_element_accessor(index.cbegin(), index.cend());
        return m_e.element(idx.cbegin(), idx.cend());
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::element(const index_type<N>& index) const -> const_reference
    {
        auto idx = build_element_accessor(index.cbegin(), index.cend());
        return m_e.element(idx.cbegin(), idx.cend());
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::element(index_type<N>&& index) -> reference
    {
        auto idx = build_element_accessor(index.cbegin(), index.cend());
        return m_e.element(idx);
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::element(index_type<N>&& index) const -> const_reference
    {
        auto idx = build_element_accessor(index.cbegin(), index.cend());
        return m_e.element(idx);
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
            return m_e.element(idx);
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
            return m_e.element(idx);
        }
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::locate_element(const locator_sequence_type<N>& locator) -> reference
    {
        locator_sequence_type<N> tmp(locator);
        auto idx = build_element_locator<N>(std::move(tmp));
        return m_e.element(idx);
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::locate_element(const locator_sequence_type<N>& locator) const -> const_reference
    {
        locator_sequence_type<N> tmp(locator);
        auto idx = build_element_locator<N>(std::move(locator));
        return m_e.element(idx);
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::locate_element(locator_sequence_type<N>&& locator) -> reference
    {
        auto idx = build_element_locator<N>(std::move(locator));
        return m_e.element(idx);
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::locate_element(locator_sequence_type<N>&& locator) const -> const_reference
    {
        auto idx = build_element_locator<N>(std::move(locator));
        return m_e.element(idx);
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::select(const selector_sequence_type<N>& selector) -> reference
    {
        return select_impl(selector_type<N>(selector));
    }

    template <class CT>
    template <class Join, std::size_t N>
    inline auto xvariable_view<CT>::select(const selector_sequence_type<N>& selector) const -> const_reference
    {
        return select_join<Join>(selector_type<N>(selector));
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::select(selector_sequence_type<N>&& selector) -> reference
    {
        return select_impl(selector_type<N>(std::move(selector)));
    }

    template <class CT>
    template <class Join, std::size_t N>
    inline auto xvariable_view<CT>::select(selector_sequence_type<N>&& selector) const -> const_reference
    {
        return select_join<Join>(selector_type<N>(std::move(selector)));
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::iselect(const iselector_sequence_type<N>& selector) -> reference
    {
        return iselect(iselector_sequence_type<N>(selector));
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::iselect(const iselector_sequence_type<N>& selector) const -> const_reference
    {
        return iselect(iselector_sequence_type<N>(selector));
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::iselect(iselector_sequence_type<N>&& selector) -> reference
    {
        iselector_sequence_type<N> tmp_selector(std::move(selector));
        adapt_iselector<N>(tmp_selector);
        return select_impl(iselector_type<N>(std::move(tmp_selector)));
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::iselect(iselector_sequence_type<N>&& selector) const -> const_reference
    {
        iselector_sequence_type<N> tmp_selector(std::move(selector));
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
    inline auto xvariable_view<CT>::build_accessor(Args&&... args) const -> internal_index_type
    {
        internal_index_type accessor(m_e.dimension());
        fill_accessor<0>(accessor, std::forward<Args>(args)...);
        fill_squeeze(accessor);
        return accessor;
    }

    template <class CT>
    template <std::size_t I, class T, class... Args>
    inline void xvariable_view<CT>::fill_accessor(internal_index_type& accessor, T idx, Args... args) const
    {
        std::size_t new_index = m_e.dimension_mapping()[dimension_labels()[I]];
        accessor[new_index] = coordinates()[dimension_labels()[I]].index(idx);
        fill_accessor<I + 1>(accessor, std::forward<Args>(args)...);
    }

    template <class CT>
    template <std::size_t I>
    inline void xvariable_view<CT>::fill_accessor(internal_index_type& /*accessor*/) const
    {
    }

    template <class CT>
    template <class It>
    inline auto xvariable_view<CT>::build_element_accessor(It first, It last) const -> internal_index_type
    {
        internal_index_type res(m_e.dimension());
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
    inline auto xvariable_view<CT>::build_locator(Args&&... args) const -> internal_index_type
    {
        internal_index_type locator(m_e.dimension());
        fill_locator<0>(locator, std::forward<Args>(args)...);
        fill_squeeze(locator);
        return locator;
    }

    template <class CT>
    template <std::size_t I, class T, class... Args>
    inline void xvariable_view<CT>::fill_locator(internal_index_type& locator, T idx, Args&&... args) const
    {
        std::size_t new_index = m_e.dimension_mapping()[dimension_labels()[I]];
        locator[new_index] = coordinates()[dimension_labels()[I]][idx];
        fill_locator<I + 1>(locator, std::forward<Args>(args)...);
    }

    template <class CT>
    template <std::size_t I>
    inline void xvariable_view<CT>::fill_locator(internal_index_type&) const
    {
    }

    template <class CT>
    template <std::size_t N>
    inline auto xvariable_view<CT>::build_element_locator(locator_sequence_type<N>&& locator) const -> internal_index_type
    {
        if (m_squeeze.empty())
        {
            locator_type<N> loc(std::move(locator));
            return loc.get_index(coordinates(), dimension_mapping());
        }
        else
        {
            internal_index_type res(m_e.dimension());
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
        return m_e.element(idx);
    }

    template <class CT>
    template <class S>
    inline auto xvariable_view<CT>::select_impl(const S& selector) const -> const_reference
    {
        typename S::index_type idx = selector.get_index(coordinates(), m_e.dimension_mapping());
        fill_squeeze(idx);
        return m_e.element(idx);
    }

    template <class CT>
    template <class S>
    inline auto xvariable_view<CT>::select_outer(const S& selector) const -> const_reference
    {
        typename S::outer_index_type idx = selector.get_outer_index(coordinates(), m_e.dimension_mapping());
        if (idx.second)
        {
            fill_squeeze(idx.first);
            return m_e.element(idx.first);
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
    inline void xvariable_view<CT>::adapt_iselector(iselector_sequence_type<N>& selector) const
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
        selector_sequence_type<> selector(index.size());
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

    template <class CT>
    inline std::ostream& operator<<(std::ostream& out, const xvariable_view<CT>& v)
    {
        const auto& dims = v.dimension_labels();
        const auto& coords = v.coordinates();

        // TODO: fixe xio.hpp so it can prints expression without strides
        //out << v.data() << std::endl;

        const auto& data = v.data();
        for (size_t i = 0; i < data.shape()[0]; ++i)
        {
            out << '(';
            for (size_t j = 0; j < data.shape()[1]; ++j)
            {
                out << data(i, j) << ", ";
            }
            out << ')' << std::endl;
        }
        out << "Coordinates:" << std::endl;

        std::size_t max_length = std::accumulate(dims.cbegin(), dims.cend(), std::size_t(0),
            [](std::size_t res, const auto& d) { return std::max(res, d.size()); });

        for (const auto& d : dims)
        {
            std::size_t nb_spaces = max_length - d.size();
            std::string spaces(nb_spaces, ' ');
            out << d << spaces << ": " << coords[d] << std::endl;
        }

        return out;
    }

    /******************************************
     * xvariable_view builders implementation *
     ******************************************/

    namespace detail
    {
        template <class R>
        struct range_adaptor_getter
        {
            using return_type = typename xaxis_index_slice<R>::storage_type;
            explicit range_adaptor_getter(std::size_t size)
                : m_size(size)
            {
            }

            template <class T>
            return_type operator()(const T&) const
            {
                throw std::runtime_error("Unvalid slice where xrange_adaptor was expected");
            }

            template <class A, class B, class C>
            return_type operator()(const xt::xrange_adaptor<A, B, C>& r) const
            {
                return r.get(m_size);
            }

            std::size_t m_size;
        };

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
            using slice_vector = xt::xdynamic_slice_vector;

            map_type coord_map;
            squeeze_map sq_map;
            dimension_label_list dim_label_list;
            slice_vector dyn_slices;
        };

        template <class T, class E>
        class slice_to_view_param
        {
        public:

            using dimension_type = typename std::decay_t<E>::dimension_type;
            using dimension_label_list = typename dimension_type::label_list;
            using label_type = typename dimension_label_list::value_type;
            using param_type = view_params<E>;
            using view_type = xvariable_view<xtl::closure_type_t<E>>;
            using coordinate_view_type = typename view_type::coordinate_type;
            using axis_type = typename coordinate_view_type::axis_type;
            using axis_slice_type = typename axis_type::slice_type;
            using size_type = typename std::decay_t<E>::size_type;
            using dynamic_slice = xt::xdynamic_slice<std::ptrdiff_t>;

            slice_to_view_param(const E& e, param_type& param)
                : m_e(e), m_param(param)
            {
            }

            inline void operator()(T slice, const label_type& dim_label)
            {
                auto dim_idx = m_e.dimension_mapping()[dim_label];
                m_param.sq_map[dim_idx] = slice;
                m_param.dyn_slices[dim_idx] = static_cast<std::ptrdiff_t>(slice);
            }

            inline void operator()(const xt::xkeep_slice<T>& slice, const label_type& dim_label)
            {
                const auto& axis = m_e.coordinates()[dim_label];
                xt::xkeep_slice<T> coord_slice(slice);
                coord_slice.normalize(axis.size());
                m_param.coord_map.emplace(dim_label, axis_type(axis, axis_slice_type(std::move(coord_slice))));
                m_param.dim_label_list.push_back(dim_label);
                auto dim_idx = m_e.dimension_mapping()[dim_label];
                m_param.dyn_slices[dim_idx] = xt::xkeep_slice<std::ptrdiff_t>(slice);
            }

            inline void operator()(const xt::xdrop_slice<T>& slice, const label_type& dim_label)
            {
                const auto& axis = m_e.coordinates()[dim_label];
                xt::xdrop_slice<T> coord_slice(slice);
                coord_slice.normalize(axis.size());
                m_param.coord_map.emplace(dim_label, axis_type(axis, axis_slice_type(std::move(coord_slice))));
                m_param.dim_label_list.push_back(dim_label);
                auto dim_idx = m_e.dimension_mapping()[dim_label];
                m_param.dyn_slices[dim_idx] = xt::xdrop_slice<std::ptrdiff_t>(slice);
            }

            inline void operator()(xt::xall_tag, const label_type& dim_label)
            {
                const auto& axis = m_e.coordinates()[dim_label];
                m_param.coord_map.emplace(dim_label, axis_type(axis, xt::xall<std::size_t>(axis.size())));
                m_param.dim_label_list.push_back(dim_label);
                auto dim_idx = m_e.dimension_mapping()[dim_label];
                m_param.dyn_slices[dim_idx] = xt::xall_tag();
            }

            inline void operator()(xt::xellipsis_tag, const label_type&)
            {
                throw std::runtime_error("xellipsis_tag not supported");
            }

            inline void operator()(xt::xnewaxis_tag, const label_type&)
            {
                throw std::runtime_error("xnewaxis_tag not supported");
            }

            template <class A, class B, class C>
            inline void operator()(const xt::xrange_adaptor<A, B, C>& slice, const label_type& dim_label)
            {
                const auto& axis = m_e.coordinates()[dim_label];
                range_adaptor_getter<size_type> rag(axis.size());
                auto ra = rag(slice);
                m_param.coord_map.emplace(dim_label, axis_type(axis, axis_slice_type(ra)));
                m_param.dim_label_list.push_back(dim_label);
                auto dim_idx = m_e.dimension_mapping()[dim_label];
                m_param.dyn_slices[dim_idx] = slice;
            }

            inline void operator()(const xt::xrange<T>& slice, const label_type& dim_label)
            {
                const auto& axis = m_e.coordinates()[dim_label];
                m_param.coord_map.emplace(dim_label, axis_type(axis, axis_slice_type(slice)));
                m_param.dim_label_list.push_back(dim_label);
                auto dim_idx = m_e.dimension_mapping()[dim_label];
                m_param.dyn_slices[dim_idx] = slice;
            }

            inline void operator()(const xt::xstepped_range<T>& slice, const label_type& dim_label)
            {
                const auto& axis = m_e.coordinates()[dim_label];
                m_param.coord_map.emplace(dim_label, axis_type(axis, axis_slice_type(slice)));
                m_param.dim_label_list.push_back(dim_label);
                auto dim_idx = m_e.dimension_mapping()[dim_label];
                m_param.dyn_slices[dim_idx] = slice;
            }

        private:

            const E& m_e;
            param_type& m_param;
        };

        template <class E>
        struct locate_params_builder
        {
            using param_type = view_params<E>;

            template <std::size_t I>
            inline void fill_view_params(param_type& /*param*/, const E& /*e*/)
            {
            }

            template <std::size_t I, class SL, class... S>
            inline void fill_view_params(param_type& param, const E& e, SL&& sl, S&&... slices)
            {
                using axis_type = typename view_params<E>::axis_type;
                using dynamic_slice = xt::xdynamic_slice<std::ptrdiff_t>;
                const auto& dim_label = e.dimension_labels()[I];
                const auto& axis = e.coordinates()[dim_label];
                if (auto* sq = sl.get_squeeze())
                {
                    auto idx_sq = axis[*sq];
                    param.sq_map[I] = idx_sq;
                    param.dyn_slices[I] = static_cast<std::ptrdiff_t>(idx_sq);
                }
                else
                {
                    auto idx_slice = sl.build_index_slice(axis);
                    param.dyn_slices[I] = idx_slice.template convert_storage<dynamic_slice, std::ptrdiff_t>();
                    param.coord_map.emplace(dim_label, axis_type(axis, std::move(idx_slice)));
                    param.dim_label_list.push_back(dim_label);
                }
                fill_view_params<I + 1>(param, e, std::forward<S>(slices)...);
            }
        };

        template <class E>
        struct ilocate_param_builder
        {
            using param_type = view_params<E>;

            template <std::size_t I>
            inline void fill_view_params(param_type& /*param*/, const E& /*e*/)
            {
            }

            template <std::size_t I, class SL, class... S>
            inline void fill_view_params(param_type& param, const E& e, SL&& sl, S&&... slices)
            {
                using size_type = typename std::decay_t<E>::size_type;
                slice_to_view_param<size_type, E> visitor(e, param);
                const auto& dim_label = e.dimension_labels()[I];
                xtl::visit([&visitor, &dim_label](auto&& slice) { visitor(slice, dim_label); }, sl);
                fill_view_params<I + 1>(param, e, std::forward<S>(slices)...);
            }
        };

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
        using builder_type = detail::ilocate_param_builder<E>;
        using view_param_type = detail::view_params<E>;
        using coordinate_view_type = typename view_param_type::coordinate_view_type;
        using dimension_type = typename view_param_type::dimension_type;
        using view_type = typename view_param_type::view_type;

        view_param_type params;
        params.dyn_slices.resize(sizeof...(S));
        builder_type builder;
        builder.template fill_view_params<0>(params, e, xt::xdynamic_slice<std::ptrdiff_t>(std::forward<S>(slices))...);

        coordinate_view_type coordinate_view(std::move(params.coord_map));
        dimension_type view_dimension(std::move(params.dim_label_list));

        return view_type(std::forward<E>(e),
                         std::move(coordinate_view),
                         std::move(view_dimension),
                         std::move(params.sq_map),
                         std::move(params.dyn_slices));
    }

    template <class L, class E, class... S>
    inline auto locate(E&& e, S&&... slices)
    {
        using builder_type = detail::locate_params_builder<E>;
        using view_param_type = typename builder_type::param_type;
        using coordinate_view_type = typename view_param_type::coordinate_view_type;
        using dimension_type = typename view_param_type::dimension_type;
        using view_type = typename view_param_type::view_type;

        view_param_type params;
        params.dyn_slices.resize(sizeof...(S));
        builder_type builder;
        builder.template fill_view_params<0>(params, e, xaxis_slice<L>(std::forward<S>(slices))...);

        coordinate_view_type coordinate_view(std::move(params.coord_map));
        dimension_type view_dimension(std::move(params.dim_label_list));

        return view_type(std::forward<E>(e),
                         std::move(coordinate_view),
                         std::move(view_dimension),
                         std::move(params.sq_map),
                         std::move(params.dyn_slices));
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
        using dynamic_slice = xt::xdynamic_slice<std::ptrdiff_t>;

        const coordinate_type& underlying_coords = e.coordinates();
        map_type coord_map;
        squeeze_map sq_map;
        dimension_label_list dim_label_list;
        xt::xdynamic_slice_vector dsv(underlying_coords.size());

        for(const auto& dim_label: e.dimension_labels())
        {
            auto dim_index = e.dimension_mapping()[dim_label];
            const auto& axis = underlying_coords[dim_label];
            auto slice_iter = slices.find(dim_label);
            if (slice_iter != slices.end())
            {
                if (auto* sq = (slice_iter->second).get_squeeze())
                {
                    auto idx_sq = axis[*sq];
                    dsv[dim_index] = static_cast<std::ptrdiff_t>(idx_sq);
                    sq_map[dim_index] = idx_sq;
                }
                else
                {
                    auto idx_slice = (slice_iter->second).build_index_slice(axis);
                    dsv[dim_index] = idx_slice.template convert_storage<dynamic_slice, std::ptrdiff_t>();
                    coord_map.emplace(dim_label, axis_type(axis, std::move(idx_slice)));
                    dim_label_list.push_back(dim_label);
                }
            }
            else
            {
                dsv[dim_index] = xt::xall_tag();
                coord_map.emplace(dim_label, axis_type(axis, xt::xall<size_type>(axis.size())));
                dim_label_list.push_back(dim_label);
            }
        }

        coordinate_view_type coordinate_view(std::move(coord_map));
        dimension_type view_dimension(std::move(dim_label_list));

        return view_type(std::forward<E>(e),
                         std::move(coordinate_view),
                         std::move(view_dimension),
                         std::move(sq_map),
                         std::move(dsv));
    }

    template <class E, class T>
    inline auto iselect(E&& e, std::map<typename std::decay_t<E>::key_type, xt::xdynamic_slice<T>>&& slices)
    {
        using visitor_type = detail::slice_to_view_param<T, E>;
        using view_param_type = typename visitor_type::param_type;
        using view_type = xvariable_view<xtl::closure_type_t<E>>;
        using coordinate_view_type = typename view_type::coordinate_type;
        using dimension_type = typename std::decay_t<E>::dimension_type;
        using axis_type = typename coordinate_view_type::axis_type;
        using size_type = typename std::decay_t<E>::size_type;

        view_param_type param;
        param.dyn_slices.resize(slices.size());
        visitor_type visitor(e, param);

        for (const auto& dim_label : e.dimension_labels())
        {
            const auto& axis = e.coordinates()[dim_label];
            auto slice_iter = slices.find(dim_label);
            if (slice_iter != slices.end())
            {
                xtl::visit([&visitor, &dim_label](auto&& val) { visitor(val, dim_label); }, slice_iter->second);
            }
            else
            {
                param.coord_map.emplace(dim_label, axis_type(axis, xt::xall<size_type>(axis.size())));
                param.dim_label_list.push_back(dim_label);
            }
        }

        coordinate_view_type coordinate_view(std::move(param.coord_map));
        dimension_type view_dimension(std::move(param.dim_label_list));

        return view_type(std::forward<E>(e),
                         std::move(coordinate_view),
                         std::move(view_dimension),
                         std::move(param.sq_map),
                         std::move(param.dyn_slices));
    }
}

#endif
