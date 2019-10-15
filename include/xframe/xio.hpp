/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_IO_HPP
#define XFRAME_IO_HPP

#include <sstream>

#include "xtensor/xio.hpp"

#ifdef __CLING__

#include <nlohmann/json.hpp>

namespace nl = nlohmann;

namespace xf
{
    template <class P, class T>
    void compute_1d_row(std::stringstream& out, P& printer, const T& expr,
                        const std::size_t& row_idx)
    {
        const auto& dim_name = expr.dimension_mapping().labels()[0];
        std::stringstream label;
        xtl::visit([&label](auto&& arg) { label << arg; }, expr.coordinates()[dim_name].label(row_idx));

        out << "<tr><th title='" << dim_name << "'>" << label.str()
            << "</th><td style='font-family:monospace;' title='"
            << dim_name << ": " << label.str() << "'><pre>";
        printer.print_next(out);
        out << "</pre></td></tr>";
    }

    template <class P, class T>
    void compute_1d_table(std::stringstream& out, P& printer, const T& expr,
                          const std::size_t& edge_items)
    {
        const auto& dim = expr.shape()[0];

        out << "<table style='border-style:solid;border-width:1px;'><tbody>";
        if (edge_items == 0 || 2 * edge_items >= dim)
        {
            for (std::size_t row_idx = 0; row_idx < dim; ++row_idx)
            {
                compute_1d_row(out, printer, expr, row_idx);
            }
        }
        else
        {
            for (std::size_t row_idx = 0; row_idx < edge_items; ++row_idx)
            {
                compute_1d_row(out, printer, expr, row_idx);
            }
            out << "<tr><th><center>...</center></th><td><center>...</center></td></tr>";
            for (std::size_t row_idx = dim - edge_items; row_idx < dim; ++row_idx)
            {
                compute_1d_row(out, printer, expr, row_idx);
            }
        }
        out << "</tbody></table>";
    }

    template <class P, class T>
    void compute_2d_element(std::stringstream& out, P& printer, const T& expr,
                            const std::string& titles_str,
                            const std::size_t& row_idx, const std::size_t& column_idx)
    {
        const auto& before_last_dim_name = expr.dimension_mapping().label(expr.dimension() - 2);
        const auto& last_dim_name = expr.dimension_mapping().label(expr.dimension() - 1);

        out << "<td style='font-family:monospace;' title='(" << titles_str;
        xtl::visit([&before_last_dim_name, &out](auto&& arg) {
            out << before_last_dim_name << ": " << arg << ", ";
        }, expr.coordinates()[before_last_dim_name].label(row_idx));
        xtl::visit([&last_dim_name, &out](auto&& arg) {
            out << last_dim_name << ": " << arg << ")'>";
        }, expr.coordinates()[last_dim_name].label(column_idx));

        out << "<pre>";
        printer.print_next(out);
        out << "</pre></td>";
    }

    template <class T>
    std::size_t compute_nb_rows(const std::size_t& dim, const T& expr, const std::size_t& edge_items)
    {
        if (dim == expr.dimension() - 2)
        {
            if (edge_items == 0 || 2 * edge_items >= expr.shape()[dim])
            {
                return expr.shape()[dim];
            }
            /*else*/ return 2 * edge_items + 1;
        }

        if (edge_items == 0 || 2 * edge_items >= expr.shape()[dim])
        {
            return expr.shape()[dim] * compute_nb_rows(dim + 1, expr, edge_items);
        }
        /*else*/ return 2 * edge_items * compute_nb_rows(dim + 1, expr, edge_items) + 1;
    }

    template <class T, class I>
    void compute_row_head(std::stringstream& out, const T& expr,
                          const std::size_t& edge_items,
                          const std::size_t& row_idx, const std::vector<I>& idx)
    {
        const auto& before_last_dim_name = expr.dimension_mapping().label(expr.dimension() - 2);

        if (row_idx == 0)
        {
            std::stringstream heads;
            std::size_t current_dim = 0;
            for (auto it = idx.cbegin(); it != idx.cend(); ++it)
            {
                if (*(it + 1) == 0 || it + 1 == idx.cend())
                {
                    const auto& current_dim_name = expr.dimension_mapping().label(current_dim);

                    std::size_t rowspan = compute_nb_rows(current_dim + 1, expr, edge_items);
                    xtl::visit([&heads, &current_dim_name, &rowspan](auto&& arg) {
                        heads << "<th title='" << current_dim_name
                        << "' scope='rowgroup' rowspan='" << rowspan
                        << "'>" << arg << "</th>";
                    }, expr.coordinates()[current_dim_name].label(*it));
                }

                current_dim++;
            }
            out << heads.str();
        }

        xtl::visit([&out, &before_last_dim_name](auto&& arg) {
            out << "<th"
                << " style='border-right-style:solid;border-right-width:1px;'"
                <<" title='" << before_last_dim_name << "'>" << arg << "</th>";
        }, expr.coordinates()[before_last_dim_name].label(row_idx));
    }

    template <class P, class T, class I>
    void compute_2d_row(std::stringstream& out, P& printer, const T& expr,
                        const std::size_t& edge_items, const std::string& titles_str,
                        const std::size_t& row_idx, const std::vector<I>& idx)
    {
        const auto& before_last_dim_name = expr.dimension_mapping().label(expr.dimension() - 2);
        const auto& last_dim = expr.shape()[expr.dimension() - 1];

        out << "<tr>";
        compute_row_head(out, expr, edge_items, row_idx, idx);
        if (edge_items == 0 || 2 * edge_items >= last_dim)
        {
            for (std::size_t column_idx = 0; column_idx < last_dim; ++column_idx)
            {
                xf::compute_2d_element(out, printer, expr, titles_str, row_idx, column_idx);
            }
        }
        else
        {
            for (std::size_t column_idx = 0; column_idx < edge_items; ++column_idx)
            {
                xf::compute_2d_element(out, printer, expr, titles_str, row_idx, column_idx);
            }
            out << "<td><center>...</center></td>";
            for (std::size_t column_idx = last_dim - edge_items; column_idx < last_dim; ++column_idx)
            {
                xf::compute_2d_element(out, printer, expr, titles_str, row_idx, column_idx);
            }
        }
        out << "</tr>";
    }

    template <class P, class T, class I>
    void compute_2d_table(std::stringstream& out, P& printer, const T& expr,
                          const std::size_t& edge_items, const std::vector<I>& idx)
    {
        const auto& before_last_dim_name = expr.dimension_mapping().label(expr.dimension() - 2);
        const auto& before_last_dim = expr.shape()[expr.dimension() - 2];
        const auto& last_dim = expr.shape()[expr.dimension() - 1];

        std::stringstream titles;
        std::size_t current_dim = 0;
        std::for_each(idx.cbegin(), idx.cend(), [&titles, &expr, &current_dim](const auto& i) {
            const auto& dim_name = expr.dimension_mapping().label(current_dim);

            xtl::visit([&titles, &dim_name](auto&& arg) {
                titles << dim_name << ": " << arg << ", ";
            }, expr.coordinates()[dim_name].label(i));

            current_dim++;
        });
        std::string titles_str = titles.str();

        if (edge_items == 0 || 2 * edge_items >= before_last_dim)
        {
            for (std::size_t row_idx = 0; row_idx < before_last_dim; ++row_idx)
            {
                xf::compute_2d_row(out, printer, expr, edge_items, titles_str, row_idx, idx);
            }
        }
        else
        {
            for (std::size_t row_idx = 0; row_idx < edge_items; ++row_idx)
            {
                xf::compute_2d_row(out, printer, expr, edge_items, titles_str, row_idx, idx);
            }
            out << "<tr><th title='" << before_last_dim_name
                << "' style='border-right-style:solid;border-right-width:1px;'>"
                << "<center>...</center></th>";
            if (2 * edge_items >= last_dim)
            {
                for (std::size_t row_idx = 0; row_idx < last_dim; ++row_idx)
                {
                    out << "<td><center>...</center></td>";
                }
            }
            else
            {
                for (std::size_t column_idx = 0; column_idx < 2 * edge_items + 1; ++column_idx)
                {
                    out << "<td><center>...</center></td>";
                }
            }
            out << "</tr>";
            for (std::size_t row_idx = before_last_dim - edge_items; row_idx < before_last_dim; ++row_idx)
            {
                xf::compute_2d_row(out, printer, expr, edge_items, titles_str, row_idx, idx);
            }
        }
    }

    template <class P, class T, class I>
    void compute_sub_nd_table(std::stringstream& out, P& printer, const T& expr,
                              const std::size_t& edge_items, const std::vector<I>& idx)
    {
        const auto& dim = expr.dimension();
        const auto& displayed_dim = idx.size();
        const auto& displayed_dim_size = expr.shape()[displayed_dim];
        const auto& displayed_dim_name = expr.dimension_mapping().label(displayed_dim);

        if (dim - displayed_dim == 2)
        {
            return xf::compute_2d_table(out, printer, expr, edge_items, idx);
        }

        std::vector<I> idx2 = idx;
        idx2.resize(displayed_dim + 1);

        if (edge_items == 0 || 2 * edge_items >= displayed_dim_size)
        {
            for (std::size_t i = 0; i < displayed_dim_size; ++i)
            {
                idx2[displayed_dim] = i;
                xf::compute_sub_nd_table(out, printer, expr, edge_items, idx2);
            }
        }
        else
        {
            for (std::size_t i = 0; i < edge_items; ++i)
            {
                idx2[displayed_dim] = i;
                xf::compute_sub_nd_table(out, printer, expr, edge_items, idx2);
            }
            out << "<tr><th title='" << displayed_dim_name << "'><center>...</center></th>";
            for (std::size_t i = displayed_dim + 1; i < dim - 1; ++i)
            {
                const auto& i_name = expr.dimension_mapping().label(i);
                out << "<th title='" << i_name << "'><center>...</center></th>";
            }
            std::size_t num_ellipsis;
            if (edge_items == 0 || 2 * edge_items >= expr.shape()[expr.dimension() - 1])
            {
                num_ellipsis = expr.shape()[expr.dimension() - 1];
            }
            else
            {
                num_ellipsis = 2 * edge_items + 1;
            }
            for (std::size_t i = 0; i < num_ellipsis; ++i)
            {
                out << "<td><center>...</center></td>";
            }
            out << "</tr>";
            for (std::size_t i = displayed_dim_size - edge_items; i < displayed_dim_size; ++i)
            {
                idx2[displayed_dim] = i;
                xf::compute_sub_nd_table(out, printer, expr, edge_items, idx2);
            }
        }
    }

    template <class P, class T>
    void compute_nd_table_impl(std::stringstream& out, P& printer, const T& expr,
                               const std::size_t& edge_items)
    {
        const auto& dim = expr.dimension();

        out << "<table style='border-style:solid;border-width:1px;'>";

        // Compute head of the table
        out << "<thead>";
        for (std::size_t d = 0; d < dim - 1; ++d)
        {
            out << "<th></th>";
        }

        const auto& last_dim_name = expr.dimension_mapping().labels()[dim - 1];
        const auto& last_dim_size = expr.shape()[dim - 1];
        if (edge_items == 0 || 2 * edge_items >= last_dim_size)
        {
            for (std::size_t d = 0; d < last_dim_size; ++d)
            {
                xtl::visit([&out, &last_dim_name](auto&& arg) {
                    out << "<th title='" << last_dim_name << "'>" << arg << "</th>";
                }, expr.coordinates()[last_dim_name].label(d));
            }
        }
        else
        {
            for (std::size_t d = 0; d < edge_items; ++d)
            {
                xtl::visit([&out, &last_dim_name](auto&& arg) {
                    out << "<th title='" << last_dim_name << "'>" << arg << "</th>";
                }, expr.coordinates()[last_dim_name].label(d));
            }
            out << "<th title='" << last_dim_name << "'><center>...</center></th>";
            for (std::size_t d = last_dim_size - edge_items; d < last_dim_size; ++d)
            {
                xtl::visit([&out, &last_dim_name](auto&& arg) {
                    out << "<th title='" << last_dim_name << "'>" << arg << "</th>";
                }, expr.coordinates()[last_dim_name].label(d));
            }
        }
        out << "</thead>";

        // Compute body of the table
        out << "<tbody>";
        std::vector<std::size_t> empty_vector;
        xf::compute_sub_nd_table(out, printer, expr, edge_items, empty_vector);
        out << "</tbody>";
        out << "</table>";
    }

    template <class P, class T>
    void compute_nd_table(std::stringstream& out, P& printer, const T& expr,
                          const std::size_t& edge_items)
    {
        if (expr.dimension() == 1)
        {
            xf::compute_1d_table(out, printer, expr, edge_items);
        }
        else
        {
            xf::compute_nd_table_impl(out, printer, expr, edge_items);
        }
    }

    template <class T>
    nl::json mime_bundle_repr_impl(const T& expr)
    {
        std::stringstream out;

        std::size_t edge_items = 0;
        std::size_t size = xt::compute_size(expr.shape());
        if (size > xt::print_options::print_options().threshold)
        {
            edge_items = xt::print_options::print_options().edge_items;
        }

        if (xt::print_options::print_options().precision != -1)
        {
            out.precision(xt::print_options::print_options().precision);
        }

        xt::detail::printer<typename T::data_type> printer(out.precision());

        xt::xstrided_slice_vector slice_vector;
        xt::detail::recurser_run(printer, expr.data(), slice_vector, edge_items);
        printer.init();

        xf::compute_nd_table(out, printer, expr, edge_items);

        auto bundle = nl::json::object();
        bundle["text/html"] = out.str();
        return bundle;
    }

    template <class CCT, class ECT>
    class xvariable_container;

    template <class CCT, class ECT>
    nl::json mime_bundle_repr(const xvariable_container<CCT, ECT>& expr)
    {
        return xf::mime_bundle_repr_impl(expr);
    }

    template <class F, class R, class... CT>
    class xvariable_function;

    template <class F, class R, class... CT>
    nl::json mime_bundle_repr(const xvariable_function<F, R, CT...>& expr)
    {
        using temporary_type = typename xvariable_function<F, R, CT...>::temporary_type;
        temporary_type tmp(expr);
        return xf::mime_bundle_repr_impl(tmp);
    }

    template <class CT>
    class xvariable_view;

    template <class CT>
    nl::json mime_bundle_repr(const xvariable_view<CT>& expr)
    {
        return xf::mime_bundle_repr_impl(expr);
    }

    template <class CTV, class CTAX>
    class xvariable_masked_view;

    template <class CTV, class CTAX>
    nl::json mime_bundle_repr(const xvariable_masked_view<CTV, CTAX>& expr)
    {
        return xf::mime_bundle_repr_impl(expr);
    }

    template <class E>
    class xreindex_view;

    template <class E>
    nl::json mime_bundle_repr(const xreindex_view<E>& expr)
    {
        return xf::mime_bundle_repr_impl(expr);
    }
}
#endif

#endif
