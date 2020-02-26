#include "adl.h"
#include <cstddef>
#include <type_traits>
#include <iterator>

template<typename Container>
class FlattenAdapter
{
    using OuterIt = decltype(adl_end(std::declval<Container&>()));
    using InnerIt = decltype(adl_end(*adl_end(std::declval<Container&>())));

    template<bool Const>
    class Iterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = typename std::iterator_traits<InnerIt>::value_type;
        using pointer = typename std::conditional<Const,
            const typename std::iterator_traits<InnerIt>::pointer,
            typename std::iterator_traits<InnerIt>::pointer>::type;
        using reference = typename std::conditional<Const,
            const typename std::iterator_traits<InnerIt>::reference,
            typename std::iterator_traits<InnerIt>::reference>::type;
        using iterator_category = typename std::common_type<
            typename std::iterator_traits<OuterIt>::iterator_category,
            typename std::iterator_traits<InnerIt>::iterator_category,
            std::bidirectional_iterator_tag>::type;

        Iterator(Container& c, OuterIt outer, InnerIt inner)
        : c(c), outer(outer), inner(inner)
        {
            go_to_next();
        }

        operator Iterator<true>() const { return {c, outer, inner}; }
        reference operator*() const { return *inner; }
        pointer operator->() const { return &*inner; }
        Iterator& operator++() { ++inner; go_to_next(); return *this; }
        Iterator operator++(int) { auto ret = *this; ++*this; return ret; }

        Iterator& operator--()
        {
            if (outer == adl_end(c)) inner = adl_end(*--outer);
            while (inner == adl_begin(*outer)) inner = adl_end(*--outer);
            --inner;
            return *this;
        }

        Iterator operator--(int) { auto ret = *this; --*this; return ret; }
        bool operator==(const Iterator& other) const
            { return outer == other.outer && inner == other.inner; }
        bool operator!=(const Iterator& other) const
            { return !(*this == other); }

    private:
        void go_to_next()
        {
            while (outer != adl_end(c) && inner == adl_end(*outer)) {
                ++outer;
                if (outer == adl_end(c)) inner = InnerIt();
                else inner = adl_begin(*outer);
            }
        }

        Container& c;
        OuterIt outer;
        InnerIt inner;
    };

public:
    using iterator = Iterator<std::is_const<Container>::value>;
    using const_iterator = Iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using value_type = typename iterator::value_type;
    using size_type = std::size_t;
    using difference_type = typename iterator::difference_type;
    using reference = typename iterator::reference;
    using const_reference = typename const_iterator::reference;
    using pointer = typename iterator::pointer;
    using const_pointer = typename const_iterator::pointer;

    FlattenAdapter(Container& c) : c(c) {}

    iterator begin()
    {
        return {c, adl_begin(c), adl_begin(c) == adl_end(c)
                                 ? InnerIt() : adl_begin(*adl_begin(c))};
    }

    const_iterator begin() const
    {
        return {c, adl_begin(c), adl_begin(c) == adl_end(c)
                                 ? InnerIt() : adl_begin(*adl_begin(c))};
    }

    const_iterator cbegin() const { return begin(); }
    iterator end() { return {c, adl_end(c), InnerIt()}; }
    const_iterator end() const { return {c, adl_end(c), InnerIt()}; }
    const_iterator cend() const { return end(); }
    reverse_iterator rbegin() { return reverse_iterator{end()}; }
    const_reverse_iterator rbegin() const
        { return const_reverse_iterator{end()}; }
    const_reverse_iterator crbegin() const { return rbegin(); }
    reverse_iterator rend() { return reverse_iterator{begin()}; }
    const_reverse_iterator rend() const
        { return const_reverse_iterator{begin()}; }
    const_reverse_iterator crend() const { return rend(); }

private:
    Container& c;
};

template<typename Container>
FlattenAdapter<Container> flatten(Container& c)
{
    return {c};
}
