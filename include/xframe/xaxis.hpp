/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
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

    template <class L, class T, class MT = hash_map_tag>
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

        template <class InputIt>
        xaxis(InputIt first, InputIt last);

        bool is_sorted() const noexcept;

        bool contains(const key_type& key) const;
        const mapped_type& operator[](const key_type& key) const;

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

    private:

        xaxis(const label_list& labels, bool is_sorted);
        xaxis(label_list&& labels, bool is_sorted);

        typename map_type::const_iterator find_index(const key_type& key) const;

        template <class... Args>
        bool merge_impl(const Args&... axes);

        template <class D, class... Args>
        bool merge_empty(const xaxis_base<D>& a, const Args&... axes);
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

    template <class L, class T, class MT>
    inline xaxis<L, T, MT>::xaxis()
        : base_type(), m_index(), m_is_sorted(true)
    {
    }

    template <class L, class T, class MT>
    inline xaxis<L, T, MT>::xaxis(const label_list& labels)
        : base_type(labels), m_index(), m_is_sorted()
    {
        m_is_sorted = init_is_sorted();
        populate_index();
    }

    template <class L, class T, class MT>
    inline xaxis<L, T, MT>::xaxis(label_list&& labels)
        : base_type(labels), m_index(), m_is_sorted()
    {
        m_is_sorted = init_is_sorted();
        populate_index();
    }

    template <class L, class T, class MT>
    inline xaxis<L, T, MT>::xaxis(const label_list& labels, bool is_sorted)
        : base_type(labels), m_index(), m_is_sorted(is_sorted)
    {
        populate_index();
    }

    template <class L, class T, class MT>
    inline xaxis<L, T, MT>::xaxis(label_list&& labels, bool is_sorted)
        : base_type(labels), m_index(), m_is_sorted(is_sorted)
    {
        populate_index();
    }

    template <class L, class T, class MT>
    inline xaxis<L, T, MT>::xaxis(std::initializer_list<key_type> init)
        : base_type(init), m_index(), m_is_sorted()
    {
        m_is_sorted = init_is_sorted();
        populate_index();
    }

    template <class L, class T, class MT>
    template <class InputIt>
    inline xaxis<L, T, MT>::xaxis(InputIt first, InputIt last)
        : base_type(first, last), m_index(), m_is_sorted()
    {
        m_is_sorted = init_is_sorted();
        populate_index();
    }

    template <class L, class T, class MT>
    inline bool xaxis<L, T, MT>::is_sorted() const noexcept
    {
        return m_is_sorted;
    }

    template <class L, class T, class MT>
    inline bool xaxis<L, T, MT>::contains(const key_type& key) const
    {
        return m_index.count(key) != typename map_type::size_type(0);
    }

    template <class L, class T, class MT>
    inline auto xaxis<L, T, MT>::operator[](const key_type& key) const -> const mapped_type&
    {
        return m_index.at(key);
    }

    template <class L, class T, class MT>
    template <class F>
    inline auto xaxis<L, T, MT>::filter(const F& f) const noexcept -> self_type
    {
        return self_type(base_type::filter_labels(f), m_is_sorted);
    }

    template <class L, class T, class MT>
    template <class F>
    inline auto xaxis<L, T, MT>::filter(const F& f, size_type size) const noexcept -> self_type
    {
        return self_type(base_type::filter_labels(f, size), m_is_sorted);
    }

    template <class L, class T, class MT>
    inline auto xaxis<L, T, MT>::find(const key_type& key) const -> const_iterator
    {
        auto map_iter = m_index.find(key);
        return map_iter != m_index.end() ? cbegin() + map_iter->second : cend();
    }

    template <class L, class T, class MT>
    inline auto xaxis<L, T, MT>::cbegin() const noexcept -> const_iterator
    {
        return const_iterator(this, this->labels().begin());
    }

    template <class L, class T, class MT>
    inline auto xaxis<L, T, MT>::cend() const noexcept -> const_iterator
    {
        return const_iterator(this, this->labels().end());
    }

    template <class L, class T, class MT>
    template <class... Args>
    inline bool xaxis<L, T, MT>::merge(const Args&... axes)
    {
        return this->empty() ? merge_empty(axes...) : merge_impl(axes...);
    }

    template <class L, class T, class MT>
    template <class... Args>
    inline bool xaxis<L, T, MT>::intersect(const Args&... axes)
    {
        bool res = intersect_to(this->mutable_labels(), axes.labels()...);
        populate_index();
        return res;
    }

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
    template <class D, class... Args>
    inline bool xaxis<L, T, MT>::merge_empty(const xaxis_base<D>& a, const Args&... axes)
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
    inline bool xaxis<L, T, MT>::merge_unsorted(bool broadcasting, const Arg& a, const Args&... axes)
    {
        bool res = merge_unsorted(broadcasting, axes...);
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
}

#endif
