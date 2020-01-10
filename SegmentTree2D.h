#include <vector>
#include <functional>
#include <utility>

template<typename T, typename Container = std::vector<std::vector<T>>,
         typename Function = std::less<T>, bool merge = false>
class SegmentTree2D;

template<typename T, typename Container, typename Compare>
class SegmentTree2D<T, Container, Compare, false>
{
public:
    SegmentTree2D(const Container& c,
                  const Compare& compare = Compare())
        : a(c),
          st(a.size() * 4, std::vector<std::pair<int, int>>(a[0].size() * 4)), 
          f(compare)
    {
        build(0, 0, a.size() - 1);
    }

    SegmentTree2D(Container&& c,
                  const Compare& compare = Compare())
        : a(std::move(c)),
          st(a.size() * 4, std::vector<std::pair<int, int>>(a[0].size() * 4)), 
          f(compare)
    {
        build(0, 0, a.size() - 1);
    }

    std::pair<int, int> query(int x1, int y1, int x2, int y2) const
    {
        return query(0, 0, a.size() - 1, x1, y1, x2, y2);
    }

    void update(int x, int y, const T& v)
    {
        a[y][x] = v;
        update(0, 0, a.size() - 1, x, y);
    }

    void update(int x, int y, T&& v)
    {
        a[y][x] = std::move(v);
        update(0, 0, a.size() - 1, x, y);
    }

private:
    static int left(int p) { return (p << 1) + 1; }
    static int right(int p) { return (p << 1) + 2; }

    void build(int p, int L, int R)
    {
        if (L == R) {
            build(st[p], 0, 0, a[0].size() - 1, L, a[L]);
        } else {
            build(left(p), L, (L + R) / 2);
            build(right(p), (L + R) / 2 + 1, R);
            buildMerge(st[p], st[left(p)], st[right(p)], 0, 0, a[0].size() - 1);
        }
    }

    void build(std::vector<std::pair<int, int>>& str,
               int p, int L, int R, int y,
               const typename Container::value_type& ar)
    {
        if (L == R) {
            str[p] = {L, y};
        } else {
            build(str, left(p), L, (L + R) / 2, y, ar);
            build(str, right(p), (L + R) / 2 + 1, R, y, ar);
            auto p1 = str[left(p)], p2 = str[right(p)];
            str[p] = f(ar[p2.first], ar[p1.first]) ? p2 : p1;
        }
    }

    void buildMerge(std::vector<std::pair<int, int>>& str,
                    const std::vector<std::pair<int, int>>& strl,
                    const std::vector<std::pair<int, int>>& strr,
                    int p, int L, int R)
    {
        if (L != R) {
            buildMerge(str, strl, strr, left(p), L, (L + R) / 2);
            buildMerge(str, strl, strr, right(p), (L + R) / 2 + 1, R);
        }
        str[p] = f(a[strr[p].second][strr[p].first],
                   a[strl[p].second][strl[p].first]) ? strr[p] : strl[p];
    }

    std::pair<int, int> query(int p, int L, int R,
                              int x1, int y1, int x2, int y2) const
    {
        if (L >= y1 && R <= y2) {
            return query(st[p], 0, 0, a[0].size() - 1, x1, x2);
        }
        if (y2 < (L + R) / 2 + 1) {
            return query(left(p), L, (L + R) / 2, x1, y1, x2, y2);
        } else if (y1 > (L + R) / 2) {
            return query(right(p), (L + R) / 2 + 1, R, x1, y1, x2, y2);
        } else {
            auto p1 = query(left(p), L, (L + R) / 2, x1, y1, x2, y2);
            auto p2 = query(right(p), (L + R) / 2 + 1, R, x1, y1, x2, y2);
            return f(a[p2.second][p2.first], a[p1.second][p1.first]) ? p2 : p1;
        }
    }

    std::pair<int, int> query(const std::vector<std::pair<int, int>>& str,
                              int p, int L, int R, int x1, int x2) const
    {
        if (L >= x1 && R <= x2) return str[p];
        if (x2 < (L + R) / 2 + 1) {
            return query(str, left(p), L, (L + R) / 2, x1, x2);
        } else if (x1 > (L + R) / 2) {
            return query(str, right(p), (L + R) / 2 + 1, R, x1, x2);
        } else {
            auto p1 = query(str, left(p), L, (L + R) / 2, x1, x2);
            auto p2 = query(str, right(p), (L + R) / 2 + 1, R, x1, x2);
            return f(a[p2.second][p2.first], a[p1.second][p1.first]) ? p2 : p1;
        }
    }

    void update(int p, int L, int R, int x, int y)
    {
        if (L == R) {
            update(st[p], 0, 0, a[0].size() - 1, x, a[L]);
        } else {
            if (y > (L + R) / 2) {
                update(right(p), (L + R) / 2 + 1, R, x, y);
            } else {
                update(left(p), L, (L + R) / 2, x, y);
            }
            updateMerge(st[p], st[left(p)], st[right(p)],
                        0, 0, a[0].size() - 1, x);
        }
    }

    void update(std::vector<std::pair<int, int>>& str,
                int p, int L, int R, int x,
                const typename Container::value_type& ar)
    {
        if (L != R) {
            if (x > (L + R) / 2) {
                update(str, right(p), (L + R) / 2 + 1, R, x, ar);
            } else {
                update(str, left(p), L, (L + R) / 2, x, ar);
            }
            auto p1 = str[left(p)], p2 = str[right(p)];
            str[p] = f(ar[p2.first], ar[p1.first]) ? p2 : p1;
        }
    }

    void updateMerge(std::vector<std::pair<int, int>>& str,
                     const std::vector<std::pair<int, int>>& strl,
                     const std::vector<std::pair<int, int>>& strr,
                     int p, int L, int R, int x)
    {
        if (L != R) {
            if (x > (L + R) / 2) {
                updateMerge(str, strl, strr, right(p), (L + R) / 2 + 1, R, x);
            } else {
                updateMerge(str, strl, strr, left(p), L, (L + R) / 2, x);
            }
        }
        str[p] = f(a[strr[p].second][strr[p].first],
                   a[strl[p].second][strl[p].first]) ? strr[p] : strl[p];
    }

    Container a;
    std::vector<std::vector<std::pair<int, int>>> st;
    Compare f;
};

template<typename T, typename Container, typename Merge>
class SegmentTree2D<T, Container, Merge, true>
{
public:
    SegmentTree2D(const Container& a,
                  const Merge& merge = Merge())
        : nx(a[0].size()), ny(a.size()), st(ny * 4, std::vector<T>(nx * 4)),
          f(merge)
    {
        build(0, 0, ny - 1, a);
    }

    T query(int x1, int y1, int x2, int y2) const
    {
        return query(0, 0, ny - 1, x1, y1, x2, y2);
    }

    void update(int x, int y, const T& v)
    {
        update(0, 0, ny - 1, x, y, v);
    }

private:
    static int left(int p) { return (p << 1) + 1; }
    static int right(int p) { return (p << 1) + 2; }

    void build(int p, int L, int R, const Container& a)
    {
        if (L == R) {
            build(st[p], 0, 0, nx - 1, a[L]);
        } else {
            build(left(p), L, (L + R) / 2, a);
            build(right(p), (L + R) / 2 + 1, R, a);
            buildMerge(st[p], st[left(p)], st[right(p)], 0, 0, nx - 1);
        }
    }

    void build(std::vector<T>& str, int p, int L, int R,
               const typename Container::value_type& ar)
    {
        if (L == R) {
            str[p] = ar[L];
        } else {
            build(str, left(p), L, (L + R) / 2, ar);
            build(str, right(p), (L + R) / 2 + 1, R, ar);
            str[p] = f(str[left(p)], str[right(p)]);
        }
    }

    void buildMerge(std::vector<T>& str,
                    const std::vector<T>& strl, const std::vector<T>& strr,
                    int p, int L, int R)
    {
        if (L != R) {
            buildMerge(str, strl, strr, left(p), L, (L + R) / 2);
            buildMerge(str, strl, strr, right(p), (L + R) / 2 + 1, R);
        }
        str[p] = f(strl[p], strr[p]);
    }

    T query(int p, int L, int R, int x1, int y1, int x2, int y2) const
    {
        if (L >= y1 && R <= y2) {
            return query(st[p], 0, 0, nx - 1, x1, x2);
        }
        if (y2 < (L + R) / 2 + 1) {
            return query(left(p), L, (L + R) / 2, x1, y1, x2, y2);
        } else if (y1 > (L + R) / 2) {
            return query(right(p), (L + R) / 2 + 1, R, x1, y1, x2, y2);
        } else {
            return f(query(left(p), L, (L + R) / 2, x1, y1, x2, y2),
                     query(right(p), (L + R) / 2 + 1, R, x1, y1, x2, y2));
        }
    }

    T query(const std::vector<T>& str,
            int p, int L, int R, int x1, int x2) const
    {
        if (L >= x1 && R <= x2) return str[p];
        if (x2 < (L + R) / 2 + 1) {
            return query(str, left(p), L, (L + R) / 2, x1, x2);
        } else if (x1 > (L + R) / 2) {
            return query(str, right(p), (L + R) / 2 + 1, R, x1, x2);
        } else {
            return f(query(str, left(p), L, (L + R) / 2, x1, x2),
                     query(str, right(p), (L + R) / 2 + 1, R, x1, x2));
        }
    }

    void update(int p, int L, int R, int x, int y, const T& v)
    {
        if (L == R) {
            update(st[p], 0, 0, nx - 1, x, v);
        } else {
            if (y > (L + R) / 2) {
                update(right(p), (L + R) / 2 + 1, R, x, y, v);
            } else {
                update(left(p), L, (L + R) / 2, x, y, v);
            }
            updateMerge(st[p], st[left(p)], st[right(p)],
                        0, 0, nx - 1, x);
        }
    }

    void update(std::vector<T>& str, int p, int L, int R, int x, const T& v)
    {
        if (L == R) {
            str[p] = v;
        } else {
            if (x > (L + R) / 2) {
                update(str, right(p), (L + R) / 2 + 1, R, x, v);
            } else {
                update(str, left(p), L, (L + R) / 2, x, v);
            }
            str[p] = f(str[left(p)], str[right(p)]);
        }
    }

    void updateMerge(std::vector<T>& str,
                     const std::vector<T>& strl, const std::vector<T>& strr,
                     int p, int L, int R, int x)
    {
        if (L != R) {
            if (x > (L + R) / 2) {
                updateMerge(str, strl, strr, right(p), (L + R) / 2 + 1, R, x);
            } else {
                updateMerge(str, strl, strr, left(p), L, (L + R) / 2, x);
            }
        }
        str[p] = f(strl[p], strr[p]);
    }

    int nx, ny;
    std::vector<std::vector<T>> st;
    Merge f;
};
