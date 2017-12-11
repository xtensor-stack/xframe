/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XFRAME_UTILS_HPP
#define XFRAME_XFRAME_UTILS_HPP

#include <iterator>

namespace xf
{
    template <class CO, class... CI>
    bool merge_to(CO& output, const CI&... input);

    template <class CO, class... CI>
    bool intersect_to(CO& output, const CI&... input);

    /***************************
     * merge_to implementation *
     ***************************/

    namespace detail
    {
        template <class C0, class C1>
        inline bool merge_containers(C0& output, const C1& input)
        {
            bool res = !(input.size() < output.size());
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
                    res = false;
                }
                else if(*first == *output_iter)
                {
                    ++first;
                }
                ++output_iter;
            }
            res &= output.empty() || (first == last);
            std::copy(first, last, std::back_inserter(output));
            return res;
        }

        template <class C0>
        inline bool merge_to_impl(C0&)
        {
            return true;
        }

        template <class C0, class C1, class... C>
        inline bool merge_to_impl(C0& out, C1& in, const C&... input)
        {
            bool res = merge_containers(out, in);
            res &= merge_to_impl(out, input...);
            return res;
        }
    }

    template <class CO, class... CI>
    inline bool merge_to(CO& output, const CI&... input)
    {
        return detail::merge_to_impl(output, input...);
    }

    /*******************************
     * intersect_to implementation *
     *******************************/

    namespace detail
    {
        template <class C0, class C1>
        inline bool intersect_containers(C0& output, const C1& input)
        {
            bool res = !(input.size() < output.size());
            auto output_iter = output.begin();
            auto output_end = output.end();
            auto first = input.begin();
            auto last = input.end();
            while(first != last && output_iter != output_end)
            {
                if(*first < *output_iter)
                {
                    ++first;
                }
                else if(*first == *output_iter)
                {
                    ++first;
                    ++output_iter;
                }
                else // *first > *output_iter
                {
                    output_iter = output.erase(output_iter);
                    output_end = output.end();
                    res = false;
                }
            }
            res &= (output_iter == output_end);
            output.erase(output_iter, output_end);
            return res;
        }

        template <class C0>
        inline bool intersect_to_impl(C0&)
        {
            return true;
        }

        template <class C0, class C1, class... C>
        inline bool intersect_to_impl(C0& out, const C1& in, const C&... input)
        {
            bool res = intersect_containers(out, in);
            res &= intersect_to_impl(out, input...);
            return res;
        }
    }

    template <class CO, class... CI>
    inline bool intersect_to(CO& output, const CI&... input)
    {
        return detail::intersect_to_impl(output, input...);
    }
}

#endif
