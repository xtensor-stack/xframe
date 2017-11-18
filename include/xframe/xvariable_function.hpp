/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVARIABLE_FUNCTION_HPP
#define XFRAME_XVARIABLE_FUNCTION_HPP

#include "xtensor/xoptional.hpp"

#include "xcoordinate.hpp"

namespace xf
{

    /**********************
     * xvariable_function *
     **********************/

    template <class F,class R, class... CT>
    class xvariable_function : public xfunction<xvariable_function<F, R, CT...>>,
                               public xexpression<xvariable_function<F, R, CT...>>
    {
    public:

        using coordinate_type = std::common_type_t<typename CT::coordinate_type...>;

        template <class Func, class U = std::enable_if<!std::is_base_of<Func, self_type>::value>>
        xvariable_function(Func&& f, CT... e) noexcept;

    private:

        mutable coordinate_type m_coordinate;
        mutable bool m_coordinate_computed;
    };

    /*************************************
     * xvariable_function implementation *
     *************************************/

    
}

#endif
