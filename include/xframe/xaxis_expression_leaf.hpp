/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay, Wolf Vollprecht and   *
* Martin Renou                                                             *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_EXPRESSION_LEAF_HPP
#define XFRAME_XAXIS_EXPRESSION_LEAF_HPP

#include "xtl/xvariant.hpp"
#include "xtl/xmeta_utils.hpp"

#include "xtensor/xoptional.hpp"

#include "xframe_config.hpp"
#include "xframe_utils.hpp"
#include "xframe_expression.hpp"

namespace xf
{
    /*************************
     * xaxis_expression_leaf *
     *************************/

    template <class CTA>
    class xaxis_expression_leaf : public xt::xexpression<xaxis_expression_leaf<CTA>>
    {
    public:

        using named_axis_type = std::decay_t<CTA>;
        using name_type = typename named_axis_type::name_type;
        using size_type = typename named_axis_type::size_type;

        using value_type = typename named_axis_type::value_type;
        using reference = typename named_axis_type::reference;
        using const_reference = typename named_axis_type::const_reference;
        using pointer = typename named_axis_type::pointer;
        using const_pointer = typename named_axis_type::const_pointer;

        template <std::size_t N = dynamic()>
        using selector_sequence_type = detail::xselector_sequence_t<std::pair<name_type, size_type>, N>;

        using expression_tag = xaxis_expression_tag;

        template <class AX>
        xaxis_expression_leaf(AX&& n_axis) noexcept;

        template <std::size_t N = std::numeric_limits<size_type>::max()>
        const_reference operator()(const selector_sequence_type<N>& selector) const;

    private:

        xaxis_closure_t<CTA> m_named_axis;
    };

    /****************************************
     * xaxis_expression_leaf implementation *
     ****************************************/

    template <class CTA>
    template <class AX>
    inline xaxis_expression_leaf<CTA>::xaxis_expression_leaf(AX&& n_axis) noexcept
        : m_named_axis(std::forward<AX>(n_axis))
    {
    }

    template <class CTA>
    template <std::size_t N>
    auto xaxis_expression_leaf<CTA>::operator()(const selector_sequence_type<N>& selector) const -> const_reference
    {
        for (const auto& sel: selector)
        {
            if (m_named_axis.name() == sel.first)
            {
                return m_named_axis.label(sel.second);
            }
        }
        throw std::runtime_error(std::string("Missing label for axis ") + m_named_axis.name());
    }
}

#endif
