/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_SCALAR_HPP
#define XFRAME_XAXIS_SCALAR_HPP

#include "xtl/xvariant.hpp"
#include "xtl/xmeta_utils.hpp"

#include "xtensor/xoptional.hpp"

#include "xframe_config.hpp"
#include "xframe_utils.hpp"
#include "xframe_expression.hpp"

namespace xf
{
    struct xaxis_scalar_name
    {
    };

    /****************
     * xaxis_scalar *
     ****************/

    template <class CT>
    class xaxis_scalar
    {
    public:

        using name_type = xaxis_scalar_name;

        using data_type = xt::xscalar<CT>;
        using value_type = typename data_type::value_type;
        using reference = typename data_type::reference;
        using const_reference = typename data_type::const_reference;
        using pointer = typename data_type::pointer;
        using const_pointer = typename data_type::const_pointer;
        using size_type = typename data_type::size_type;

        using expression_tag = xaxis_expression_tag;

        xaxis_scalar(const xt::xscalar<CT>& v) noexcept;

        template <std::size_t N = dynamic(), class S>
        const_reference operator()(S&& /*selector*/) const;

    private:

        data_type m_data;
    };

    /*******************************
     * xaxis_scalar implementation *
     *******************************/

    template <class CT>
    xaxis_scalar<CT>::xaxis_scalar(const xt::xscalar<CT>& v) noexcept
        : m_data(v)
    {
    }

    template <class CT>
    template <std::size_t N, class S>
    auto xaxis_scalar<CT>::operator()(S&& /*selector*/) const -> const_reference
    {
        return m_data;
    }
}

#endif
