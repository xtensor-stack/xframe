/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XREINDEX_DATA_HPP
#define XREINDEX_DATA_HPP

#include "xtensor/xexpression.hpp"
#include "xtensor/xiterable.hpp"
#include "xtensor/xlayout.hpp"

namespace xf
{
    template <class RV>
    class xreindex_data;
}

namespace xt
{
    template <class RV>
    struct xiterable_inner_types<xf::xreindex_data<RV>>
    {
        using xexpression_type = std::decay_t<RV>;
        using inner_shape_type = typename xexpression_type::shape_type;
        using const_stepper = xindexed_stepper<xf::xreindex_data<RV>, true>;
        using stepper = const_stepper;
    };
}

namespace xf
{
    template <class RV>
    class xreindex_data : public xt::xexpression<xreindex_data<RV>>,
                          public xt::xconst_iterable<xreindex_data<RV>>
    {
    public:

        using self_type = xreindex_data<RV>;
        using xexpression_type = std::decay_t<RV>;
        using expression_tag = xt::xtensor_expression_tag;

        using value_type = typename xexpression_type::value_type;
        using reference = typename xexpression_type::reference;
        using const_reference = typename xexpression_type::const_reference;
        using pointer = typename xexpression_type::pointer;
        using const_pointer = typename xexpression_type::const_pointer;
        using size_type = typename xexpression_type::size_type;
        using difference_type = typename xexpression_type::difference_type;

        using iterable_base = xt::xconst_iterable<self_type>;
        using inner_shape_type = typename iterable_base::inner_shape_type;
        using shape_type = inner_shape_type;

        using stepper = typename iterable_base::stepper;
        using const_stepper = typename iterable_base::const_stepper;

        static constexpr xt::layout_type static_layout = xt::layout_type::dynamic;
        static constexpr bool contiguous_layout = false;

        explicit xreindex_data(const xexpression_type& e);

        size_type size() const noexcept;
        size_type dimension() const noexcept;
        const inner_shape_type& shape() const noexcept;
        xt::layout_type layout() const noexcept;

        template <class... Args>
        const_reference operator()(Args... args) const;

        template <class... Args>
        const_reference at(Args... args) const;

        template <class... Args>
        const_reference unchecked(Args... args) const;

        template <class S>
        xt::disable_integral_t<S, const_reference> operator[](const S& index) const;
        template <class I>
        const_reference operator[](std::initializer_list<I> index) const;
        const_reference operator[](size_type i) const;

        template <class It>
        const_reference element(It first, It last) const;

        template <class S>
        bool braodcast_shape(S& shape, bool reuse_cache = false) const;

        template <class S>
        bool has_linear_assign(const S& strides) const noexcept;

        template <class S>
        const_stepper stepper_begin(const S& shape) const noexcept;
        template <class S>
        const_stepper stepper_end(const S& shape, xt::layout_type l) const noexcept;

    private:

        using index_type = typename xexpression_type::template index_type<>;
        const xexpression_type& m_e;
    };

    /********************************
     * xreindex_data implementation *
     ********************************/

    template <class RV>
    inline xreindex_data<RV>::xreindex_data(const xexpression_type& e)
        : m_e(e)
    {
    }

    template <class RV>
    inline auto xreindex_data<RV>::size() const noexcept -> size_type
    {
        return m_e.size();
    }

    template <class RV>
    inline auto xreindex_data<RV>::dimension() const noexcept -> size_type
    {
        return m_e.dimension();
    }

    template <class RV>
    inline auto xreindex_data<RV>::shape() const noexcept -> const inner_shape_type&
    {
        return m_e.shape();
    }

    template <class RV>
    inline auto xreindex_data<RV>::layout() const noexcept -> xt::layout_type
    {
        return xt::layout_type::dynamic;
    }

    template <class RV>
    template <class... Args>
    inline auto xreindex_data<RV>::operator()(Args... args) const -> const_reference
    {
        return m_e(args...);
    }

    template <class RV>
    template <class... Args>
    inline auto xreindex_data<RV>::at(Args... args) const -> const_reference
    {
        return m_e(args...);
    }

    template <class RV>
    template <class... Args>
    inline auto xreindex_data<RV>::unchecked(Args... args) const -> const_reference
    {
        return m_e(args...);
    }

    template <class RV>
    template <class S>
    inline auto xreindex_data<RV>::operator[](const S& index) const -> xt::disable_integral_t<S, const_reference>
    {
        index_type idx = xtl::forward_sequence<index_type, const S&>(index);
        return m_e.element(std::move(idx));
    }

    template <class RV>
    template <class I>
    inline auto xreindex_data<RV>::operator[](std::initializer_list<I> index) const -> const_reference
    {
        index_type idx(index);
        return m_e.element(std::move(idx));
    }

    template <class RV>
    inline auto xreindex_data<RV>::operator[](size_type i) const -> const_reference
    {
        index_type idx = xtl::make_sequence<index_type>(size_type(1), i);
        return m_e.element(std::move(idx));
    }

    template <class RV>
    template <class It>
    inline auto xreindex_data<RV>::element(It first, It last) const -> const_reference
    {
        index_type idx = xtl::make_sequence<index_type>(static_cast<size_type>(std::distance(first, last)));
        std::copy(first, last, idx.begin());
        return m_e.element(std::move(idx));
    }

    template <class RV>
    template <class S>
    inline bool xreindex_data<RV>::braodcast_shape(S& s, bool) const
    {
        return xt::broadcast_shape(shape(), s);
    }

    template <class RV>
    template <class S>
    inline bool xreindex_data<RV>::has_linear_assign(const S& strides) const noexcept
    {
        return false;
    }

    template <class RV>
    template <class S>
    inline auto xreindex_data<RV>::stepper_begin(const S& /*shape*/) const noexcept -> const_stepper
    {
        return const_stepper(this, size_type(0));
    }

    template <class RV>
    template <class S>
    inline auto xreindex_data<RV>::stepper_end(const S& /*shape*/, xt::layout_type /*l*/) const noexcept -> const_stepper
    {
        return const_stepper(this, size(), true);
    }
}

#endif
