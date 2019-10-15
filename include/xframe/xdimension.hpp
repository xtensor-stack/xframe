/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XDIMENSION_HPP
#define XFRAME_XDIMENSION_HPP

#include "xaxis.hpp"

namespace xf
{
    class xfull_coordinate;

    /**************
     * xdimension *
     **************/

    /**
     * @class xdimension
     * @brief Class modeling dimensions
     *
     * The xdimension class is used for modeling the mapping of dimension names
     * to their positions in a data tensor. This class is a special axis with
     * a broadcast method instead of merge and intersect, thus its API is really
     * close the one of \c xaxis.
     *
     * @tparam L the type of dimension name.
     * @tparam T the integer type use to represent the positions of the dimensions.
     *           Default value is \c std::size_t.
     * @sa xaxis
     */
    template <class L, class T = std::size_t>
    class xdimension : private xaxis<L, T, map_tag>
    {
    public:

        using self_type = xdimension<L, T>;
        using base_type = xaxis<L, T, map_tag>;
        using label_list = typename base_type::label_list;
        using key_type = typename base_type::key_type;
        using mapped_type = typename base_type::mapped_type;
        using map_type = typename base_type::map_type;
        using value_type = typename base_type::value_type;
        using reference = typename base_type::reference;
        using const_reference = typename base_type::const_reference;
        using pointer = typename base_type::pointer;
        using const_pointer = typename base_type::const_pointer;
        using size_type = typename base_type::size_type;
        using difference_type = typename base_type::difference_type;
        using iterator = typename base_type::iterator;
        using const_iterator = typename base_type::const_iterator;
        using reverse_iterator = typename base_type::reverse_iterator;
        using const_reverse_iterator = typename base_type::const_reverse_iterator;

        xdimension();
        explicit xdimension(const label_list& labels);
        explicit xdimension(label_list&& labels);
        xdimension(std::initializer_list<key_type> init);

        template <class InputIt>
        xdimension(InputIt first, InputIt last);

        template <class... Args>
        bool broadcast(const Args&... dims);

        using base_type::labels;
        using base_type::label;
        using base_type::empty;
        using base_type::size;
        using base_type::contains;
        using base_type::operator[];
        using base_type::find;
        using base_type::begin;
        using base_type::end;
        using base_type::cbegin;
        using base_type::cend;
        using base_type::rbegin;
        using base_type::rend;
        using base_type::crbegin;
        using base_type::crend;

    private:

        template <class... Args>
        bool broadcast_impl(const self_type& a, const Args&... dims);

        template <class... Args>
        bool broadcast_impl(const xfull_coordinate& a, const Args&... dims);

        bool broadcast_impl();

        template <class... Args>
        bool broadcast_empty(const self_type& a, const Args&... dims);

        template <class... Args>
        bool broadcast_empty(const xfull_coordinate& a, const Args&... dims);

        bool broadcast_empty();
    };

    template <class L, class T>
    bool operator==(const xdimension<L, T>& lhs, const xdimension<L, T>& rhs) noexcept;

    template <class L, class T>
    bool operator!=(const xdimension<L, T>& lhs, const xdimension<L, T>& rhs) noexcept;

    template <class OS, class L, class T>
    OS& operator<<(OS& out, const xdimension<L, T>& dim);

    template <class L, class T, class... Args>
    bool broadcast_dimensions(xdimension<L, T>& output, const Args&... dims);

    /**********************
     * xdimension builder *
     **********************/

    template <class T = std::size_t, class L>
    xdimension<L, T> dimension(std::initializer_list<L> init) noexcept;

    template <class L = fstring, class T = std::size_t>
    xdimension<L, T> dimension(std::initializer_list<const char*> init) noexcept;

    /*****************************
     * is_dimension metafunction *
     *****************************/

    namespace detail
    {
        template <class T>
        struct is_dimension_impl : std::false_type
        {
        };

        template <class L, class T>
        struct is_dimension_impl<xdimension<L, T>> : std::true_type
        {
        };

        template <class L>
        struct is_dimension_list_impl : std::false_type
        {
        };

        template <class L>
        struct is_dimension_list_impl<std::vector<L>>
            : std::true_type
        {
        };
    }

    template <class T>
    struct is_dimension : detail::is_dimension_impl<std::decay_t<T>>
    {
    };

    template <class T>
    struct is_dimension_list : detail::is_dimension_list_impl<std::decay_t<T>>
    {
    };

    /*****************************
     * xdimension implementation *
     *****************************/

    /**
     * Constructs an empty xdimension object.
     */
    template <class L, class T>
    inline xdimension<L, T>::xdimension()
        : base_type()
    {
    }

    /**
     * Constructs an xdimension object with the given list of dimension
     * labels. This list is copied.
     * @param labels the list of dimension names.
     */
    template <class L, class T>
    inline xdimension<L, T>::xdimension(const label_list& labels)
        : base_type(labels)
    {
    }

    /**
     * Constructs an xdimension object with the given list of dimension
     * labels. This list is moved and therefore it is invalid after the
     * xdimension object has been constructed.
     * @param labels the list of dimension names.
     */
    template <class L, class T>
    inline xdimension<L, T>::xdimension(label_list&& labels)
        : base_type(std::move(labels))
    {
    }

    /**
     * Constructs an xdimension object from the given initializer list of
     * dimension names.
     * @param init the list of dimension names.
     */
    template <class L, class T>
    inline xdimension<L, T>::xdimension(std::initializer_list<key_type> init)
        : base_type(init)
    {
    }

    /**
     * Constructs an xdimension object from the content of the range [first, last)
     * @param first An iterator to the first dimension name.
     * @param last An iterator the the element following the last dimension name.
     */
    template <class L, class T>
    template <class InputIt>
    inline xdimension<L, T>::xdimension(InputIt first, InputIt last)
        : base_type(first, last)
    {
    }


    /**
     * Broadcast the specified dimensions to this xdimension object.
     * @param dims the xdimension objects to broadcast.
     * @return true if the dimension objects are the same.
     */
    template <class L, class T>
    template <class... Args>
    inline bool xdimension<L, T>::broadcast(const Args&... dims)
    {
        return this->empty() ? broadcast_empty(dims...) : broadcast_impl(dims...);
    }

    template <class L, class T>
    template <class... Args>
    inline bool xdimension<L, T>::broadcast_impl(const self_type& a, const Args&... dims)
    {
        XFRAME_TRACE_BROADCAST_DIMENSIONS(*this, a);
        bool res = base_type::merge_unsorted(true, a.labels());
        XFRAME_TRACE_DIMENSIONS_RESULT(*this, res);
        res &= broadcast_impl(dims...);
        return res;
    }

    template <class L, class T>
    template <class... Args>
    inline bool xdimension<L, T>::broadcast_impl(const xfull_coordinate& /*a*/, const Args&... dims)
    {
        return broadcast_impl(dims...);
    }

    template <class L, class T>
    inline bool xdimension<L, T>::broadcast_impl()
    {
        return true;
    }

    template <class L, class T>
    template <class... Args>
    bool xdimension<L, T>::broadcast_empty(const self_type& a, const Args&... dims)
    {
        base_type::set_labels(a.labels());
        return broadcast_impl(dims...);
    }

    template <class L, class T>
    template <class... Args>
    bool xdimension<L, T>::broadcast_empty(const xfull_coordinate& /*a*/, const Args&... dims)
    {
        return broadcast_empty(dims...);
    }

    template <class L, class T>
    bool xdimension<L, T>::broadcast_empty()
    {
        return true;
    }

    /**
     * Returns true if \c lhs and \c rhs are equivalent dimension mappings, i.e. they contain
     * the same dimension name - dimension position pairs.
     * @param lhs an xdimension object.
     * @param rhs an xdimension obejct.
     */
    template <class L, class T>
    inline bool operator==(const xdimension<L, T>& lhs, const xdimension<L, T>& rhs) noexcept
    {
        return lhs.labels() == rhs.labels();
    }

    /**
     * Returns true if \c lhs and \c rhs are not equivalent dimension mappings, i.e. they contain
     * different dimension name - dimension position pairs.
     * @param lhs an xdimension object.
     * @param rhs an xdimension obejct.
     */
    template <class L, class T>
    inline bool operator!=(const xdimension<L, T>& lhs, const xdimension<L, T>& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    template <class OS, class L, class T>
    inline OS& operator<<(OS& out, const xdimension<L, T>& dim)
    {
        using iterator = std::ostream_iterator<L, typename OS::char_type, typename OS::traits_type>;
        out << '(';
        std::copy(dim.labels().begin(), dim.labels().end(), iterator(out, ", "));
        out << ')';
        return out;
    }

    template <class L, class T, class... Args>
    inline bool broadcast_dimensions(xdimension<L, T>& output, const Args&... dims)
    {
        return output.broadcast(dims...);
    }

    /**************************************
     * xdimension builders implementation *
     **************************************/

    template <class T, class L>
    inline xdimension<L, T> dimension(std::initializer_list<L> init) noexcept
    {
        return xdimension<L, T>(init);
    }

    template <class L, class T>
    inline xdimension<L, T> dimension(std::initializer_list<const char*> init) noexcept
    {
        return xdimension<L, T>(init.begin(), init.end());
    }
}

#endif
