#include "adl.h"
#include <cstddef>
#include <type_traits>
#include <iterator>

template<typename CIt>
class ColumnIterator
{
    template<typename...>
    struct void_type
    {
        using type = void;
    };

    template<typename T, typename = void>
    struct ValueType
    {
        using type = typename std::remove_reference<
            decltype(std::declval<T>()[0])>::type;
    };

    template<typename T>
    struct ValueType<T, typename void_type<typename T::value_type>::type>
    {
        using type = typename T::value_type;
    };

public:
    using difference_type =
        typename std::iterator_traits<CIt>::difference_type;
    using value_type = typename ValueType<
        typename std::iterator_traits<CIt>::value_type>::type;
    using pointer = value_type*;
    using reference = decltype((*std::declval<CIt>())[0]);
    using iterator_category =
        typename std::iterator_traits<CIt>::iterator_category;

    ColumnIterator(CIt it, std::size_t pos) : it(it), pos(pos) {}
    reference operator*() const { return (*it)[pos]; }
    pointer operator->() const { return &(*it)[pos]; }
    reference operator[](difference_type n) const { return it[n][pos]; }
    ColumnIterator& operator++() { ++it; return *this; }
    ColumnIterator operator++(int) { auto ret = *this; ++*this; return ret; }
    ColumnIterator& operator--() { --it; return *this; }
    ColumnIterator operator--(int) { auto ret = *this; --*this; return ret; }
    ColumnIterator& operator+=(difference_type n) { it += n; return *this; }
    ColumnIterator operator+(difference_type n) const
        { auto ret = *this; return ret += n; }
    ColumnIterator& operator-=(difference_type n) { it -= n; return *this; }
    ColumnIterator operator-(difference_type n) const
        { auto ret = *this; return ret -= n; }
    difference_type operator-(const ColumnIterator& other) const
        { return it - other.it; }
    bool operator==(const ColumnIterator& other) const
        { return it == other.it; }
    bool operator!=(const ColumnIterator& other) const
        { return it != other.it; }
    bool operator<(const ColumnIterator& other) const { return it < other.it; }
    bool operator>(const ColumnIterator& other) const { return it > other.it; }
    bool operator>=(const ColumnIterator& other) const
        { return it >= other.it; }
    bool operator<=(const ColumnIterator& other) const
        { return it <= other.it; }

private:
    CIt it;
    std::size_t pos;
};

template<typename CIt>
ColumnIterator<CIt> operator+(
    typename ColumnIterator<CIt>::difference_type n,
    const ColumnIterator<CIt>& it)
{
    return it + n;
}

template<typename C>
auto begin_column(C&& c, std::size_t pos)
-> ColumnIterator<decltype(adl_begin(std::forward<C>(c)))>
{
    return {adl_begin(std::forward<C>(c)), pos};
}

template<typename C>
auto end_column(C&& c, std::size_t pos)
-> ColumnIterator<decltype(adl_end(std::forward<C>(c)))>
{
    return {adl_end(std::forward<C>(c)), pos};
}
