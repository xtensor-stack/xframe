/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_BASE_HPP
#define XFRAME_XAXIS_BASE_HPP

#include <algorithm>
#include <iterator>
#include <vector>

namespace xf
{

    template <class D>
    struct xaxis_inner_types;

    /**************
     * xaxis_base *
     **************/

    /**
     * @class xaxis_base
     * @brief Base class for axes.
     *
     * The xaxis_base class defines the common interface for axes, which define the
     * mapping of labels to positions in a given dimension. The axis_base class
     * embeds the list of labels only, the mapping is hold by the inheriting classes.
     *
     * @tparam D The derived type, i.e. the inheriting class for which xaxis_base
     *           provides the interface.
     */
    template <class D>
    class xaxis_base
    {
    public:

        using derived_type = D;
        using inner_types = xaxis_inner_types<D>;

        using key_type = typename inner_types::key_type;
        using mapped_type = typename inner_types::mapped_type;
        using label_list = std::vector<key_type>;
        using size_type = typename label_list::size_type;
        using difference_type = typename label_list::difference_type;
        using iterator = typename inner_types::iterator;
        using const_iterator = iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = reverse_iterator;

        static_assert(std::is_integral<mapped_type>::value, "mapped_type T must be an integral type");

        derived_type& derived_cast() & noexcept;
        const derived_type& derived_cast() const & noexcept;
        derived_type derived_cast() && noexcept;

        const label_list& labels() const noexcept;
        key_type label(size_type i) const;

        bool empty() const noexcept;
        size_type size() const noexcept;

        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;

        const_reverse_iterator rbegin() const noexcept;
        const_reverse_iterator rend() const noexcept;

        const_reverse_iterator crbegin() const noexcept;
        const_reverse_iterator crend() const noexcept;

    protected:

        xaxis_base();
        xaxis_base(const label_list& labels);
        xaxis_base(label_list&& labels);
        xaxis_base(std::initializer_list<key_type> init);

        template <class InputIt>
        xaxis_base(InputIt first, InputIt last);

        ~xaxis_base() = default;

        xaxis_base(const xaxis_base&) = default;
        xaxis_base& operator=(const xaxis_base&) = default;

        xaxis_base(xaxis_base&&) = default;
        xaxis_base& operator=(xaxis_base&&) = default;

        label_list& mutable_labels() noexcept;

        template <class F>
        label_list filter_labels(const F& f) const noexcept;

        template <class F>
        label_list filter_labels(const F& f, size_type size) const noexcept;

        label_list m_labels;
    };

    template <class D1, class D2>
    bool operator==(const xaxis_base<D1>& lhs, const xaxis_base<D2>& rhs) noexcept;

    template <class D1, class D2>
    bool operator!=(const xaxis_base<D1>& lhs, const xaxis_base<D2>& rhs) noexcept;

    template <class OS, class D>
    OS& operator<<(OS& out, const xaxis_base<D>& axis);

    /************************
     * is_axis metafunction *
     ************************/

    template <class T>
    struct is_axis : std::is_base_of<xaxis_base<T>, T>
    {
    };

    /*****************************
     * xaxis_base implementation *
     *****************************/

    template <class D>
    inline xaxis_base<D>::xaxis_base()
        : m_labels()
    {
    }

    template <class D>
    inline xaxis_base<D>::xaxis_base(const label_list& labels)
        : m_labels(labels)
    {
    }

    template <class D>
    inline xaxis_base<D>::xaxis_base(label_list&& labels)
        : m_labels(std::move(labels))
    {
    }

    template <class D>
    inline xaxis_base<D>::xaxis_base(std::initializer_list<key_type> init)
        : m_labels(init)
    {
    }

    template <class D>
    template <class InputIt>
    inline xaxis_base<D>::xaxis_base(InputIt first, InputIt last)
        : m_labels(first, last)
    {
    }

    /**
     * @name Downcast
     */
    //@{
    /**
     * Casts the object to its inheriting type (i.e. D); this method
     * is called when the object is an lvalue.
     */
    template <class D>
    inline auto xaxis_base<D>::derived_cast() & noexcept -> derived_type&
    {
        return *static_cast<derived_type*>(this);
    }

    /**
     * Casts the object to its inheriting type (i.e. D); this method
     * is called when the object is a constant lvalue.
     */
    template <class D>
    inline auto xaxis_base<D>::derived_cast() const & noexcept -> const derived_type&
    {
        return *static_cast<const derived_type*>(this);
    }

    /**
     * Casts the object to its inheriting type (i.e. D); this method
     * is called when the object is an rvalue.
     */
    template <class D>
    inline auto xaxis_base<D>::derived_cast() && noexcept -> derived_type
    {
        return *static_cast<derived_type*>(this);
    }
    //@}

    /**
     * @name Labels
     */
    //@{
    /**
     * Returns the list of labels contained in the axis.
     */
    template <class D>
    inline auto xaxis_base<D>::labels() const noexcept -> const label_list&
    {
        return m_labels;
    }

    /**
     * Return the i-th label of the axis.
     * @param i the position of the label.
     */
    template <class D>
    inline auto xaxis_base<D>::label(size_type i) const -> key_type
    {
        return m_labels[i];
    }

    /**
     * Checks if the axis has no labels.
     */
    template <class D>
    inline bool xaxis_base<D>::empty() const noexcept
    {
        return m_labels.empty();
    }

    /**
     * Returns the number of labels in the axis.
     */
    template <class D>
    inline auto xaxis_base<D>::size() const noexcept -> size_type
    {
        return m_labels.size();
    }
    //@}

    /**
     * @name Iterators
     */
    //@{
    /**
     * Returns a constant iterator to the first element of the axis.
     * This element is a pair label - position.
     */
    template <class D>
    inline auto xaxis_base<D>::begin() const noexcept -> const_iterator
    {
        return derived_cast().cbegin();
    }

    /**
     * Returns a constant iterator to the element following the last element
     * of the axis.
     */
    template <class D>
    inline auto xaxis_base<D>::end() const noexcept -> const_iterator
    {
        return derived_cast().cend();
    }

    /**
     * Returns a constant iterator to the first element of the reverse axis.
     * This element is a pair labal - position.
     */
    template <class D>
    inline auto xaxis_base<D>::rbegin() const noexcept -> const_reverse_iterator
    {
        return crbegin();
    }

    /**
     * Returns a constant iterator to the element following the last element
     * of the reversed axis.
     */
    template <class D>
    inline auto xaxis_base<D>::rend() const noexcept -> const_reverse_iterator
    {
        return crend();
    }

    /**
     * Returns a constant iterator to the first element of the reverse axis.
     * This element is a pair labal - position.
     */
    template <class D>
    inline auto xaxis_base<D>::crbegin() const noexcept -> const_reverse_iterator
    {
        return const_reverse_iterator(derived_cast().cend());
    }

    /**
     * Returns a constant iterator to the element following the last element
     * of the reversed axis.
     */
    template <class D>
    inline auto xaxis_base<D>::crend() const noexcept -> const_reverse_iterator
    {
        return const_reverse_iterator(derived_cast().cbegin());
    }
    //@}

    template <class D>
    inline auto xaxis_base<D>::mutable_labels() noexcept -> label_list&
    {
        return m_labels;
    }

    template <class D>
    template <class F>
    inline auto xaxis_base<D>::filter_labels(const F& f) const noexcept -> label_list
    {
        label_list l;
        std::copy_if(m_labels.cbegin(), m_labels.cend(), std::back_inserter(l), f);
        return l;
    }

    template <class D>
    template <class F>
    inline auto xaxis_base<D>::filter_labels(const F& f, size_type size) const noexcept -> label_list
    {
        label_list l(size);
        std::copy_if(m_labels.cbegin(), m_labels.cend(), l.begin(), f);
        return l;
    }

    /**
     * Returns true is \c lhs and \c rhs are equivalent axes, i.e. they contain the same
     * label - position pairs.
     * @param lhs an axis.
     * @param rhs an axis.
     */
    template <class D1, class D2>
    inline bool operator==(const xaxis_base<D1>& lhs, const xaxis_base<D2>& rhs) noexcept
    {
        return lhs.labels() == rhs.labels();
    }

    /**
     * Returns true is \c lhs and \c rhs are not equivalent axes, i.e. they contain different
     * label - position pairs.
     * @param lhs an axis.
     * @param rhs an axis.
     */
    template <class D1, class D2>
    inline bool operator!=(const xaxis_base<D1>& lhs, const xaxis_base<D2>& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    template <class OS, class D>
    inline OS& operator<<(OS& out, const xaxis_base<D>& axis)
    {
        using iterator = std::ostream_iterator<typename xaxis_base<D>::key_type, typename OS::char_type, typename OS::traits_type>;
        out << '(';
        std::copy(axis.labels().begin(), axis.labels().end(), iterator(out, ", "));
        out << ')';
        return out;
    }
}

#endif
