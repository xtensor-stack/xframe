/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_HPP
#define XFRAME_XAXIS_HPP

#include <initializer_list>
#include <iterator>
#include <algorithm>
#include <map>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "xtl/xiterator_base.hpp"

#include "xtensor/xbuilder.hpp"

#include "xaxis_base.hpp"
#include "xframe_utils.hpp"

namespace xf
{
    template <class L, class T, class MT>
    class xaxis_iterator;

    template <class L, class T>
    class xaxis_default;

    /*********************
     * map container tag *
     *********************/

    struct map_tag {};
    struct hash_map_tag {};

    template <class K, class T, class MT>
    struct map_container;

    template <class K, class T>
    struct map_container<K, T, map_tag>
    {
        using type = std::map<K, T>;
    };

    template <class K, class T>
    struct map_container<K, T, hash_map_tag>
    {
        using type = std::unordered_map<K, T>;
    };

    template <class K, class T, class MT>
    using map_container_t = typename map_container<K, T, MT>::type;

    /*********
     * xaxis *
     *********/

    /**
     * @class xaxis
     * @brief Class modeling an axis in a coordinate system.
     *
     * The xaxis class is used for modeling general axes; an axis is a mapping
     * of labels to positions in a given dimension. It is the equivalent of
     * the \c Index object from <a href="pandas.pydata.org">pandas</a>.
     *
     * @tparam L the type of labels.
     * @tparam T the integer type used to represent positions. Default value is
     *           \c std::size_t.
     * @tparam MT the tag used for choosing the map type which holds the label-
     *            position pairs. Possible values are \c map_tag and \c hash_map_tag.
     *            Default value is \c hash_map_tag.
     */
    template <class L, class T = std::size_t, class MT = hash_map_tag>
    class xaxis : public xaxis_base<xaxis<L, T, MT>>
    {
    public:

        using base_type = xaxis_base<xaxis>;
        using self_type = xaxis<L, T, MT>;
        using key_type = typename base_type::key_type;
        using label_list = typename base_type::label_list;
        using mapped_type = typename base_type::mapped_type;
        using map_type = map_container_t<key_type, mapped_type, MT>;
        using value_type = typename map_type::value_type;
        using reference = typename map_type::const_reference;
        using const_reference = typename map_type::const_reference;
        using pointer = typename map_type::const_pointer;
        using const_pointer = typename map_type::const_pointer;
        using size_type = typename base_type::size_type;
        using difference_type = typename base_type::difference_type;
        using iterator = typename base_type::iterator;
        using const_iterator = typename base_type::const_iterator;
        using reverse_iterator = typename base_type::reverse_iterator;
        using const_reverse_iterator = typename base_type::const_reverse_iterator;

        explicit xaxis();
        explicit xaxis(const label_list& labels);
        explicit xaxis(label_list&& labels);
        xaxis(std::initializer_list<key_type> init);

        template <class L1>
        explicit xaxis(xaxis_default<L1, T> axis);

        template <class InputIt>
        xaxis(InputIt first, InputIt last);

        bool is_sorted() const noexcept;

        bool contains(const key_type& key) const;
        mapped_type operator[](const key_type& key) const;

        template <class F>
        self_type filter(const F& f) const noexcept;

        template <class F>
        self_type filter(const F& f, size_type size) const noexcept;

        const_iterator find(const key_type& key) const;

        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;

        template <class... Args>
        bool merge(const Args&... axes);

        template <class... Args>
        bool intersect(const Args&... axes);

    protected:

        void populate_index();
        void set_labels(const label_list& labels);

        template <class Arg, class... Args>
        bool merge_unsorted(bool broadcasting, const Arg& a, const Args&... axes);
        bool merge_unsorted(bool broadcasting);

        template <class Arg, class... Args>
        bool intersect_unsorted(const Arg& al, const Args&... axes_labels);
        bool intersect_unsorted();

    private:

        xaxis(const label_list& labels, bool is_sorted);
        xaxis(label_list&& labels, bool is_sorted);

        typename map_type::const_iterator find_index(const key_type& key) const;

        template <class... Args>
        bool merge_impl(const Args&... axes);

        template <class Arg1, class... Args>
        bool merge_empty(const Arg1& a, const Args&... axes);
        bool merge_empty();

        bool init_is_sorted() const noexcept;

        template <class Arg, class... Args>
        bool all_sorted(const Arg& a, const Args&... axes) const noexcept;

        template <class Arg>
        bool all_sorted(const Arg& a) const noexcept;

        map_type m_index;
        bool m_is_sorted;

        friend class xaxis_iterator<L, T, MT>;
        friend class xaxis_default<L, T>;
    };

    template <class L, class T, class MT, class... Args>
    bool merge_axes(xaxis<L, T, MT>& output, const Args&... axes);

    template <class L, class T, class MT, class... Args>
    bool intersect_axes(xaxis<L, T, MT>& output, const Args&... axes);

    /******************
     * xaxis builders *
     ******************/

    template <class T = std::size_t, class L>
    xaxis<L, T> axis(L start, L stop, L step = 1) noexcept;

    template <class T = std::size_t, class L>
    xaxis<L, T> axis(std::initializer_list<L> init) noexcept;

    template <class T = std::size_t>
    xaxis<XFRAME_STRING_LABEL, T> axis(std::initializer_list<const char*> init) noexcept;

    /********************
    * xaxis_inner_types *
    *********************/

    template <class L, class T, class MT>
    struct xaxis_inner_types<xaxis<L, T, MT>>
    {
        using key_type = L;
        using mapped_type = T;
        using iterator = xaxis_iterator<L, T, MT>;
    };

    /******************
     * xaxis_iterator *
     ******************/

    template <class L, class T, class MT>
    class xaxis_iterator : public xtl::xrandom_access_iterator_base<xaxis_iterator<L, T, MT>,
                                                                    typename xaxis<L, T, MT>::value_type,
                                                                    typename xaxis<L, T, MT>::difference_type,
                                                                    typename xaxis<L, T, MT>::const_pointer,
                                                                    typename xaxis<L, T, MT>::const_reference>
    {

    public:

        using self_type = xaxis_iterator<L, T, MT>;
        using container_type = xaxis<L, T, MT>;
        using label_list = typename container_type::label_list;
        using label_iterator = typename label_list::const_iterator;
        using value_type = typename container_type::value_type;
        using reference = typename container_type::const_reference;
        using pointer = typename container_type::const_pointer;
        using difference_type = typename container_type::difference_type;
        using iterator_category = std::random_access_iterator_tag;

        xaxis_iterator() = default;
        xaxis_iterator(const container_type* c, label_iterator it);

        self_type& operator++();
        self_type& operator--();

        self_type& operator+=(difference_type n);
        self_type& operator-=(difference_type n);

        difference_type operator-(const self_type& rhs) const;

        reference operator*() const;
        pointer operator->() const;

        bool equal(const self_type& rhs) const noexcept;
        bool less_than(const self_type& rhs) const noexcept;

    private:

        const container_type* p_c;
        label_iterator m_it;
    };

    template <class L, class T, class MT>
    typename xaxis_iterator<L, T, MT>::difference_type operator-(const xaxis_iterator<L, T, MT>& lhs, const xaxis_iterator<L, T, MT>& rhs);

    template <class L, class T, class MT>
    bool operator==(const xaxis_iterator<L, T, MT>& lhs, const xaxis_iterator<L, T, MT>& rhs) noexcept;

    template <class L, class T, class MT>
    bool operator<(const xaxis_iterator<L, T, MT>& lhs, const xaxis_iterator<L, T, MT>& rhs) noexcept;

    /************************
     * xaxis implementation *
     ************************/

    /**
     * @name Constructors
     */
    //@{
    /**
     * Constructs an empty axis.
     */
    template <class L, class T, class MT>
    inline xaxis<L, T, MT>::xaxis()
        : base_type(), m_index(), m_is_sorted(true)
    {
    }

    /**
     * Constructs an axis with the given list of labels. This list
     * is copied and the constructor internally checks whether it
     * is sorted.
     * @param labels the list of labels.
     */
    template <class L, class T, class MT>
    inline xaxis<L, T, MT>::xaxis(const label_list& labels)
        : base_type(labels), m_index(), m_is_sorted()
    {
        m_is_sorted = init_is_sorted();
        populate_index();
    }

    /**
     * Constructs an axis with the given list of labels. The list is
     * moved and therefore it is invalid after the axis has been
     * constructed. The constructor internally checks whether the list
     * is sorted.
     * @param labels the list of labels.
     */
    template <class L, class T, class MT>
    inline xaxis<L, T, MT>::xaxis(label_list&& labels)
        : base_type(std::move(labels)), m_index(), m_is_sorted()
    {
        m_is_sorted = init_is_sorted();
        populate_index();
    }

    /**
     * Constructs an axis with the given list of labels, and a boolean
     * specifying if the labels list is sorted. This is an optimization
     * that prevents the constructor to check if the labels list is sorted.
     * The list is copied.
     * @param labels th list of labels.
     * @param is_sorted a boolean parameter indicating if the labels list
     *                  is sorted.
     */
    template <class L, class T, class MT>
    inline xaxis<L, T, MT>::xaxis(const label_list& labels, bool is_sorted)
        : base_type(labels), m_index(), m_is_sorted(is_sorted)
    {
        populate_index();
    }
    /**
     * Constructs an axis with the given list of labels, and a boolean
     * specifying if the labels list is sorted. This is an optimization
     * that prevents the constructor to check if the labels list is sorted.
     * The list is moved and therefore invalid after the axis has been
     * constructed.
     * @param labels th list of labels.
     * @param is_sorted a boolean parameter indicating if the labels list
     *                  is sorted.
     */

    template <class L, class T, class MT>
    inline xaxis<L, T, MT>::xaxis(label_list&& labels, bool is_sorted)
        : base_type(labels), m_index(), m_is_sorted(is_sorted)
    {
        populate_index();
    }

    /**
     * Constructs an axis from the given initializer list of labels. The
     * constructor internally checks whether the list is sorted.
     */
    template <class L, class T, class MT>
    inline xaxis<L, T, MT>::xaxis(std::initializer_list<key_type> init)
        : base_type(init), m_index(), m_is_sorted()
    {
        m_is_sorted = init_is_sorted();
        populate_index();
    }

    /**
     * Constructs an axis from a \c default_axis.
     * @sa default_axis
     */
    template <class L, class T, class MT>
    template <class L1>
    inline xaxis<L, T, MT>::xaxis(xaxis_default<L1, T> axis)
        : base_type(axis.labels()), m_index(), m_is_sorted(true)
    {
        static_assert(std::is_same<L, L1>::value, "key_type L and key_type L1 must be the same");

        populate_index();
    }

    /**
     * Constructs an axis from the content of the range [first, last)
     * @param first An iterator to the first label.
     * @param last An iterator the the element following the last label.
     */
    template <class L, class T, class MT>
    template <class InputIt>
    inline xaxis<L, T, MT>::xaxis(InputIt first, InputIt last)
        : base_type(first, last), m_index(), m_is_sorted()
    {
        m_is_sorted = init_is_sorted();
        populate_index();
    }
    //@}

    /**
     * Returns true if the labels list is sorted.
     */
    template <class L, class T, class MT>
    inline bool xaxis<L, T, MT>::is_sorted() const noexcept
    {
        return m_is_sorted;
    }

    /**
      * @name Data
     */
    //@{
    /**
     * Returns true if the axis contains the speficied label.
     * @param key the label to search for.
     */
    template <class L, class T, class MT>
    inline bool xaxis<L, T, MT>::contains(const key_type& key) const
    {
        return m_index.count(key) != typename map_type::size_type(0);
    }

    /**
     * Returns the position of the specified label. If this last one is
     * not found, an exception is thrown.
     * @param key the label to search for.
     */
    template <class L, class T, class MT>
    inline auto xaxis<L, T, MT>::operator[](const key_type& key) const -> mapped_type
    {
        return m_index.at(key);
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
    inline auto xaxis<L, T, MT>::filter(const F& f) const noexcept -> self_type
    {
        return self_type(base_type::filter_labels(f), m_is_sorted);
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
    inline auto xaxis<L, T, MT>::filter(const F& f, size_type size) const noexcept -> self_type
    {
        return self_type(base_type::filter_labels(f, size), m_is_sorted);
    }
    //@}

    /**
     * @name Iterator
     */
    //@{
    /**
     * Returns a constant iterator to the element with label equivalent to \c key. If
     * no such element is found, past-the-end iterator is returned.
     * @param key the label to search for.
     */
    template <class L, class T, class MT>
    inline auto xaxis<L, T, MT>::find(const key_type& key) const -> const_iterator
    {
        auto map_iter = m_index.find(key);
        return map_iter != m_index.end() ? cbegin() + map_iter->second : cend();
    }

    /**
     * Returns a constant iterator to the first element of the axis.
     * This element is a pair label - position.
     */
    template <class L, class T, class MT>
    inline auto xaxis<L, T, MT>::cbegin() const noexcept -> const_iterator
    {
        return const_iterator(this, this->labels().begin());
    }

    /**
     * Returns a constant iterator to the element following the last element
     * of the axis.
     */
    template <class L, class T, class MT>
    inline auto xaxis<L, T, MT>::cend() const noexcept -> const_iterator
    {
        return const_iterator(this, this->labels().end());
    }
    //@}

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
    inline bool xaxis<L, T, MT>::merge(const Args&... axes)
    {
        return this->empty() ? merge_empty(axes...) : merge_impl(axes...);
    }

    /**
     * Replaces the labels with the intersection of the labels of
     * the axes arguments and the labels of this axis.
     * @param axes the axes to intersect.
     * @return true if the intersection is equivalent to this axis.
     */
    template <class L, class T, class MT>
    template <class... Args>
    inline bool xaxis<L, T, MT>::intersect(const Args&... axes)
    {
        bool res = true;
        if (all_sorted(*this, axes...))
        {
            res = intersect_to(this->mutable_labels(), axes.labels()...);
            populate_index();
        }
        else
        {
            res = intersect_unsorted(axes.labels()...);
        }
        return res;
    }
    //@}

    template <class L, class T, class MT>
    inline void xaxis<L, T, MT>::populate_index()
    {
        for(size_type i = 0; i < this->labels().size(); ++i)
        {
            m_index[this->labels()[i]] = T(i);
        }
    }

    template <class L, class T, class MT>
    void xaxis<L, T, MT>::set_labels(const label_list& labels)
    {
        this->mutable_labels() = labels;
        populate_index();
    }

    template <class L, class T, class MT>
    inline auto xaxis<L, T, MT>::find_index(const key_type& key) const -> typename map_type::const_iterator
    {
        return m_index.find(key);
    }

    template <class L, class T, class MT>
    template <class... Args>
    inline bool xaxis<L, T, MT>::merge_impl(const Args&... axes)
    {
        bool res = true;
        if(all_sorted(*this, axes...))
        {
            res = merge_to(this->mutable_labels(), axes.labels()...);
            populate_index();
        }
        else
        {
            m_is_sorted = false;
            if (m_index.empty())
            {
                populate_index();
            }
            res = merge_unsorted(false, axes.labels()...);
        }
        return res;
    }

    template <class L, class T, class MT>
    template <class Arg1, class... Args>
    inline bool xaxis<L, T, MT>::merge_empty(const Arg1& a, const Args&... axes)
    {
        this->mutable_labels() = a.labels();
        return merge_impl(axes...);
    }

    template <class L, class T, class MT>
    inline bool xaxis<L, T, MT>::merge_empty()
    {
        return true;
    }

    template <class L, class T, class MT>
    inline bool xaxis<L, T, MT>::init_is_sorted() const noexcept
    {
        return std::is_sorted(this->labels().begin(), this->labels().end());
    }

    template <class L, class T, class MT>
    template <class Arg, class... Args>
    inline bool xaxis<L, T, MT>::all_sorted(const Arg& a, const Args&... axes) const noexcept
    {
        return a.is_sorted() && all_sorted(axes...);
    }

    template <class L, class T, class MT>
    template <class Arg>
    inline bool xaxis<L, T, MT>::all_sorted(const Arg& a) const noexcept
    {
        return a.is_sorted();
    }

    template <class L, class T, class MT>
    template <class Arg, class... Args>
    inline bool xaxis<L, T, MT>::merge_unsorted(bool broadcasting, const Arg& a, const Args&... axes_labels)
    {
        bool res = merge_unsorted(broadcasting, axes_labels...);
        auto& labels = this->mutable_labels();
        auto output_iter = labels.rbegin();
        auto output_end = labels.rend();
        auto input_iter = a.rbegin();
        auto input_end = a.rend();
        while ((output_iter != output_end) && (input_iter != input_end) && (*output_iter == *input_iter))
        {
            ++output_iter;
            ++input_iter;
        }
        if(input_iter == input_end)
        {
            if(output_iter != output_end)
            {
                res &= broadcasting;
            }
        }
        else if(output_iter == output_end)
        {
            std::copy(a.begin(), a.begin() + std::distance(input_iter, input_end),
                      std::inserter(labels, labels.begin()));
            populate_index();
            res &= broadcasting;
        }
        else
        {
            while(input_iter != input_end)
            {
                if(!contains(*input_iter))
                {
                    if(output_iter != labels.rbegin())
                    {
                        labels.insert(labels.begin(), *input_iter);
                    }
                    else
                    {
                        labels.push_back(*input_iter);
                    }
                }
                ++input_iter;
            }
            populate_index();
            res = false;
        }
        return res;
    }

    template <class L, class T, class MT>
    inline bool xaxis<L, T, MT>::merge_unsorted(bool /*broadcasting*/)
    {
        return true;
    }

    template <class L, class T, class MT>
    template <class Arg, class... Args>
    inline bool xaxis<L, T, MT>::intersect_unsorted(const Arg& al, const Args&... axes_labels)
    {
        bool res = intersect_unsorted(axes_labels...);
        auto& labels = this->mutable_labels();
        auto iter = labels.begin();
        auto iter_end = labels.end();
        bool must_populate = false;
        while (iter != iter_end)
        {
            auto it = std::find(al.begin(), al.end(), *iter);
            if (it == al.end())
            {
                iter = labels.erase(iter, iter + 1);
                iter_end = labels.end();
                res = false;
                must_populate = true;
            }
            else
            {
                if (it - al.begin() != iter - labels.begin())
                {
                    res = false;
                }
                ++iter;
            }
        }
        if (must_populate)
        {
            populate_index();
        }
        return res;
    }

    template <class L, class T, class MT>
    inline bool xaxis<L, T, MT>::intersect_unsorted()
    {
        return true;
    }

    template <class L, class T, class MT, class... Args>
    inline bool merge_axes(xaxis<L, T, MT>& output, const Args&... axes)
    {
        return output.merge(axes...);
    }

    template <class L, class T, class MT, class... Args>
    inline bool intersect_axes(xaxis<L, T, MT>& output, const Args&... axes)
    {
        return output.intersect(axes...);
    }

    /*********************************
     * xaxis_iterator implementation *
     *********************************/

    template <class L, class T, class MT>
    inline xaxis_iterator<L, T, MT>::xaxis_iterator(const container_type* c, label_iterator it)
        : p_c(c), m_it(it)
    {
    }

    template <class L, class T, class MT>
    inline auto xaxis_iterator<L, T, MT>::operator++() -> self_type&
    {
        ++m_it;
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_iterator<L, T, MT>::operator--() -> self_type&
    {
        --m_it;
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_iterator<L, T, MT>::operator+=(difference_type n) -> self_type&
    {
        m_it += n;
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_iterator<L, T, MT>::operator-=(difference_type n) -> self_type&
    {
        m_it -= n;
        return *this;
    }

    template <class L, class T, class MT>
    inline auto xaxis_iterator<L, T, MT>::operator-(const self_type& rhs) const -> difference_type
    {
        return m_it - rhs.m_it;
    }

    template <class L, class T, class MT>
    inline auto xaxis_iterator<L, T, MT>::operator*() const -> reference
    {
        return *(p_c->find_index(*m_it));
    }

    template <class L, class T, class MT>
    inline auto xaxis_iterator<L, T, MT>::operator->() const -> pointer
    {
        return &(*(p_c->find_index(*m_it)));
    }

    template <class L, class T, class MT>
    inline bool xaxis_iterator<L, T, MT>::equal(const self_type& rhs) const noexcept
    {
        return m_it == rhs.m_it;
    }

    template <class L, class T, class MT>
    inline bool xaxis_iterator<L, T, MT>::less_than(const self_type& rhs) const noexcept
    {
        return m_it < rhs.m_it;
    }

    template <class L, class T, class MT>
    inline typename xaxis_iterator<L, T, MT>::difference_type operator-(const xaxis_iterator<L, T, MT>& lhs, const xaxis_iterator<L, T, MT>& rhs)
    {
        return lhs.operator-(rhs);
    }

    template <class L, class T, class MT>
    inline bool operator==(const xaxis_iterator<L, T, MT>& lhs, const xaxis_iterator<L, T, MT>& rhs) noexcept
    {
        return lhs.equal(rhs);
    }

    template <class L, class T, class MT>
    inline bool operator<(const xaxis_iterator<L, T, MT>& lhs, const xaxis_iterator<L, T, MT>& rhs) noexcept
    {
        return lhs.less_than(rhs);
    }

    /********************************
     * axis builders implementation *
     ********************************/

    /**
     * Returns an axis containing a range of integral labels.
     * @param start the first value of the range.
     * @param stop the end of the range. The range doe snot contain
     *             this value.
     * @param step Spacing between values. Default step is \c 1.
     * @tparam T the integral type used for positions. Default value
     * @tparam L the type of the labels.
     */
    template <class T, class L>
    inline xaxis<L, T> axis(L start, L stop, L step) noexcept
    {
        auto range = xt::arange(start, stop, step);
        return xaxis<L, T>(range.begin(), range.end());
    }

    /**
     * Builds an returns an axis from the specified list of labels.
     * @param init the list of labels.
     * @tparam T the integral type used for positions. Default value
     *           is \c std::size_t.
     * @tparam L the type of the labels.
     */
    template <class T, class L>
    inline xaxis<L, T> axis(std::initializer_list<L> init) noexcept
    {
        return xaxis<L, T>(init);
    }

    template <class T>
    inline xaxis<XFRAME_STRING_LABEL, T> axis(std::initializer_list<const char*> init) noexcept
    {
        return xaxis<XFRAME_STRING_LABEL, T>(init.begin(), init.end());
    }
}

#endif
