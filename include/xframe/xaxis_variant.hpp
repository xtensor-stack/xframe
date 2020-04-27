/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_VARIANT_HPP
#define XFRAME_XAXIS_VARIANT_HPP

#include <functional>
#include "xtl/xclosure.hpp"
#include "xtl/xmeta_utils.hpp"
#include "xtl/xvariant.hpp"
#include "xaxis.hpp"
#include "xaxis_default.hpp"
#include "xvector_variant.hpp"

namespace xf
{

    namespace detail
    {
        template <class V, class S, class... L>
        struct add_default_axis;

        template <class... A, class S>
        struct add_default_axis<xtl::variant<A...>, S>
        {
            using type = xtl::variant<A...>;
        };

        template <class... A, class S, class L1, class... L>
        struct add_default_axis<xtl::variant<A...>, S, L1, L...>
        {
            using type = typename xtl::mpl::if_t<std::is_integral<L1>,
                add_default_axis<xtl::variant<A..., xaxis_default<L1, S>>, S, L...>,
                add_default_axis<xtl::variant<A...>, S, L...>>::type;
        };

        template <class V, class S, class... L>
        using add_default_axis_t = typename add_default_axis<V, S, L...>::type;

        template <class V>
        struct get_axis_variant_iterator;

        template <class... A>
        struct get_axis_variant_iterator<xtl::variant<A...>>
        {
            using type = xtl::variant<typename A::const_iterator...>;
        };

        template <class V>
        using get_axis_variant_iterator_t = typename get_axis_variant_iterator<V>::type;

        template <class S, class MT, class TL>
        struct xaxis_variant_traits;

        template <class S, class MT, template <class...> class TL, class... L>
        struct xaxis_variant_traits<S, MT, TL<L...>>
        {
            using tmp_storage_type = xtl::variant<xaxis<L, S, MT>...>;
            using storage_type = add_default_axis_t<tmp_storage_type, S, L...>;
            using label_list = xvector_variant_cref<std::vector<L>...>;
            using key_type = xtl::variant<typename xaxis<L, S, MT>::key_type...>;
            using key_reference = xtl::variant<xtl::xclosure_wrapper<const typename xaxis<L, S, MT>::key_type&>...>;
            using mapped_type = S;
            using value_type = std::pair<key_type, mapped_type>;
            using reference = std::pair<key_reference, mapped_type&>;
            using const_reference = std::pair<key_reference, const mapped_type&>;
            using pointer = xtl::xclosure_pointer<reference>;
            using const_pointer = xtl::xclosure_pointer<const_reference>;
            using size_type = typename label_list::size_type;
            using difference_type = typename label_list::difference_type;
            using subiterator = get_axis_variant_iterator_t<storage_type>;
        };
    }

    template <class L, class T, class MT>
    class xaxis_variant_iterator;

    /*****************
     * xaxis_variant *
     *****************/

    /**
     * @class xaxis_variant
     * @brief Axis whose label type is a variant
     *
     * The xaxis_variant holds a variant of axes with different label types. It
     * provides the same API as a regular axis, wrapping the visitor mechanism
     * required to access the underlying axis. This allows to store axes with
     * different label types in a coordinate system.
     *
     * @tparam L the type list of labels
     * @tparam T the integer type used to represent positions.
     * @tparam MT the tag used for choosing the map type which holds the label-
     *            position pairs. Possible values are \c map_tag and \c hash_map_tag.
     *            Default value is \c hash_map_tag.
     */
    template <class L, class T, class MT = hash_map_tag>
    class xaxis_variant
    {
    public:

        static_assert(std::is_integral<T>::value, "index_type must be an integral type");

        using self_type = xaxis_variant<L, T, MT>;
        using map_container_tag = MT;
        using traits_type = detail::xaxis_variant_traits<T, MT, L>;
        using storage_type = typename traits_type::storage_type;
        using key_type = typename traits_type::key_type;
        using key_reference = typename traits_type::key_reference;
        using mapped_type = T;
        using label_list = typename traits_type::label_list;
        using value_type = typename traits_type::value_type;
        using reference = typename traits_type::reference;
        using const_reference = typename traits_type::const_reference;
        using pointer = typename traits_type::pointer;
        using const_pointer = typename traits_type::const_pointer;
        using size_type = typename traits_type::size_type;
        using difference_type = typename traits_type::difference_type;
        using iterator = xaxis_variant_iterator<L, T, MT>;
        using const_iterator = iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using subiterator = typename traits_type::subiterator;

        xaxis_variant() = default;
        template <class LB>
        xaxis_variant(const xaxis<LB, T, MT>& axis);
        template <class LB>
        xaxis_variant(xaxis<LB, T, MT>&& axis);
        template <class LB>
        xaxis_variant(const xaxis_default<LB, T>& axis);
        template <class LB>
        xaxis_variant(xaxis_default<LB, T>&& axis);

        label_list labels() const;
        key_type label(size_type i) const;

        bool empty() const;
        size_type size() const;

        bool is_sorted() const noexcept;

        bool contains(const key_type& key) const;
        mapped_type operator[](const key_type& key) const;

        template <class F>
        self_type filter(const F& f) const;

        template <class F>
        self_type filter(const F& f, size_type size) const;

        const_iterator find(const key_type& key) const;

        const_iterator begin() const;
        const_iterator end() const;

        const_iterator cbegin() const;
        const_iterator cend() const;

        const_reverse_iterator rbegin() const;
        const_reverse_iterator rend() const;

        const_reverse_iterator crbegin() const;
        const_reverse_iterator crend() const;

        template <class... Args>
        bool merge(const Args&... axes);

        template <class... Args>
        bool intersect(const Args&... axes);

        self_type as_xaxis() const;

        bool operator==(const self_type& rhs) const;
        bool operator!=(const self_type& rhs) const;

    private:

        storage_type m_data;

        template <class OS, class L1, class T1, class MT1>
        friend OS& operator<<(OS&, const xaxis_variant<L1, T1, MT1>&);
    };

    template <class OS, class L, class T, class MT>
    OS& operator<<(OS& out, const xaxis_variant<L, T, MT>& axis);

    /**************************
     * xaxis_variant_iterator *
     **************************/

    template <class L, class T, class MT>
    class xaxis_variant_iterator : public xtl::xrandom_access_iterator_base<xaxis_variant_iterator<L, T, MT>,
                                                                            typename xaxis_variant<L, T, MT>::value_type,
                                                                            typename xaxis_variant<L, T, MT>::difference_type,
                                                                            typename xaxis_variant<L, T, MT>::const_pointer,
                                                                            typename xaxis_variant<L, T, MT>::const_reference>
    {
    public:

        using self_type = xaxis_variant_iterator<L, T, MT>;
        using container_type = xaxis_variant<L, T, MT>;
        using key_reference = typename container_type::key_reference;
        using value_type = typename container_type::value_type;
        using reference = typename container_type::const_reference;
        using pointer = typename container_type::const_pointer;
        using difference_type = typename container_type::difference_type;
        using iterator_category = std::random_access_iterator_tag;
        using subiterator = typename container_type::subiterator;

        xaxis_variant_iterator() = default;
        xaxis_variant_iterator(subiterator it);

        self_type& operator++();
        self_type& operator--();

        self_type& operator+=(difference_type n);
        self_type& operator-=(difference_type n);

        difference_type operator-(const self_type& rhs) const;

        reference operator*() const;
        pointer operator->() const;

        bool equal(const self_type& rhs) const;
        bool less_than(const self_type& rhs) const;

    private:

        subiterator m_it;
    };

    template <class L, class T, class MT>
    typename xaxis_variant_iterator<L, T, MT>::difference_type operator-(const xaxis_variant_iterator<L, T, MT>& lhs,
                                                                         const xaxis_variant_iterator<L, T, MT>& rhs);

    template <class L, class T, class MT>
    bool operator==(const xaxis_variant_iterator<L, T, MT>& lhs, const xaxis_variant_iterator<L, T, MT>& rhs);

    template <class L, class T, class MT>
    bool operator<(const xaxis_variant_iterator<L, T, MT>& lhs, const xaxis_variant_iterator<L, T, MT>& rhs);

    /********************************
     * xaxis_variant implementation *
     ********************************/

    /**
     * @name Constructors
     */
    //@{
    /**
     * Constructs an xaxis_variant from the specified xaxis. This latter is copied
     * in the variant.
     * @tparam LB the label type of the axis argument.
     * @param axis the axis to copy in the variant.
     */
    template <class L, class T, class MT>
    template <class LB>
    inline xaxis_variant<L, T, MT>::xaxis_variant(const xaxis<LB, T, MT>& axis)
        : m_data(axis)
    {
    }

    /**
     * Constructs an xaxis_variant from the specified xaxis. This latter is moved in
     * the variant.
     * @tparam LB the label type of the axis argument.
     * @param axis the axis to move in the variant.
     */
    template <class L, class T, class MT>
    template <class LB>
    inline xaxis_variant<L, T, MT>::xaxis_variant(xaxis<LB, T, MT>&& axis)
        : m_data(std::move(axis))
    {
    }

    /**
     * Constructs an xaxis_variant from the specified xaxis_default. This latter is
     * copied in the variant.
     * @tparam LB the label type of the axis argument.
     * @param axis the axis to copy in the variant.
     */
    template <class L, class T, class MT>
    template <class LB>
    inline xaxis_variant<L, T, MT>::xaxis_variant(const xaxis_default<LB, T>& axis)
        : m_data(axis)
    {
    }

    /**
     * Constructs an xaxis_variant from the specified xaxis_default. This latter
     * is moved in the variant.
     * @tparam LB the label type of the axis argument.
     * @param axis the axis to move in the variant.
     */
    template <class L, class T, class MT>
    template <class LB>
    inline xaxis_variant<L, T, MT>::xaxis_variant(xaxis_default<LB, T>&& axis)
        : m_data(std::move(axis))
    {
    }

    //@}

    /**
     * @name Labels
     */
    //@{
    /**
     * Returns the list of labels contained in the axis.
     */
    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::labels() const -> label_list
    {
        return xtl::visit([](auto&& arg) -> label_list { return arg.labels(); }, m_data);
    };

    /**
     * Return the i-th label of the axis.
     * @param i the position of the label.
     */
    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::label(size_type i) const -> key_type
    {
        return xtl::visit([i](auto&& arg) -> key_type { return arg.labels()[i]; }, m_data);
    }

    /**
     * Checks if the axis has no labels.
     */
    template <class L, class T, class MT>
    inline bool xaxis_variant<L, T, MT>::empty() const
    {
        return xtl::visit([](auto&& arg) { return arg.empty(); }, m_data);
    }

    /**
     * Returns the number of labels in the axis.
     */
    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::size() const -> size_type
    {
        return xtl::visit([](auto&& arg) { return arg.size(); }, m_data);
    }

    /**
     * Returns true if the labels list is sorted.
     */
    template <class L, class T, class MT>
    inline bool xaxis_variant<L, T, MT>::is_sorted() const noexcept
    {
        return xtl::visit([](auto&& arg) { return arg.is_sorted(); }, m_data);
    }
    //@}

    /**
      * @name Data
     */
    //@{
    /**
     * Returns true if the axis contains the speficied label.
     * @param key the label to search for.
     */
    template <class L, class T, class MT>
    inline bool xaxis_variant<L, T, MT>::contains(const key_type& key) const
    {
        auto lambda = [&key](auto&& arg) -> bool
        {
            using type = typename std::decay_t<decltype(arg)>::key_type;
            return arg.contains(xtl::get<type>(key));
        };
        return xtl::visit(lambda, m_data);
    }

    /**
     * Returns the position of the specified label. If this last one is
     * not found, an exception is thrown.
     * @param key the label to search for.
     */
    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::operator[](const key_type& key) const -> mapped_type
    {
        auto lambda = [&key](auto&& arg) -> mapped_type
        {
            using type = typename std::decay_t<decltype(arg)>::key_type;
            return arg[xtl::get<type>(key)];
        };
        return xtl::visit(lambda, m_data);
    }
    //@}

    /**
     * @name Filters
     */
    //@{
    /**
     * Builds an return a new axis by applying the given filter to the axis.
     * @param f the filter used to select the labels to keep in the new axis.
     */
    template <class L, class T, class MT>
    template <class F>
    inline auto xaxis_variant<L, T, MT>::filter(const F& f) const -> self_type
    {
        return xtl::visit([&f](const auto& arg) { return self_type(arg.filter(f)); }, m_data);
    }

    /**
     * Builds an return a new axis by applying the given filter to the axis. When
     * the size of the new list of labels is known, this method allows some
     * optimizations compared to the previous one.
     * @param f the filter used to select the labels to keep in the new axis.
     * @param size the size of the new label list.
     */
    template <class L, class T, class MT>
    template <class F>
    inline auto xaxis_variant<L, T, MT>::filter(const F& f, size_type size) const -> self_type
    {
        return xtl::visit([&f, size](const auto& arg) { return self_type(arg.filter(f, size)); }, m_data);
    }
    //@}

    /**
     * @name Iterators
     */
    //@{
    /**
     * Returns a constant iterator to the element with label equivalent to \c key. If
     * no such element is found, past-the-end iterator is returned.
     * @param key the label to search for.
     */
    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::find(const key_type& key) const -> const_iterator
    {
        auto lambda = [&key](auto&& arg) -> const_iterator
        {
            using type = typename std::decay_t<decltype(arg)>::key_type;
            return subiterator(arg.find(xtl::get<type>(key)));
        };
        return xtl::visit(lambda, m_data);
    }

    /**
     * Returns a constant iterator to the first element of the axis.
     * This element is a pair label - position.
     */
    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::begin() const -> const_iterator
    {
        return cbegin();
    }

    /**
     * Returns a constant iterator to the element following the last element
     * of the axis.
     */
    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::end() const -> const_iterator
    {
        return cend();
    }

    /**
     * Returns a constant iterator to the first element of the axis.
     * This element is a pair label - position.
     */
    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::cbegin() const -> const_iterator
    {
        return xtl::visit([](auto&& arg) { return subiterator(arg.cbegin()); }, m_data);
    }

    /**
     * Returns a constant iterator to the element following the last element
     * of the axis.
     */
    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::cend() const -> const_iterator
    {
        return xtl::visit([](auto&& arg) { return subiterator(arg.cend()); }, m_data);
    }

    /**
     * Returns a constant iterator to the first element of the reverse axis.
     * This element is a pair labal - position.
     */
    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::rbegin() const -> const_reverse_iterator
    {
        return crbegin();
    }

    /**
     * Returns a constant iterator to the element following the last element
     * of the reversed axis.
     */
    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::rend() const -> const_reverse_iterator
    {
        return crend();
    }

    /**
     * Returns a constant iterator to the first element of the reverse axis.
     * This element is a pair labal - position.
     */
    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::crbegin() const -> const_reverse_iterator
    {
        return xtl::visit([](auto&& arg) { return subiterator(arg.cend()); }, m_data);
    }

    /**
     * Returns a constant iterator to the element following the last element
     * of the reversed axis.
     */
    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::crend() const -> const_reverse_iterator
    {
        return xtl::visit([](auto&& arg) { return subiterator(arg.cbegin()); }, m_data);
    }
    //@}

    template <class L, class T, class MT, class K>
    struct xaxis_variant_adaptor
    {
        using axis_variant_type = xaxis_variant<L, T, MT>;
        using key_type = K;
        using axis_type =  xaxis<K, T, MT>;
        using label_list = typename axis_type::label_list;

        xaxis_variant_adaptor(const axis_variant_type& axis)
            : m_axis(axis)
        {
        };

        inline const label_list& labels() const
        {
            return xget_vector<std::vector<key_type>>(m_axis.labels());
        };

        inline bool is_sorted() const noexcept
        {
            return m_axis.is_sorted();
        };

    private:

        const axis_variant_type& m_axis;
    };

    /**
     * @name Set operations
     */
    //@{
    /**
     * Merges all the axes arguments into this ones. After this function call,
     * the axis contains all the labels from all the arguments.
     * @param axes the axes to merge.
     * @return true is the axis already contained all the labels.
     */
    template <class L, class T, class MT>
    template <class... Args>
    inline bool xaxis_variant<L, T, MT>::merge(const Args&... axes)
    {
        auto lambda = [&axes...](auto&& arg) -> bool
        {
            using key_type = typename std::decay_t<decltype(arg)>::key_type;
            return arg.merge(xaxis_variant_adaptor<L, T, MT, key_type>(axes)...);
        };
        return xtl::visit(lambda, m_data);
    }

    /**
     * Replaces the labels with the intersection of the labels of
     * the axes arguments and the labels of this axis.
     * @param axes the axes to intersect.
     * @return true if the intersection is equivalent to this axis.
     */
    template <class L, class T, class MT>
    template <class... Args>
    inline bool xaxis_variant<L, T, MT>::intersect(const Args&... axes)
    {
        auto lambda = [&axes...](auto&& arg) -> bool
        {
            using key_type = typename std::decay_t<decltype(arg)>::key_type;
            return arg.intersect(xaxis_variant_adaptor<L, T, MT, key_type>(axes)...);
        };
        return xtl::visit(lambda, m_data);
    }
    //@}

    template <class L, class T, class MT>
    inline auto xaxis_variant<L, T, MT>::as_xaxis() const -> self_type
    {
        return xtl::visit([](auto&& arg) { return self_type(xaxis<typename std::decay_t<decltype(arg)>::key_type, T, MT>(arg)); }, m_data);
    }

    /**
     * Returns true is this axis and \c rhs are equivalent axes, i.e. they contain the same
     * label - position pairs.
     * @param rhs an axis.
     */
    template <class L, class T, class MT>
    inline bool xaxis_variant<L, T, MT>::operator==(const self_type& rhs) const
    {
        return m_data == rhs.m_data;
    }

    /**
     * Returns true is this axis and \c rhs are not equivalent axes, i.e. they contain different
     * label - position pairs.
     * @param rhs an axis.
     */
    template <class L, class T, class MT>
    inline bool xaxis_variant<L, T, MT>::operator!=(const self_type& rhs) const
    {
        return m_data != rhs.m_data;
    }

    template <class OS, class L, class T, class MT>
    inline OS& operator<<(OS& out, const xaxis_variant<L, T, MT>& axis)
    {
        xtl::visit([&out](auto&& arg) { out << arg; }, axis.m_data);
        return out;
    }

    /*****************************************
     * xaxis_variant_iterator implementation *
     *****************************************/

    template<class L, class T, class MT>
    inline xaxis_variant_iterator<L, T, MT>::xaxis_variant_iterator(subiterator it)
        : m_it(it)
    {
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant_iterator<L, T, MT>::operator++() -> self_type&
    {
        xtl::visit([](auto&& arg) { ++arg; }, m_it);
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant_iterator<L, T, MT>::operator--() -> self_type&
    {
        xtl::visit([](auto&& arg) { --arg; }, m_it);
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant_iterator<L, T, MT>::operator+=(difference_type n) -> self_type&
    {
        xtl::visit([n](auto&& arg) { arg += n; }, m_it);
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant_iterator<L, T, MT>::operator-=(difference_type n) -> self_type&
    {
        xtl::visit([n](auto&& arg) { arg -= n; }, m_it);
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant_iterator<L, T, MT>::operator-(const self_type& rhs) const -> difference_type
    {
        xtl::visit([&rhs](auto&& arg) { return arg - std::get<std::decay_t<decltype(arg)>>(rhs); }, m_it);
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant_iterator<L, T, MT>::operator*() const -> reference
    {
        return xtl::visit([](auto&& arg)
        {
            return reference(key_reference(xtl::closure(arg->first)), arg->second);
        }, m_it);
    }

    template <class L, class T, class MT>
    inline auto xaxis_variant_iterator<L, T, MT>::operator->() const -> pointer
    {
        return xtl::visit([](auto&& arg)
        {
            return pointer(reference(key_reference(xtl::closure(arg->first)), arg->second));
        }, m_it);
    }

    template <class L, class T, class MT>
    inline bool xaxis_variant_iterator<L, T, MT>::equal(const self_type& rhs) const
    {
        return m_it == rhs.m_it;
    }

    template <class L, class T, class MT>
    inline bool xaxis_variant_iterator<L, T, MT>::less_than(const self_type& rhs) const
    {
        return m_it < rhs.m_it;
    }

    template <class L, class T, class MT>
    inline auto operator-(const xaxis_variant_iterator<L, T, MT>& lhs, const xaxis_variant_iterator<L, T, MT>& rhs)
        -> typename xaxis_variant_iterator<L, T, MT>::difference_type
    {
        return lhs.operator-(rhs);
    }

    template <class L, class T, class MT>
    inline bool operator==(const xaxis_variant_iterator<L, T, MT>& lhs, const xaxis_variant_iterator<L, T, MT>& rhs)
    {
        return lhs.equal(rhs);
    }

    template <class L, class T, class MT>
    inline bool operator<(const xaxis_variant_iterator<L, T, MT>& lhs, const xaxis_variant_iterator<L, T, MT>& rhs)
    {
        return lhs.less_than(rhs);
    }

    template <class LB, class L, class T, class MT>
    auto get_labels(const xaxis_variant<L, T, MT>& axis_variant) -> const typename xaxis<LB, T, MT>::label_list&
    {
        using label_list = typename xaxis<LB, T, MT>::label_list;
        return xtl::xget<const label_list&>(axis_variant.labels().storage());
    }
}

#endif
