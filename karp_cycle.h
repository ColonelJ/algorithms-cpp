#include <vector>
#include <tuple>
#include <cmath>
#include <type_traits>

template<typename EdgeList>
double karp_cycle_min_mean_weight(const EdgeList& edges, int n_vertices)
{
    using std::begin;
    using Weight =
        typename std::decay<decltype(std::get<2>(*begin(edges)))>::type;
    std::vector<std::vector<std::pair<bool, Weight>>>
        dp(n_vertices, std::vector<std::pair<bool, Weight>>(n_vertices));
    for (const auto& edge : edges) {
        int dst;
        Weight w;
        std::tie(std::ignore, dst, w) = edge;
        if (dp[0][dst].first) {
            dp[0][dst].second = std::min(dp[0][dst].second, w);
        } else {
            dp[0][dst].first = true;
            dp[0][dst].second = w;
        }
    }
    for (int i = 1; i < n_vertices; ++i) {
        for (const auto& edge : edges) {
            int src, dst;
            Weight w;
            std::tie(src, dst, w) = edge;
            if (!dp[i - 1][src].first) continue;
            w += dp[i - 1][src].second;
            if (dp[i][dst].first) {
                dp[i][dst].second = std::min(dp[i][dst].second, w);
            } else {
                dp[i][dst].first = true;
                dp[i][dst].second = w;
            }
        }
    }
    bool found = false;
    double min_mean;
    for (int i = 0; i < n_vertices; ++i) {
        if (!dp[n_vertices - 1][i].first) continue;
        double max_mean = static_cast<double>(dp[n_vertices - 1][i].second)
                          / n_vertices;
        for (int j = 0; j < n_vertices - 1; ++j) {
            if (!dp[j][i].first) continue;
            max_mean = std::max(max_mean,
                static_cast<double>(dp[n_vertices - 1][i].second
                                    - dp[j][i].second)
                / (n_vertices - 1 - j));
        }
        if (found) min_mean = std::min(min_mean, max_mean);
        else min_mean = max_mean;
        found = true;
    }
    return found ? min_mean : NAN;
}
