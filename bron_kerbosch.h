#include "degeneracy_ordering.h"
#include <algorithm>
#include <numeric>
#include <iterator>

template<typename AdjList>
void bron_kerbosch(const AdjList& adj, std::vector<int>& R, std::vector<int>& P,
                   std::vector<int>& X, std::vector<std::vector<int>>& cliques)
{
    if (P.empty()) {
        if (X.empty()) cliques.push_back(R);
        return;
    }
    std::vector<int> PuX;
    std::set_union(P.begin(), P.end(), X.begin(), X.end(),
                   std::back_inserter(PuX));
    std::vector<int> pivoted;
    bool first = true;
    for (int u : PuX) {
        std::vector<int> candidate;
        std::set_difference(P.begin(), P.end(), adj[u].begin(), adj[u].end(),
                            std::back_inserter(candidate));
        if (first || candidate.size() < pivoted.size()) {
            std::swap(candidate, pivoted);
            first = false;
        }
    }
    for (int v : pivoted) {
        std::vector<int> PN, XN;
        R.push_back(v);
        std::set_intersection(P.begin(), P.end(), adj[v].begin(), adj[v].end(),
                              std::back_inserter(PN));
        std::set_intersection(X.begin(), X.end(), adj[v].begin(), adj[v].end(),
                              std::back_inserter(XN));
        bron_kerbosch(adj, R, PN, XN, cliques);
        R.pop_back();
        P.erase(std::find(P.begin(), P.end(), v));
        X.insert(std::upper_bound(X.begin(), X.end(), v), v);
    }
}

template<typename AdjList>
std::vector<std::vector<int>> bron_kerbosch(const AdjList& adj)
{
    std::vector<std::vector<int>> ret;
    std::vector<int> R, P(adj.size()), X;
    std::iota(P.begin(), P.end(), 0);
    for (int v : degeneracy_ordering(adj)) {
        std::vector<int> PN, XN;
        R.push_back(v);
        std::set_intersection(P.begin(), P.end(), adj[v].begin(), adj[v].end(),
                              std::back_inserter(PN));
        std::set_intersection(X.begin(), X.end(), adj[v].begin(), adj[v].end(),
                              std::back_inserter(XN));
        bron_kerbosch(adj, R, PN, XN, ret);
        R.pop_back();
        P.erase(std::find(P.begin(), P.end(), v));
        X.insert(std::upper_bound(X.begin(), X.end(), v), v);
    }
    return ret;
}
