#include <vector>
#include <functional>
#include <utility>

template<typename T, typename Container = std::vector<T>,
         typename Function = std::less<T>, bool merge = false>
class SegmentTree;

template<typename T, typename Container, typename Compare>
class SegmentTree<T, Container, Compare, false>
{
public:
    SegmentTree(const Container& c,
                const Compare& compare = Compare())
        : a(c), st(a.size() * 4), f(compare)
    {
        build(0, 0, a.size() - 1);
    }

    SegmentTree(Container&& c,
                const Compare& compare = Compare())
        : a(std::move(c)), st(a.size() * 4), f(compare)
    {
        build(0, 0, a.size() - 1);
    }

    template<typename InputIt>
    SegmentTree(InputIt first, InputIt last,
                const Compare& compare = Compare())
        : a(first, last), st(a.size() * 4), f(compare)
    {
        build(0, 0, a.size() - 1);
    }

    int query(int i, int j) const { return query(0, 0, a.size() - 1, i, j); }

    void update(int i, const T& v)
    {
        a[i] = v;
        update(0, 0, a.size() - 1, i);
    }

    void update(int i, T&& v)
    {
        a[i] = std::move(v);
        update(0, 0, a.size() - 1, i);
    }

private:
    static int left(int p) { return (p << 1) + 1; }
    static int right(int p) { return (p << 1) + 2; }

    void build(int p, int L, int R)
    {
        if (L == R) {
            st[p] = L;
        } else {
            build(left(p), L, (L + R) / 2);
            build(right(p), (L + R) / 2 + 1, R);
            int p1 = st[left(p)], p2 = st[right(p)];
            st[p] = f(a[p2], a[p1]) ? p2 : p1;
        }
    }

    int query(int p, int L, int R, int i, int j) const
    {
        if (L >= i && R <= j) return st[p];
        if (j < (L + R) / 2 + 1) {
            return query(left(p), L, (L + R) / 2, i, j);
        } else if (i > (L + R) / 2) {
            return query(right(p), (L + R) / 2 + 1, R, i, j);
        } else {
            int p1 = query(left(p), L, (L + R) / 2, i, j);
            int p2 = query(right(p), (L + R) / 2 + 1, R, i, j);
            return f(a[p2], a[p1]) ? p2 : p1;
        }
    }

    void update(int p, int L, int R, int i)
    {
        if (L != R) {
            if (i > (L + R) / 2) {
                update(right(p), (L + R) / 2 + 1, R, i);
            } else {
                update(left(p), L, (L + R) / 2, i);
            }
            int p1 = st[left(p)], p2 = st[right(p)];
            st[p] = f(a[p2], a[p1]) ? p2 : p1;
        }
    }

    Container a;
    std::vector<int> st;
    Compare f;
};

template <typename T, typename Container, typename Merge>
class SegmentTree<T, Container, Merge, true>
{
public:
    SegmentTree(const Container& a,
                const Merge& merge = Merge())
        : n(a.size()), st(n * 4), f(merge)
    {
        build(0, 0, n - 1, a);
    }

    template<typename InputIt>
    SegmentTree(InputIt first, InputIt last,
                const Merge& merge = Merge())
        : f(merge)
    {
        Container a(first, last);
        n = a.size();
        st.resize(n * 4);
        build(0, 0, n - 1, a);
    }

    T query(int i, int j) const { return query(0, 0, n - 1, i, j); }

    void update(int i, const T& v)
    {
        update(0, 0, n - 1, i, v);
    }

private:
    static int left(int p) { return (p << 1) + 1; }
    static int right(int p) { return (p << 1) + 2; }

    void build(int p, int L, int R, const Container& a)
    {
        if (L == R) {
            st[p] = a[L];
        } else {
            build(left(p), L, (L + R) / 2, a);
            build(right(p), (L + R) / 2 + 1, R, a);
            st[p] = f(st[left(p)], st[right(p)]);
        }
    }

    T query(int p, int L, int R, int i, int j) const
    {
        if (L >= i && R <= j) return st[p];
        if (j < (L + R) / 2 + 1) {
            return query(left(p), L, (L + R) / 2, i, j);
        } else if (i > (L + R) / 2) {
            return query(right(p), (L + R) / 2 + 1, R, i, j);
        } else {
            return f(query(left(p), L, (L + R) / 2, i, j),
                     query(right(p), (L + R) / 2 + 1, R, i, j));
        }
    }

    void update(int p, int L, int R, int i, const T& v)
    {
        if (L == R) {
            st[p] = v;
        } else {
            if (i > (L + R) / 2) {
                update(right(p), (L + R) / 2 + 1, R, i, v);
            } else {
                update(left(p), L, (L + R) / 2, i, v);
            }
            st[p] = f(st[left(p)], st[right(p)]);
        }
    }

    int n;
    std::vector<T> st;
    Merge f;
};
