#include <utility>

namespace adl_detail {

using std::begin;
using std::end;
using std::swap;

template<typename C>
auto adl_begin(C&& c) -> decltype(begin(std::forward<C>(c)))
{
    return begin(std::forward<C>(c));
}

template<typename C>
auto adl_end(C&& c) -> decltype(end(std::forward<C>(c)))
{
    return end(std::forward<C>(c));
}

template<typename T>
void adl_swap(T&& lhs, T&& rhs) noexcept(noexcept(swap(std::declval<T>(),
                                                       std::declval<T>())))
{
    swap(std::forward<T>(lhs), std::forward<T>(rhs));
}

} // namespace adl_detail

template<typename C>
auto adl_begin(C&& c) -> decltype(adl_detail::adl_begin(std::forward<C>(c)))
{
    return adl_detail::adl_begin(std::forward<C>(c));
}

template<typename C>
auto adl_end(C&& c) -> decltype(adl_detail::adl_end(std::forward<C>(c)))
{
    return adl_detail::adl_end(std::forward<C>(c));
}

template<typename T>
void adl_swap(T&& lhs, T&& rhs) noexcept(
    noexcept(adl_detail::adl_swap(std::declval<T>(), std::declval<T>())))
{
    adl_detail::adl_swap(std::forward<T>(lhs), std::forward<T>(rhs));
}
