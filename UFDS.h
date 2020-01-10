/* Union-Find Disjoint Sets */
#include <vector>
#include <set>
#include <unordered_map>

template<typename T>
class UFDS
{
    struct Entry
    {
        T p;
        int size;
        int rank;
    };

public:
    UFDS(T N) : v(N)
    {
        for (T i = 0; i < N; ++i) {
            v[i].p = i;
            v[i].size = 1;
        }
    }

    T findSet(T i) { return v[i].p == i ? i : (v[i].p = findSet(v[i].p)); }

    int getSetSize(T i) { return v[findSet(i)].size; }

    bool isSameSet(T i, T j) { return findSet(i) == findSet(j); }

    void unionSet(T i, T j)
    {
        if (!isSameSet(i, j)) {
            T x = findSet(i), y = findSet(j);
            if (v[x].rank > v[y].rank) {
                v[y].p = x;
                v[x].size += v[y].size;
            } else {
                v[x].p = y;
                v[y].size += v[x].size;
                if (v[x].rank == v[y].rank) ++v[y].rank;
            }
        }
    }

private:
    std::vector<Entry> v;
};

template<typename T>
class UFDSIterable
{
    struct Entry
    {
        T p;
        int rank;
        std::set<T> s;
    };

public:
    UFDSIterable(T N) : v(N)
    {
        for (T i = 0; i < N; ++i) {
            v[i].p = i;
            v[i].s.insert(i);
            roots.insert(i);
        }
    }

    T findSet(T i) { return v[i].p == i ? i : (v[i].p = findSet(v[i].p)); }

    const std::set<T>& getSet(T i) { return v[findSet(i)].s; }

    bool isSameSet(T i, T j) { return findSet(i) == findSet(j); }

    void unionSet(T i, T j)
    {
        if (!isSameSet(i, j)) {
            T x = findSet(i), y = findSet(j);
            if (v[x].rank > v[y].rank) {
                v[y].p = x;
                v[x].s.insert(v[y].s.begin(), v[y].s.end());
                v[y].s.clear();
                roots.erase(y);
            } else {
                v[x].p = y;
                v[y].s.insert(v[x].s.begin(), v[x].s.end());
                v[x].s.clear();
                roots.erase(x);
                if (v[x].rank == v[y].rank) ++v[y].rank;
            }
        }
    }

    const std::set<T>& getRoots() const { return roots; }

private:
    std::vector<Entry> v;
    std::set<T> roots;
};

template<typename T, template<typename...> class Container = std::unordered_map>
class UFDSLazy
{
    struct Entry
    {
        bool initialized;
        T p;
        int size;
        int rank;
    };

public:
    T findSet(T i)
    {
        Entry& entry = getEntry(i);
        return entry.p == i ? i : (entry.p = findSet(entry.p));
    }

    int getSetSize(T i) { return getEntry(findSet(i)).size; }

    bool isSameSet(T i, T j) { return findSet(i) == findSet(j); }

    void unionSet(T i, T j)
    {
        if (!isSameSet(i, j)) {
            T x = findSet(i), y = findSet(j);
            Entry& x_entry = getEntry(x);
            Entry& y_entry = getEntry(y);
            if (x_entry.rank > y_entry.rank) {
                y_entry.p = x;
                x_entry.size += y_entry.size;
            } else {
                x_entry.p = y;
                y_entry.size += x_entry.size;
                if (x_entry.rank == y_entry.rank) ++y_entry.rank;
            }
        }
    }

private:
    Entry& getEntry(T i)
    {
        Entry& entry = map[i];
        if (!entry.initialized) {
            entry.p = i;
            entry.size = 1;
            entry.initialized = true;
        }
        return entry;
    }

    Container<T, Entry> map;
};

template<typename T>
class UFDSLazy<T, std::vector>
{
    struct Entry
    {
        bool initialized;
        T p;
        int size;
        int rank;
    };

public:
    T findSet(T i)
    {
        Entry& entry = getEntry(i);
        return entry.p == i ? i : (entry.p = findSet(entry.p));
    }

    int getSetSize(T i) { return getEntry(findSet(i)).size; }

    bool isSameSet(T i, T j) { return findSet(i) == findSet(j); }

    void unionSet(T i, T j)
    {
        if (!isSameSet(i, j)) {
            T x = findSet(i), y = findSet(j);
            Entry& x_entry = getEntry(x);
            Entry& y_entry = getEntry(y);
            if (x_entry.rank > y_entry.rank) {
                y_entry.p = x;
                x_entry.size += y_entry.size;
            } else {
                x_entry.p = y;
                y_entry.size += x_entry.size;
                if (x_entry.rank == y_entry.rank) ++y_entry.rank;
            }
        }
    }

private:
    Entry& getEntry(T i)
    {
        if (static_cast<decltype(v.size())>(i) >= v.size()) v.resize(i + 1);
        Entry& entry = v[i];
        if (!entry.initialized) {
            entry.p = i;
            entry.size = 1;
            entry.initialized = true;
        }
        return entry;
    }

    std::vector<Entry> v;
};
