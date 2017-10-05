/***************************************************************************
* Copyright (c) 2017, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_UTILS_HPP
#define XFRAME_UTILS_HPP

#include <iterator>

namespace xf
{
    struct xframe_expression_tag {};

    template <class CO, class... CI>
    void merge_to(CO& output, const CI&... input);

    /***************************
     * merge_to implementation *
     ***************************/

    namespace detail
    {
        template <class C0, class C1>
        inline void merge_containers(C0& output, const C1& input)
        {
            auto output_iter = output.begin();
            auto output_end = output.end();
            auto first = input.begin();
            auto last = input.end();
            while(first != last && output_iter != output_end)
            {
                if(*first < *output_iter)
                {
                    output_iter = output.insert(output_iter, *first++);
                    output_end = output.end();
                }
                else if(*first == *output_iter)
                {
                    ++first;
                }
                ++output_iter;
            }
            std::copy(first, last, std::back_inserter(output));
        }

        template <class C0>
        inline void merge_to_impl(C0&)
        {
        }

        template <class C0, class C1, class... C>
        inline void merge_to_impl(C0& out, C1& in, const C&... input)
        {
            merge_containers(out, in);
            merge_to_impl(out, input...);
        }
    }

    template <class CO, class... CI>
    inline void merge_to(CO& output, const CI&... input)
    {
        detail::merge_to_impl(output, input...);
    }
}

#endif
