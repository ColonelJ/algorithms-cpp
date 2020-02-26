#include <vector>
#include <algorithm>
#include <limits>

template <typename T, typename Interval>
class SegmentStabbingTree
{
    struct Entry
    {
        bool leaf;
        T start;
        T end;
        std::vector<Interval> subset;
    };

public:
    template<typename InputIt>
    SegmentStabbingTree(InputIt first, InputIt last)
    {
        std::vector<T> endpoints;
        for (auto it = first; it != last; ++it) {
            endpoints.push_back(it->start);
            endpoints.push_back(it->end);
        }
        std::sort(endpoints.begin(), endpoints.end());
        endpoints.erase(std::unique(endpoints.begin(), endpoints.end()),
                        endpoints.end());
        std::vector<std::pair<T, T>> leaves;
        leaves.reserve(2 * endpoints.size() + 1);
        if (endpoints.empty()) {
            leaves.emplace_back(std::numeric_limits<T>::lowest(),
                                std::numeric_limits<T>::max());
        } else {
            if (endpoints[0] > std::numeric_limits<T>::lowest()) {
                leaves.emplace_back(std::numeric_limits<T>::lowest(),
                                    endpoints[0] - 1);
            }
            for (int i = 0; i < static_cast<int>(endpoints.size()) - 1; ++i) {
                leaves.emplace_back(endpoints[i], endpoints[i]);
                if (endpoints[i + 1] > endpoints[i] + 1) {
                    leaves.emplace_back(endpoints[i] + 1, endpoints[i + 1] - 1);
                }
            }
            leaves.emplace_back(endpoints[endpoints.size() - 1],
                                endpoints[endpoints.size() - 1]);
            if (endpoints[endpoints.size() - 1] <
                    std::numeric_limits<T>::max()) {
                leaves.emplace_back(endpoints[endpoints.size() - 1] + 1,
                                    std::numeric_limits<T>::max());
            }
        }
        st.resize(leaves.size() * 4);
        build(0, 0, leaves.size() - 1, leaves);
        for (auto it = first; it != last; ++it) insert(0, *it);
    }

    std::vector<Interval> query(int i) const
    {
        std::vector<Interval> ret;
        query(0, i, ret);
        return ret;
    }

private:
    static int left(int p) { return (p << 1) + 1; }
    static int right(int p) { return (p << 1) + 2; }

    void build(int p, int L, int R, std::vector<std::pair<T, T>>& leaves)
    {
        if (L == R) {
            st[p].leaf = true;
            st[p].start = leaves[L].first;
            st[p].end = leaves[L].second;
        } else {
            build(left(p), L, (L + R) / 2, leaves);
            build(right(p), (L + R) / 2 + 1, R, leaves);
            st[p].start = st[left(p)].start;
            st[p].end = st[right(p)].end;
        }
    }

    void insert(int p, const Interval& interval)
    {
        if (st[p].start >= interval.start && st[p].end <= interval.end) {
            st[p].subset.push_back(interval);
        } else {
            if (interval.start <= st[left(p)].end) insert(left(p), interval);
            if (interval.end >= st[right(p)].start) insert(right(p), interval);
        }
    }

    void query(int p, int i, std::vector<Interval>& result) const
    {
        result.insert(result.end(), st[p].subset.begin(), st[p].subset.end());
        if (!st[p].leaf) {
            if (i <= st[left(p)].end) query(left(p), i, result);
            else query(right(p), i, result);
        }
    }

    std::vector<Entry> st;
};
