#include "adl.h"
#include <cstddef>
#include <type_traits>
#include <iterator>
#include <tuple>

template<typename AdjList>
class AdjListEdgeListAdapter
{
    using OuterIt = decltype(adl_end(std::declval<AdjList&>()));
    using InnerIt = decltype(adl_end(*adl_end(std::declval<AdjList&>())));

    template<bool Const>
    class Iterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = std::pair<const int, int>;
        using reference = typename std::conditional<Const,
            std::pair<const int, const int&>, std::pair<const int, int&>>::type;
        using pointer = reference*;
        using iterator_category = typename std::common_type<
            typename std::iterator_traits<OuterIt>::iterator_category,
            typename std::iterator_traits<InnerIt>::iterator_category,
            std::forward_iterator_tag>::type;

    private:
        struct ArrowProxy
        {
            reference r;
            reference* operator->() { return &r; }
        };

    public:
        Iterator(AdjList& adj, OuterIt outer, InnerIt inner)
        : adj(adj), outer(outer), inner(inner)
        {
            go_to_next();
        }

        operator Iterator<true>() const
        {
            return {adj, outer, inner};
        }

        reference operator*() const { return {outer_idx, *inner}; }
        ArrowProxy operator->() const
            { return ArrowProxy{{outer_idx, *inner}}; }
        Iterator& operator++() { ++inner; go_to_next(); return *this; }
        Iterator operator++(int) { auto ret = *this; ++*this; return ret; }
        bool operator==(const Iterator& other) const
            { return outer == other.outer && inner == other.inner; }
        bool operator!=(const Iterator& other) const
            { return !(*this == other); }

    private:
        void go_to_next()
        {
            while (outer != adl_end(adj) && inner == adl_end(*outer)) {
                ++outer;
                ++outer_idx;
                if (outer == adl_end(adj)) inner = InnerIt();
                else inner = adl_begin(*outer);
            }
        }

        AdjList& adj;
        OuterIt outer;
        InnerIt inner;
        int outer_idx = 0;
    };

public:
    using iterator = Iterator<std::is_const<AdjList>::value>;
    using const_iterator = Iterator<true>;
    using value_type = typename iterator::value_type;
    using size_type = std::size_t;
    using difference_type = typename iterator::difference_type;
    using reference = typename iterator::reference;
    using const_reference = typename const_iterator::reference;
    using pointer = typename iterator::pointer;
    using const_pointer = typename const_iterator::pointer;

    AdjListEdgeListAdapter(AdjList& adj) : adj(adj) {}

    iterator begin()
    {
        return {adj, adl_begin(adj), adl_begin(adj) == adl_end(adj)
                                     ? InnerIt() : adl_begin(*adl_begin(adj))};
    }

    const_iterator begin() const
    {
        return {adj, adl_begin(adj), adl_begin(adj) == adl_end(adj)
                                     ? InnerIt() : adl_begin(*adl_begin(adj))};
    }

    const_iterator cbegin() const { return begin(); }

    iterator end()
    {
        return {adj, adl_end(adj), InnerIt()};
    }

    const_iterator end() const
    {
        return {adj, adl_end(adj), InnerIt()};
    }

    const_iterator cend() const { return end(); }

private:
    AdjList& adj;
};

template<typename AdjList>
class AdjListEdgeListWeightedAdapter
{
    using OuterIt = decltype(adl_end(std::declval<AdjList&>()));
    using InnerIt = decltype(adl_end(*adl_end(std::declval<AdjList&>())));

    template<bool Const>
    class Iterator
    {
        using Weight = decltype(adl_end(*adl_end(std::declval<AdjList&>()))
                                ->second);
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = std::tuple<const int, int, Weight>;
        using reference = typename std::conditional<Const,
            std::tuple<const int, const int&, const Weight&>,
            std::tuple<const int, int&, Weight&>>::type;
        using pointer = reference*;
        using iterator_category = typename std::common_type<
            typename std::iterator_traits<OuterIt>::iterator_category,
            typename std::iterator_traits<InnerIt>::iterator_category,
            std::forward_iterator_tag>::type;

    private:
        struct ArrowProxy
        {
            reference r;
            reference* operator->() { return &r; }
        };

    public:
        Iterator(AdjList& adj, OuterIt outer, InnerIt inner)
        : adj(adj), outer(outer), inner(inner)
        {
            go_to_next();
        }

        operator Iterator<true>() const
        {
            return {adj, outer, inner};
        }

        reference operator*() const
            { return {outer_idx, inner->first, inner->second}; }
        ArrowProxy operator->() const
            { return ArrowProxy{{outer_idx, inner->first, inner->second}}; }
        Iterator& operator++() { ++inner; go_to_next(); return *this; }
        Iterator operator++(int) { auto ret = *this; ++*this; return ret; }
        bool operator==(const Iterator& other) const
            { return outer == other.outer && inner == other.inner; }
        bool operator!=(const Iterator& other) const
            { return !(*this == other); }

    private:
        void go_to_next()
        {
            while (outer != adl_end(adj) && inner == adl_end(*outer)) {
                ++outer;
                ++outer_idx;
                if (outer == adl_end(adj)) inner = InnerIt();
                else inner = adl_begin(*outer);
            }
        }

        AdjList& adj;
        OuterIt outer;
        InnerIt inner;
        int outer_idx = 0;
    };

public:
    using iterator = Iterator<std::is_const<AdjList>::value>;
    using const_iterator = Iterator<true>;
    using value_type = typename iterator::value_type;
    using size_type = std::size_t;
    using difference_type = typename iterator::difference_type;
    using reference = typename iterator::reference;
    using const_reference = typename const_iterator::reference;
    using pointer = typename iterator::pointer;
    using const_pointer = typename const_iterator::pointer;

    AdjListEdgeListWeightedAdapter(AdjList& adj) : adj(adj) {}

    iterator begin()
    {
        return {adj, adl_begin(adj), adl_begin(adj) == adl_end(adj)
                                     ? InnerIt() : adl_begin(*adl_begin(adj))};
    }

    const_iterator begin() const
    {
        return {adj, adl_begin(adj), adl_begin(adj) == adl_end(adj)
                                     ? InnerIt() : adl_begin(*adl_begin(adj))};
    }

    const_iterator cbegin() const { return begin(); }
    iterator end() { return {adj, adl_end(adj), InnerIt()}; }
    const_iterator end() const { return {adj, adl_end(adj), InnerIt()}; }
    const_iterator cend() const { return end(); }

private:
    AdjList& adj;
};

template<typename AdjList>
AdjListEdgeListAdapter<AdjList> adj_list_to_edge_list(AdjList& adj)
{
    return {adj};
}

template<typename AdjList>
AdjListEdgeListWeightedAdapter<AdjList>
    adj_list_to_edge_list_weighted(AdjList& adj)
{
    return {adj};
}
