#include <vector>
#include <set>

template<typename AdjList>
std::vector<int> degeneracy_ordering(const AdjList& adj)
{
    std::vector<int> ret;
    ret.reserve(adj.size());
    std::vector<std::pair<bool, int>> used_degree(adj.size());
    std::set<std::pair<int, int>> degree_vertex;
    for (int i = 0; i < static_cast<int>(adj.size()); ++i) {
        used_degree[i].second = adj[i].size();
        degree_vertex.emplace(used_degree[i].second, i);
    }
    while (!degree_vertex.empty()) {
        auto dv = *degree_vertex.begin();
        degree_vertex.erase(degree_vertex.begin());
        ret.push_back(dv.second);
        used_degree[dv.second].first = true;
        for (int v : adj[dv.second]) {
            if (used_degree[v].first) continue;
            degree_vertex.erase({used_degree[v].second, v});
            degree_vertex.emplace(--used_degree[v].second, v);
        }
    }
    return ret;
}
