/* Hierholzer's algorithm */
#include <vector>
#include <algorithm>

template<typename AdjMatrix>
void euler_tour_adj_matrix_directed(AdjMatrix& adj, int u,
                                    std::vector<int>& tour)
{
    for (int v = 0; v < static_cast<int>(adj[u].size()); ++v) {
        if (!adj[u][v]) continue;
        adj[u][v] = 0;
        euler_tour_adj_matrix_directed(adj, v, tour);
    }
    tour.push_back(u);
}

template<typename AdjMatrix>
std::vector<int> euler_tour_adj_matrix_directed(AdjMatrix& adj)
{
    std::vector<int> ret;
    euler_tour_adj_matrix_directed(adj, 0, ret);
    std::reverse(ret.begin(), ret.end());
    return ret;
}

template<typename AdjMatrix>
void euler_tour_adj_matrix_undirected(AdjMatrix& adj, int u,
                                    std::vector<int>& tour)
{
    for (int v = 0; v < static_cast<int>(adj[u].size()); ++v) {
        if (!adj[u][v]) continue;
        adj[u][v] = 0;
        adj[v][u] = 0;
        euler_tour_adj_matrix_undirected(adj, v, tour);
    }
    tour.push_back(u);
}

template<typename AdjMatrix>
std::vector<int> euler_tour_adj_matrix_undirected(AdjMatrix& adj)
{
    std::vector<int> ret;
    euler_tour_adj_matrix_undirected(adj, 0, ret);
    std::reverse(ret.begin(), ret.end());
    return ret;
}

template<typename AdjList>
void euler_tour_adj_list_directed(AdjList& adj, int u, std::vector<int>& tour)
{
    while (!adj[u].empty()) {
        int v = adj[u].back();
        adj[u].pop_back();
        euler_tour_adj_list_directed(adj, v, tour);
    }
    tour.push_back(u);
}

template<typename AdjList>
std::vector<int> euler_tour_adj_list_directed(AdjList& adj)
{
    std::vector<int> ret;
    euler_tour_adj_list_directed(adj, 0, ret);
    std::reverse(ret.begin(), ret.end());
    return ret;
}

template<typename AdjList>
void euler_tour_adj_list_undirected(AdjList& adj, int u, std::vector<int>& tour)
{
    while (!adj[u].empty()) {
        int v = adj[u].back();
        adj[u].pop_back();
        for (auto it = adj[v].begin(); it != adj[v].end(); ++it) {
            if (*it == u) {
                adj[v].erase(it);
                break;
            }
        }
        euler_tour_adj_list_undirected(adj, v, tour);
    }
    tour.push_back(u);
}

template<typename AdjList>
std::vector<int> euler_tour_adj_list_undirected(AdjList& adj)
{
    std::vector<int> ret;
    euler_tour_adj_list_undirected(adj, 0, ret);
    std::reverse(ret.begin(), ret.end());
    return ret;
}
