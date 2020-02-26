#include <vector>

template<typename AdjList>
std::vector<std::vector<int>> complement_graph(const AdjList& adj)
{
    using std::begin;
    using std::end;
    std::vector<std::vector<int>> ret(adj.size());
    int i = 0;
    for (const auto& adj_i : adj) {
        auto it = begin(adj_i);
        for (int j = 0; j < static_cast<int>(adj.size()); ++j) {
            if (it != end(adj_i) && *it == j) ++it;
            else if (j != i) ret[i].push_back(j);
        }
        ++i;
    }
    return ret;
}
