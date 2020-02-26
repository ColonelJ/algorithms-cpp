#include <iterator>
#include <algorithm>

template<typename RandomIt1, typename RandomIt2, typename Compare>
int merge(RandomIt1 first, RandomIt1 middle, RandomIt1 last, RandomIt2 buf,
          Compare comp)
{
    RandomIt1 left = first, right = middle;
    int inversions = 0;
    while (left != middle && right != last) {
        if (comp(*right, *left)) {
            inversions += middle - left;
            *buf = *right;
            ++right;
        } else {
            *buf = *left;
            ++left;
        }
        ++buf;
    }
    std::copy(left, middle, buf);
    std::copy(right, last, buf);
    return inversions;
}

template<typename RandomIt1, typename RandomIt2, typename Compare>
int merge_sort(RandomIt1 src_first, RandomIt1 src_last,
               RandomIt2 dst_first, RandomIt2 dst_last, Compare comp)
{
    int length = src_last - src_first;
    if (length <= 1) return 0;
    RandomIt1 src_middle = src_first + (length / 2);
    RandomIt2 dst_middle = dst_first + (length / 2);
    return merge_sort(dst_first, dst_middle, src_first, src_middle, comp)
         + merge_sort(dst_middle, dst_last, src_middle, src_last, comp)
         + merge(src_first, src_middle, src_last, dst_first, comp);
}

template<typename RandomIt>
int merge_sort(RandomIt first, RandomIt last)
{
    int length = last - first;
    if (length <= 1) return 0;
    using T = typename std::iterator_traits<RandomIt>::value_type;
    T* buf = new T[length];
    std::copy(first, last, buf);
    int inversions = merge_sort(buf, buf + length, first, last, std::less<T>());
    delete[] buf;
    return inversions;
}

template<typename RandomIt, typename Compare>
int merge_sort(RandomIt first, RandomIt last, Compare comp)
{
    int length = last - first;
    if (length <= 1) return 0;
    using T = typename std::iterator_traits<RandomIt>::value_type;
    T* buf = new T[length];
    std::copy(first, last, buf);
    int inversions = merge_sort(buf, buf + length, first, last, comp);
    delete[] buf;
    return inversions;
}
