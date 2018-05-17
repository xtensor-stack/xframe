/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

// TODO: the content of that file should be moved to xtensor/xdynamic_view.hpp

#ifndef XTENSOR_XDYNAMIC_VIEW_HPP
#define XTENSOR_XDYNAMIC_VIEW_HPP

#include <map>
#include <xtl/xclosure.hpp>

#include "xtensor/xiterable.hpp"
#include "xtensor/xsemantic.hpp"
#include "xtensor/xtensor_forward.hpp"
#include "xtensor_xslice.hpp"

namespace xt
{
    template <class CT>
    class xdynamic_view;

    template <class CT>
    struct xcontainer_inner_types<xdynamic_view<CT>>
    {
        using xexpression_type = std::decay_t<CT>;
        using temporary_type = xarray<typename xexpression_type::value_type>;
    };

    template <class CT>
    struct xiterable_inner_types<xdynamic_view<CT>>
    {
        using xexpression_type = std::decay_t<CT>;
        using inner_shape_type = typename xexpression_type::shape_type;
    };

    template <class CT>
    class xdynamic_view
    {
    public:

        using self_type = xdynamic_view<CT>;
        using xexpression_type = std::decay_t<CT>;

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

        using iterable_base = xiterable_inner_types<self_type>;
        using inner_shape_type = typename iterable_base::inner_shape_type;
        using shape_type = inner_shape_type;

        using slice_type = xslice_variant<size_type>;
        using slice_list = std::vector<slice_type>;
        using squeeze_map = std::map<size_type, size_type>;

        static constexpr layout_type static_layout = layout_type::dynamic;
        static constexpr bool contiguous_layout = false;

        template <class CTA>
        xdynamic_view(CTA&& e, ) noexcept;

        template <class CTA>
        explicit xdynamic_view(CTA&& e, const slice_list& sl, const squeeze_map& squeeze) noexcept;

        xdynamic_view(const xdynamic_view&) = default;
        xdynamic_view& operator=(const xdynamic_view&);

        size_type dimension() const noexcept;

        size_type size() const noexcept;
        const inner_shape_type& shape() const noexcept;
        const slice_type& slices() const noexcept;
        layout_type layout() const noexcept;

        template <class... Args>
        reference operator()(Args... args);
        template <class... Args>
        reference at(Args... args);
        template <class OS>
        disable_integral_t<OS, reference> operator[](const OS& index);
        template <class I>
        reference operator[](std::initializer_list<I> index);
        reference operator[](size_type i);
        template <class It>
        reference element(It first, It last);

        template <class... Args>
        const_reference operator()(Args... args) const;
        template <class... Args>
        const_reference at(Args... args) const;
        template <class OS>
        disable_integral_t<OS, const_reference> operator[](const OS& index) const;
        template <class I>
        const_reference operator[](std::initializer_list<I> index) const;
        const_reference operator[](size_type i) const;
        template <class It>
        const_reference element(It first, It last) const;

    private:

        using index_type = shape_type;

        shape_type compute_shape() const noexcept;

        template <std::size_t... I, class... Args>
        reference access_impl(std::index_sequence<I...>, Args... args);

        template <std::size_t... I, class Args>
        const_reference access_impl(std::index_sequence<I...>, Args... args) const;

        template <class... Args>
        index_type build_index(Args... args) const noexcept;

        template <std::size_t I, class T, class Args...>
        void fill_index(index_type& index, T idx, Args... args) const noexcept;

        template <std::size_t I>
        void fill_index(index_type& index) const noexcept;

        template <class It>
        index_type build_element_index(It first, It last) const noexcept;

        CT m_e;
        slice_list m_slice;
        squeeze_map m_squeeze;
        inner_shape_type m_shape;
    };

    /********************************
     * xdynamic_view implementation *
     ********************************/

    template <class CT>
    template <class CTA>
    inline xdynamic_view<CT>::xdynamic_view(CTA&& e, slice_list&& sl, squeeze_map&& squeeze) noexcept
        : m_e(std::forward<E>(e)),
          m_slice(std::move(sl)),
          m_squeeze(std::move(squeeze)),
          m_shape(compute_shape())
    {
    }

    template <class CT>
    template <class CTA>
    inline xdynamic_view<CT>::xdynamic_view(CTA&& e, const slice_list& sl, const squeeze_map& squeeze) noexcept
        : m_e(std::forward<E>(e)),
          m_slice(sl),
          m_squeeze(squeeze),
          m_shape(compute_shape())
    {
    }

    template <class CT>
    inline auto xdynamic_view<CT>::dimension() const noexcept -> size_type
    {
        return m_shape.size();
    }

    template <class CT>
    inline auto xdynamic_view<CT>::size() const noexcept -> size_type
    {
        return compute_size(shape());
    }

    template <class CT>
    inline auto xdynamic_view<CT>::shape() const noexcept -> const inner_shape_type&
    {
        return m_shape;
    }

    template <class CT>
    inline auto xdynamic_view<CT>::slices() const noexcept -> const slice_type&
    {
        return m_slice;
    }

    template <class CT>
    inline auto xdynamic_view<CT>::layout() const noexcept -> layout_type
    {
        return layout_type::dynamic;
    }

    template <class CT>
    template <class... Args>
    inline auto xdynamic_view<CT>::operator()(Args... args) -> reference
    {
        if (m_squeeze.empty())
        {
            return access_impl(std::make_index_sequence<sizeof...(Args)>(), args...);
        }
        else
        {
            auto idx = build_index(std::forward<Args>(args)...);
            return m_e.element(idx);
        }
    }

    template <class CT>
    template <class... Args>
    inline auto xdynamic_view<CT>::at(Args... args) -> reference
    {
        check_access(shape(), static_cast<size_type>(args)...);
        return this->operator()(args...);
    }

    template <class CT>
    template <class OS>
    inline auto xdynamic_view<CT>::operator[](const OS& index) -> disable_integral_t<OS, reference>
    {
        return element(index.cbegin(), index.cend());
    }

    template <class CT>
    template <class I>
    inline auto xdynamic_view<CT>::operator[](std::initializer_list<I> index) -> reference
    {
        return element(index.begin(), index.end());
    }

    template <class CT>
    inline auto xdynamic_view<CT>::operator[](size_type i) -> reference
    {
        return operator()(i);
    }

    template <class CT>
    template <class It>
    inline auto xdynamic_view<CT>::element(It first, It last) -> reference
    {
        auto idx = build_element_index(first, last);
        return m_e.element(idx.cbegin(), idx.cend());
    }

    template <class CT>
    template <class... Args>
    inline auto xdynamic_view<CT>::operator()(Args... args) const -> const_reference
    {
        if (m_squeeze.empty())
        {
            return access_impl(std::make_index_sequence<sizeof...(Args)>(), args...);
        }
        else
        {
            auto idx = build_index(std::forward<Args>(args)...);
            return m_e.element(idx);
        }
    }

    template <class CT>
    template <class... Args>
    inline auto xdynamic_view<CT>::at(Args... args) const -> const_reference
    {
        check_access(shape(), static_cast<size_type>(args)...);
        return this->operator()(args...);
    }

    template <class CT>
    template <class OS>
    inline auto xdynamic_view<CT>::operator[](const OS& index) const -> disable_integral_t<OS, const_reference>
    {
        return element(index.cbegin(), index.cend());
    }

    template <class CT>
    template <class I>
    inline auto xdynamic_view<CT>::operator[](std::initializer_list<I> index) const -> const_reference
    {
        return element(index.begin(), index.end());
    }

    template <class CT>
    inline auto xdynamic_view<CT>::operator[](size_type i) const -> const_reference
    {
        return operator()(i);
    }

    template <class CT>
    template <class It>
    inline auto xdynamic_view<CT>::element(It first, It last) const -> const_reference
    {
        auto idx = build_element_index(first, last);
        return m_e.element(idx.cbegin(), idx.cend());
    }

    template <class CT>
    inline auto xdynamic_view<CT>::compute_shape() const noexcept-> shape_type
    {
        shape_type sh(m_slice.size());
        std::transform(m_slice.cbegin(), m_slice.cend(), sh.begin(),
            [](const auto& arg) { return arg.size(); });
        return sh;
    }

    template <class CT>
    template <std::size_t... I, class... Args>
    inline auto xdynamic_view<CT>::access_impl(std::index_sequence<I...>, Args... args) -> reference
    {
        return m_e(m_slice[I](args)...);
    }

    template <class CT>
    template <std::size_t... I, class Args>
    inline auto xdynamic_view<CT>::access_impl(std::index_sequence<I...>, Args... args) const -> const_reference
    {
        return m_e(m_slice[I](args)...);
    }

    template <class CT>
    template <class... Args>
    inline auto xdynamic_view<CT>::build_index(Args... args) const noexcept -> index_type
    {
        index_type idx = xtl::make_sequence<index_type>(m_e.dimension());
        fill_index<0>(idx, size_type(0), args...);
        return idx;
    }

    template <class CT>
    template <std::size_t I, class T, class Args...>
    inline void xdynamic_view<CT>::fill_index(index_type& index, T idx, Args... args) const noexcept
    {
        auto iter = m_squeeze.find(I);
        if (iter != m_squeeze.end())
        {
            index[I] = iter->second;
            fill_index[I + 1](idx, args...);
        }
        else
        {
            index[I] = idx;
            fill_index(args...);
        }
    }

    template <class CT>
    template <std::size_t I>
    inline void xdynamic_view<CT>::fill_index(index_type& index) const noexcept
    {
        for (size_type i = I; i < index.size(); ++i)
        {
            auto iter = m_squeeze.find(i);
            index[i] = iter->second;
        }
    }

    template <class CT>
    template <class It>
    inline auto xdynamic_view<CT>::build_element_index(It first, It last) const noexcept -> index_type
    {
        index_type idx = xtl::make_sequence<index_type>(m_e.dimension());
        size_type i = 0;
        while (i != idx.size() && first != last)
        {
            auto iter = m_squeeze.find(i);
            if (iter != m_squeeze.end())
            {
                idx[i] = iter->second;
            }
            else
            {
                idx[i] = *first++;
            }
            ++i;
        }
        while (i != idx.size())
        {
            idx[i] = m_squeeze.find(i)->second;
            ++i;
        }
        return idx;
    }
}

#endif