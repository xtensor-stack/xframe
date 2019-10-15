/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XEXPAND_DIMS_VIEW_HPP
#define XFRAME_XEXPAND_DIMS_VIEW_HPP

#include <vector>
#include <utility>
#include <string>
#include <stdexcept>

#include "xtensor/xexpression.hpp"
#include "xtensor/xstrided_view.hpp"

#include "xaxis.hpp"
#include "xcoordinate_expanded.hpp"
#include "xvariable_base.hpp"
#include "xvariable.hpp"

namespace xf
{

    template <class CT>
    class xexpand_dims_view;

    template <class CT>
    struct xvariable_inner_types<xexpand_dims_view<CT>>
    {
        using xexpression_type = std::decay_t<CT>;
        using data_type = xt::xstrided_view<typename xexpression_type::data_type&, xt::svector<std::size_t>>;
        using data_closure_type = data_type;

        using subcoordinate_type = typename xexpression_type::coordinate_type;
        using coordinate_type = xcoordinate_expanded<subcoordinate_type>;
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
    struct xcontainer_inner_types<xf::xexpand_dims_view<CT>>
        : xf::xvariable_inner_types<xf::xexpand_dims_view<CT>>
    {
        using base_type = xf::xvariable_inner_types<xf::xexpand_dims_view<CT>>;

        using data_type = typename base_type::data_type;

        using temporary_coordinate_type = xf::xcoordinate<typename base_type::key_type,
                                                          typename base_type::label_list,
                                                          typename base_type::size_type>;
        using temporary_data_type = typename std::decay_t<CT>::temporary_type::data_type;
        using temporary_type = xf::xvariable_container<temporary_coordinate_type, temporary_data_type>;
    };
}

namespace xf
{

    /*********************
     * xexpand_dims_view *
     *********************/

    /**
     * @class xexpand_dims_view
     * @brief View on a variable with additional axes
     *
     * The xvariable_masked_view class is used for creating a view on a variable
     * with additional axes.
     *
     * @tparam CT the closure type on the underlying variable.
     */
    template <class CT>
    class xexpand_dims_view : public xt::xexpression<xexpand_dims_view<CT>>,
                              private xvariable_base<xexpand_dims_view<CT>>
    {
    public:

        using self_type = xexpand_dims_view<CT>;
        using base_type = xvariable_base<xexpand_dims_view<CT>>;
        using inner_types = xvariable_inner_types<xexpand_dims_view<CT>>;

        using xexpression_type = typename inner_types::xexpression_type;
        using data_type = typename inner_types::data_type;
        using value_type = typename xexpression_type::value_type;
        using reference = typename xexpression_type::const_reference;
        using const_reference = reference;
        using pointer = typename xexpression_type::const_pointer;
        using const_pointer = pointer;
        using size_type = typename xexpression_type::size_type;
        using difference_type = typename xexpression_type::difference_type;

        using shape_type = typename xexpression_type::shape_type;

        using coordinate_type = typename inner_types::coordinate_type;
        using dimension_type = typename inner_types::dimension_type;
        using dimension_list = typename dimension_type::label_list;
        using coordinate_map = typename coordinate_type::map_type;
        using key_type = typename coordinate_type::key_type;
        using label_list = typename coordinate_type::label_list;

        using expression_tag = xvariable_expression_tag;

        using extra_dimensions_type = std::vector<std::pair<typename xexpression_type::key_type, std::size_t>>;

        template <class E>
        xexpand_dims_view(E&& e, const extra_dimensions_type& dims);

        using base_type::missing;
        using base_type::shape;
        using base_type::size;
        using base_type::dimension;
        using base_type::data;
        using base_type::coordinates;
        using base_type::dimension_labels;
        using base_type::dimension_mapping;
        using base_type::broadcast_coordinates;
        using base_type::broadcast_dimensions;
        using base_type::operator();
        using base_type::element;
        using base_type::locate;
        using base_type::locate_element;
        using base_type::select;
        using base_type::iselect;

    private:

        template <class E>
        dimension_list init_dimension_mapping(E&& e, const extra_dimensions_type& dims) const noexcept;
        template <class E>
        coordinate_type init_coordinate(E&& e, const extra_dimensions_type& dims) const noexcept;
        data_type init_data(const extra_dimensions_type& dims) const noexcept;

        const data_type& data_impl() const noexcept;
        data_type& data_impl() noexcept;

        CT m_e;
        data_type m_data;

        friend class xvariable_base<self_type>;
    };

    template <class CT>
    std::ostream& operator<<(std::ostream& out, const xexpand_dims_view<CT>& view);

    /******************************
     * xexpand_dims_view builders *
     ******************************/

    template <class E, class K>
    auto expand_dims(E&& e, std::initializer_list<K> dim_names);

    template <class E>
    auto expand_dims(E&& e, std::initializer_list<std::pair<typename std::decay_t<E>::key_type, std::size_t>> dims);

    template <class E>
    auto expand_dims(E&& e, std::initializer_list<const char*> dim_names);

    /************************************
     * xexpand_dims_view implementation *
     ************************************/

    /**
     * Constructs an xexpand_dims_view given a map of extra dimensions
     * @param e the variable expression on which to create the view.
     * @param dims the map of extra dimensions, the key being the dimension name,
     * the value the position where to put the new dimension.
     */
    template <class CT>
    template <class E>
    inline xexpand_dims_view<CT>::xexpand_dims_view(E&& e, const extra_dimensions_type& dims)
        : base_type(init_coordinate(std::forward<E>(e), dims), init_dimension_mapping(std::forward<E>(e), dims)),
          m_e(std::forward<E>(e)),
          m_data(init_data(dims))
    {
    }

    template <class CT>
    template <class E>
    inline auto xexpand_dims_view<CT>::init_dimension_mapping(E&& e, const extra_dimensions_type& dims) const noexcept -> dimension_list
    {
        auto orig_dims_cp = e.dimension_mapping().labels();
        for (const auto& dim : dims)
        {
            orig_dims_cp.insert(orig_dims_cp.cbegin() + dim.second, dim.first);
        }
        return orig_dims_cp;
    }

    template <class CT>
    template <class E>
    inline auto xexpand_dims_view<CT>::init_coordinate(E&& e, const extra_dimensions_type& dims) const noexcept -> coordinate_type
    {
        coordinate_map extra_dims;
        for (const auto& dim : dims)
        {
            extra_dims.insert(std::make_pair(dim.first, ::xf::axis(1)));
        }
        return expand_dims(e.coordinates(), std::move(extra_dims));
    }

    template <class CT>
    inline auto xexpand_dims_view<CT>::init_data(const extra_dimensions_type& dims) const noexcept -> data_type
    {
        xt::xstrided_slice_vector sv(m_e.data().dimension() + dims.size(), xt::all());
        for (const auto& iter : dims)
        {
            sv[iter.second] = xt::newaxis();
        }
        return xt::strided_view(m_e.data(), std::move(sv));
    }

    template <class CT>
    inline auto xexpand_dims_view<CT>::data_impl() const noexcept -> const data_type&
    {
        return m_data;
    }

    template <class CT>
    inline auto xexpand_dims_view<CT>::data_impl() noexcept -> data_type&
    {
        return m_data;
    }

    /*********************************************
     * xexpand_dims_view builders implementation *
     *********************************************/

    template <class E, class K>
    inline auto expand_dims_impl(E&& e, std::initializer_list<K> dim_names)
    {
        using extra_dimensions_type = std::vector<std::pair<typename std::decay_t<E>::key_type, std::size_t>>;
        extra_dimensions_type extra_dimensions;
        for (const auto& dim_name : dim_names)
        {
            extra_dimensions.push_back(std::make_pair(dim_name, extra_dimensions.size()));
        }

        return xexpand_dims_view<xtl::closure_type_t<E>>(std::forward<E>(e), std::move(extra_dimensions));
    }

    /**
     * Creates a view on a variable expression with additional axes, given a list
     * of extra dimension names
     * @param e the variable expression on which to create the view.
     * @param dim_names the list of extra dimension names.
     */
    template <class E, class K>
    inline auto expand_dims(E&& e, std::initializer_list<K> dim_names)
    {
        return expand_dims_impl(std::forward<E>(e), dim_names);
    }

    /**
     * Creates a view on a variable expression with additional axes, given a list
     * of extra dimensions
     * @param e the variable expression on which to create the view.
     * @param dims the extra dimensions as a mapping "dimension name" -> "position".
     */
    template <class E>
    inline auto expand_dims(E&& e, std::initializer_list<std::pair<typename std::decay_t<E>::key_type, std::size_t>> dims)
    {
        return xexpand_dims_view<xtl::closure_type_t<E>>(std::forward<E>(e), dims);
    }

    template <class E>
    inline auto expand_dims(E&& e, std::initializer_list<const char*> dim_names)
    {
        return expand_dims_impl(std::forward<E>(e), dim_names);
    }

    template <class CT>
    inline std::ostream& operator<<(std::ostream& out, const xexpand_dims_view<CT>& view)
    {
        return print_variable_expression(out, view);
    }
}

#endif
