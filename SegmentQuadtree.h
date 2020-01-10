#include <vector>
#include <functional>
#include <utility>

template<typename E, typename Container = std::vector<std::vector<E>>,
         typename Function = std::less<E>, bool merge = false>
class SegmentQuadtree;

template<typename E, typename Container, typename Compare>
class SegmentQuadtree<E, Container, Compare, false>
{
public:
    SegmentQuadtree(const Container& c,
                    const Compare& compare = Compare())
        : a(c), f(compare)
    {
        int max_dim = std::max(a[0].size(), a.size());
        st.resize(max_dim * max_dim * 16 / 3);
        build(0, 0, 0, a[0].size() - 1, a.size() - 1);
    }

    SegmentQuadtree(Container&& c,
                const Compare& compare = Compare())
        : a(std::move(c)), f(compare)
    {
        int max_dim = std::max(a[0].size(), a.size());
        st.resize(max_dim * max_dim * 16 / 3);
        build(0, 0, 0, a[0].size() - 1, a.size() - 1);
    }

    std::pair<int, int> query(int x1, int y1, int x2, int y2) const
    {
        return query(0, 0, 0, a[0].size() - 1, a.size() - 1, x1, y1, x2, y2);
    }

    void update(int x, int y, const E& v)
    {
        a[y][x] = v;
        update(0, 0, 0, a[0].size() - 1, a.size() - 1, x, y);
    }

    void update(int x, int y, E&& v)
    {
        a[y][x] = std::move(v);
        update(0, 0, 0, a[0].size() - 1, a.size() - 1, x, y);
    }

private:
    static int topleft(int p) { return (p << 2) + 1; }
    static int topright(int p) { return (p << 2) + 2; }
    static int bottomleft(int p) { return (p << 2) + 3; }
    static int bottomright(int p) { return (p << 2) + 4; }

    void build(int p, int L, int T, int R, int B)
    {
        if (L == R) {
            if (T == B) {
                st[p] = {L, T};
            } else {
                build(topleft(p), L, T, L, (T + B) / 2);
                build(bottomleft(p), L, (T + B) / 2 + 1, L, B);
                auto p1 = st[topleft(p)], p2 = st[bottomleft(p)];
                st[p] = f(a[p2.second][p2.first], a[p1.second][p1.first])
                        ? p2 : p1;
            }
        } else {
            if (T == B) {
                build(topleft(p), L, T, (L + R) / 2, T);
                build(topright(p), (L + R) / 2 + 1, T, R, T);
                auto p1 = st[topleft(p)], p2 = st[topright(p)];
                st[p] = f(a[p2.second][p2.first], a[p1.second][p1.first])
                        ? p2 : p1;
            } else {
                build(topleft(p), L, T, (L + R) / 2, (T + B) / 2);
                build(topright(p), (L + R) / 2 + 1, T, R, (T + B) / 2);
                build(bottomleft(p), L, (T + B) / 2 + 1, (L + R) / 2, B);
                build(bottomright(p), (L + R) / 2 + 1, (T + B) / 2 + 1, R, B);
                auto p1 = st[topleft(p)], p2 = st[topright(p)],
                     p3 = st[bottomleft(p)], p4 = st[bottomright(p)];
                auto b1 = f(a[p2.second][p2.first], a[p1.second][p1.first])
                          ? p2 : p1;
                auto b2 = f(a[p4.second][p4.first], a[p3.second][p3.first])
                          ? p4 : p3;
                st[p] = f(a[b2.second][b2.first], a[b1.second][b1.first])
                        ? b2 : b1;
            }
        }
    }

    std::pair<int, int> query(int p, int L, int T, int R, int B,
                              int x1, int y1, int x2, int y2) const
    {
        if (L >= x1 && R <= x2 && T >= y1 && B <= y2) return st[p];
        if (L == R || x2 < (L + R) / 2 + 1) {
            if (T == B || y2 < (T + B) / 2 + 1) {
                return query(topleft(p),
                             L, T, (L + R) / 2, (T + B) / 2,
                             x1, y1, x2, y2);
            } else if (y1 > (T + B) / 2) {
                return query(bottomleft(p),
                             L, (T + B) / 2 + 1, (L + R) / 2, B,
                             x1, y1, x2, y2);
            } else {
                auto p1 = query(topleft(p),
                                L, T, (L + R) / 2, (T + B) / 2,
                                x1, y1, x2, y2);
                auto p2 = query(bottomleft(p),
                                L, (T + B) / 2 + 1, (L + R) / 2, B,
                                x1, y1, x2, y2);
                return f(a[p2.second][p2.first], a[p1.second][p1.first])
                       ? p2 : p1;
            }
        } else if (x1 > (L + R) / 2) {
            if (T == B || y2 < (T + B) / 2 + 1) {
                return query(topright(p),
                             (L + R) / 2 + 1, T, R, (T + B) / 2,
                             x1, y1, x2, y2);
            } else if (y1 > (T + B) / 2) {
                return query(bottomright(p),
                             (L + R) / 2 + 1, (T + B) / 2 + 1, R, B,
                             x1, y1, x2, y2);
            } else {
                auto p1 = query(topright(p),
                                (L + R) / 2 + 1, T, R, (T + B) / 2,
                                x1, y1, x2, y2);
                auto p2 = query(bottomright(p),
                                (L + R) / 2 + 1, (T + B) / 2 + 1, R, B,
                                x1, y1, x2, y2);
                return f(a[p2.second][p2.first], a[p1.second][p1.first])
                       ? p2 : p1;
            }
        } else {
            if (T == B || y2 < (T + B) / 2 + 1) {
                auto p1 = query(topleft(p),
                                L, T, (L + R) / 2, (T + B) / 2,
                                x1, y1, x2, y2);
                auto p2 = query(topright(p),
                                (L + R) / 2 + 1, T, R, (T + B) / 2,
                                x1, y1, x2, y2);
                return f(a[p2.second][p2.first], a[p1.second][p1.first])
                       ? p2 : p1;
            } else if (y1 > (T + B) / 2) {
                auto p1 = query(bottomleft(p),
                                L, (T + B) / 2 + 1, (L + R) / 2, B,
                                x1, y1, x2, y2);
                auto p2 = query(bottomright(p),
                                (L + R) / 2 + 1, (T + B) / 2 + 1, R, B,
                                x1, y1, x2, y2);
                return f(a[p2.second][p2.first], a[p1.second][p1.first])
                       ? p2 : p1;
            } else {
                auto p1 = query(topleft(p),
                                L, T, (L + R) / 2, (T + B) / 2,
                                x1, y1, x2, y2);
                auto p2 = query(topright(p),
                                (L + R) / 2 + 1, T, R, (T + B) / 2,
                                x1, y1, x2, y2);
                auto p3 = query(bottomleft(p),
                                L, (T + B) / 2 + 1, (L + R) / 2, B,
                                x1, y1, x2, y2);
                auto p4 = query(bottomright(p),
                                (L + R) / 2 + 1, (T + B) / 2 + 1, R, B,
                                x1, y1, x2, y2);
                auto b1 = f(a[p2.second][p2.first], a[p1.second][p1.first])
                          ? p2 : p1;
                auto b2 = f(a[p4.second][p4.first], a[p3.second][p3.first])
                          ? p4 : p3;
                return f(a[b2.second][b2.first], a[b1.second][b1.first])
                       ? b2 : b1;
            }
        }
    }

    void update(int p, int L, int T, int R, int B, int x, int y)
    {
        if (L == R) {
            if (T != B) {
                if (y > (T + B) / 2) {
                    update(bottomleft(p), L, (T + B) / 2 + 1, L, B, x, y);
                } else {
                    update(topleft(p), L, T, L, (T + B) / 2, x, y);
                }
                auto p1 = st[topleft(p)], p2 = st[bottomleft(p)];
                st[p] = f(a[p2.second][p2.first], a[p1.second][p1.first])
                        ? p2 : p1;
            }
        } else {
            if (T == B) {
                if (x > (L + R) / 2) {
                    update(topright(p), (L + R) / 2 + 1, T, R, T, x, y);
                } else {
                    update(topleft(p), L, T, (L + R) / 2, T, x, y);
                }
                auto p1 = st[topleft(p)], p2 = st[topright(p)];
                st[p] = f(a[p2.second][p2.first], a[p1.second][p1.first])
                        ? p2 : p1;
            } else {
                if (x > (L + R) / 2) {
                    if (y > (T + B) / 2) {
                        update(bottomright(p),
                               (L + R) / 2 + 1, (T + B) / 2 + 1, R, B, x, y);
                    } else {
                        update(topright(p),
                               (L + R) / 2 + 1, T, R, (T + B) / 2, x, y);
                    }
                } else {
                    if (y > (T + B) / 2) {
                        update(bottomleft(p),
                               L, (T + B) / 2 + 1, (L + R) / 2, B, x, y);
                    } else {
                        update(topleft(p),
                               L, T, (L + R) / 2, (T + B) / 2, x, y);
                    }
                }
                auto p1 = st[topleft(p)], p2 = st[topright(p)],
                     p3 = st[bottomleft(p)], p4 = st[bottomright(p)];
                auto b1 = f(a[p2.second][p2.first], a[p1.second][p1.first])
                          ? p2 : p1;
                auto b2 = f(a[p4.second][p4.first], a[p3.second][p3.first])
                          ? p4 : p3;
                st[p] = f(a[b2.second][b2.first], a[b1.second][b1.first])
                        ? b2 : b1;
            }
        }
    }

    Container a;
    std::vector<std::pair<int, int>> st;
    Compare f;
};

template<typename E, typename Container, typename Merge>
class SegmentQuadtree<E, Container, Merge, true>
{
public:
    SegmentQuadtree(const Container& a,
                    const Merge& merge = Merge())
        : nx(a[0].size()), ny(a.size()), f(merge)
    {
        int max_dim = std::max(nx, ny);
        st.resize(max_dim * max_dim * 16 / 3);
        build(0, 0, 0, nx - 1, ny - 1, a);
    }

    E query(int x1, int y1, int x2, int y2) const
    {
        return query(0, 0, 0, nx - 1, ny - 1, x1, y1, x2, y2);
    }

    void update(int x, int y, const E& v)
    {
        update(0, 0, 0, nx - 1, ny - 1, x, y, v);
    }

private:
    static int topleft(int p) { return (p << 2) + 1; }
    static int topright(int p) { return (p << 2) + 2; }
    static int bottomleft(int p) { return (p << 2) + 3; }
    static int bottomright(int p) { return (p << 2) + 4; }

    void build(int p, int L, int T, int R, int B, const Container& a)
    {
        if (L == R) {
            if (T == B) {
                st[p] = a[T][L];
            } else {
                build(topleft(p), L, T, L, (T + B) / 2, a);
                build(bottomleft(p), L, (T + B) / 2 + 1, L, B, a);
                st[p] = f(st[topleft(p)], st[bottomleft(p)]);
            }
        } else {
            if (T == B) {
                build(topleft(p), L, T, (L + R) / 2, T, a);
                build(topright(p), (L + R) / 2 + 1, T, R, T, a);
                st[p] = f(st[topleft(p)], st[topright(p)]);
            } else {
                build(topleft(p), L, T, (L + R) / 2, (T + B) / 2, a);
                build(topright(p), (L + R) / 2 + 1, T, R, (T + B) / 2, a);
                build(bottomleft(p), L, (T + B) / 2 + 1, (L + R) / 2, B, a);
                build(bottomright(p),
                      (L + R) / 2 + 1, (T + B) / 2 + 1, R, B, a);
                st[p] = f(f(f(st[topleft(p)], st[topright(p)]),
                            st[bottomleft(p)]), st[bottomright(p)]);
            }
        }
    }

    E query(int p, int L, int T, int R, int B,
            int x1, int y1, int x2, int y2) const
    {
        if (L >= x1 && R <= x2 && T >= y1 && B <= y2) return st[p];
        if (L == R || x2 < (L + R) / 2 + 1) {
            if (T == B || y2 < (T + B) / 2 + 1) {
                return query(topleft(p),
                             L, T, (L + R) / 2, (T + B) / 2,
                             x1, y1, x2, y2);
            } else if (y1 > (T + B) / 2) {
                return query(bottomleft(p),
                             L, (T + B) / 2 + 1, (L + R) / 2, B,
                             x1, y1, x2, y2);
            } else {
                return f(query(topleft(p),
                               L, T, (L + R) / 2, (T + B) / 2,
                               x1, y1, x2, y2),
                         query(bottomleft(p),
                               L, (T + B) / 2 + 1, (L + R) / 2, B,
                               x1, y1, x2, y2));
            }
        } else if (x1 > (L + R) / 2) {
            if (T == B || y2 < (T + B) / 2 + 1) {
                return query(topright(p),
                             (L + R) / 2 + 1, T, R, (T + B) / 2,
                             x1, y1, x2, y2);
            } else if (y1 > (T + B) / 2) {
                return query(bottomright(p),
                             (L + R) / 2 + 1, (T + B) / 2 + 1, R, B,
                             x1, y1, x2, y2);
            } else {
                return f(query(topright(p),
                               (L + R) / 2 + 1, T, R, (T + B) / 2,
                               x1, y1, x2, y2),
                         query(bottomright(p),
                               (L + R) / 2 + 1, (T + B) / 2 + 1, R, B,
                               x1, y1, x2, y2));
            }
        } else {
            if (T == B || y2 < (T + B) / 2 + 1) {
                return f(query(topleft(p),
                               L, T, (L + R) / 2, (T + B) / 2,
                               x1, y1, x2, y2),
                         query(topright(p),
                               (L + R) / 2 + 1, T, R, (T + B) / 2,
                               x1, y1, x2, y2));
            } else if (y1 > (T + B) / 2) {
                return f(query(bottomleft(p),
                               L, (T + B) / 2 + 1, (L + R) / 2, B,
                               x1, y1, x2, y2),
                         query(bottomright(p),
                               (L + R) / 2 + 1, (T + B) / 2 + 1, R, B,
                               x1, y1, x2, y2));
            } else {
                return f(f(f(query(topleft(p),
                                   L, T, (L + R) / 2, (T + B) / 2,
                                   x1, y1, x2, y2),
                             query(topright(p),
                                   (L + R) / 2 + 1, T, R, (T + B) / 2,
                                   x1, y1, x2, y2)),
                           query(bottomleft(p),
                                 L, (T + B) / 2 + 1, (L + R) / 2, B,
                                 x1, y1, x2, y2)),
                         query(bottomright(p),
                               (L + R) / 2 + 1, (T + B) / 2 + 1, R, B,
                               x1, y1, x2, y2));
            }
        }
    }

    void update(int p, int L, int T, int R, int B, int x, int y, const E& v)
    {
        if (L == R) {
            if (T == B) {
                st[p] = v;
            } else {
                if (y > (T + B) / 2) {
                    update(bottomleft(p), L, (T + B) / 2 + 1, L, B, x, y, v);
                } else {
                    update(topleft(p), L, T, L, (T + B) / 2, x, y, v);
                }
                st[p] = f(st[topleft(p)], st[bottomleft(p)]);
            }
        } else {
            if (T == B) {
                if (x > (L + R) / 2) {
                    update(topright(p), (L + R) / 2 + 1, T, R, T, x, y, v);
                } else {
                    update(topleft(p), L, T, (L + R) / 2, T, x, y, v);
                }
                st[p] = f(st[topleft(p)], st[topright(p)]);
            } else {
                if (x > (L + R) / 2) {
                    if (y > (T + B) / 2) {
                        update(bottomright(p),
                               (L + R) / 2 + 1, (T + B) / 2 + 1, R, B, x, y, v);
                    } else {
                        update(topright(p),
                               (L + R) / 2 + 1, T, R, (T + B) / 2, x, y, v);
                    }
                } else {
                    if (y > (T + B) / 2) {
                        update(bottomleft(p),
                               L, (T + B) / 2 + 1, (L + R) / 2, B, x, y, v);
                    } else {
                        update(topleft(p),
                               L, T, (L + R) / 2, (T + B) / 2, x, y, v);
                    }
                }
                st[p] = f(f(f(st[topleft(p)], st[topright(p)]),
                            st[bottomleft(p)]), st[bottomright(p)]);
            }
        }
    }

    int nx, ny;
    std::vector<E> st;
    Merge f;
};
