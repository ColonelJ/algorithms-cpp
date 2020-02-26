#include <vector>
#include <stack>
#include <list>
#include <algorithm>
#include <limits>

struct CompareAbs
{
    bool operator()(int a, int b) const { return std::abs(a) < std::abs(b); }
};

struct DPLLVariable
{
    bool assigned;
    bool value;
    int pos_count;
    int neg_count;
};

template<typename CNF>
int dpll_heuristic(const CNF& cnf, int n_vars)
{
    int min_size = std::numeric_limits<int>::max();
    for (const auto& clause : cnf) {
        min_size = std::min<int>(min_size, clause.size());
    }
    std::vector<std::pair<int, int>> count(n_vars);
    for (const auto& clause : cnf) {
        if (static_cast<int>(clause.size()) != min_size) continue;
        for (int x : clause) {
            int v = std::abs(x) - 1;
            ++(x > 0 ? count[v].first : count[v].second);
        }
    }
    int best_count = 0;
    int literal;
    for (int i = 0; i < n_vars; ++i) {
        if (count[i].first > best_count) {
            best_count = count[i].first;
            literal = i + 1;
        }
        if (count[i].second > best_count) {
            best_count = count[i].second;
            literal = -(i + 1);
        }
    }
    return literal;
}

template<typename CNF>
bool dpll_assign(CNF& cnf, std::vector<DPLLVariable>& vars, int literal,
                 std::stack<int>& assignments)
{
    int var = std::abs(literal) - 1;
    if (vars[var].assigned) return vars[var].value == (literal > 0);
    vars[var].value = literal > 0;
    vars[var].assigned = true;
    auto it = cnf.begin();
    while (it != cnf.end()) {
        auto it2 =
            std::lower_bound(it->begin(), it->end(), literal, CompareAbs());
        if (it2 != it->end() && std::abs(*it2) == std::abs(literal)) {
            if (*it2 == literal) {
                for (int x : *it) {
                    int v = std::abs(x) - 1;
                    if (x > 0) {
                        --vars[v].pos_count;
                        if (!vars[v].pos_count && vars[v].neg_count
                            && v != var) assignments.push(-x);
                    } else {
                        --vars[v].neg_count;
                        if (!vars[v].neg_count && vars[v].pos_count
                            && v != var) assignments.push(-x);
                    }
                }
                it = cnf.erase(it);
            } else {
                --(literal > 0 ? vars[var].neg_count : vars[var].pos_count);
                it->erase(it2);
                if (it->empty()) return false;
                if (it->size() == 1) assignments.push((*it)[0]);
                ++it;
            }
        } else {
            ++it;
        }
    }
    return true;
}

template<typename CNF>
bool dpll(CNF& cnf, std::vector<DPLLVariable>& vars, int literal)
{
    std::stack<int> assignments;
    if (!dpll_assign(cnf, vars, literal, assignments)) return false;
    while (!assignments.empty()) {
        literal = assignments.top();
        assignments.pop();
        if (!dpll_assign(cnf, vars, literal, assignments)) return false;
    }
    if (cnf.empty()) return true;
    literal = dpll_heuristic(cnf, vars.size());
    {
        auto cnf2 = cnf;
        auto vars2 = vars;
        if (dpll(cnf2, vars2, -literal)) {
            std::swap(vars, vars2);
            return true;
        }
    }
    return dpll(cnf, vars, literal);
}

template<typename CNF>
bool dpll(const CNF& cnf, int n_vars, std::vector<bool>& model)
{
    using std::begin;
    using std::end;
    std::vector<DPLLVariable> vars(n_vars);
    std::stack<int> assignments;
    std::list<std::vector<int>> cnfp;
    for (const auto& clause : cnf) {
        if (clause.empty()) return false;
        cnfp.emplace_back(clause.size());
        std::partial_sort_copy(begin(clause), end(clause),
                               cnfp.back().begin(), cnfp.back().end(),
                               CompareAbs());
        int i, p = 1, last = cnfp.back()[0];
        for (i = 1; i < static_cast<int>(cnfp.back().size()); ++i) {
            if (std::abs(cnfp.back()[i]) == std::abs(last)) {
                if (cnfp.back()[i] != last) break;
            } else {
                last = cnfp.back()[i];
                cnfp.back()[p++] = last;
            }
        }
        if (i < static_cast<int>(cnfp.back().size())) {
            cnfp.pop_back();
            continue;
        }
        cnfp.back().resize(p);
        for (int x : cnfp.back()) {
            int var = std::abs(x) - 1;
            ++(x > 0 ? vars[var].pos_count : vars[var].neg_count);
        }
        if (cnfp.back().size() == 1) assignments.push(cnfp.back()[0]);
    }
    for (int i = 0; i < n_vars; ++i) {
        if (vars[i].pos_count && !vars[i].neg_count) {
            assignments.push(i + 1);
        } else if (vars[i].neg_count && !vars[i].pos_count) {
            assignments.push(-(i + 1));
        }
    }
    int literal;
    while (!assignments.empty()) {
        literal = assignments.top();
        assignments.pop();
        if (!dpll_assign(cnfp, vars, literal, assignments)) return false;
    }
    bool solved = cnfp.empty();
    if (!solved) {
        literal = dpll_heuristic(cnfp, vars.size());
        auto cnfp2 = cnfp;
        auto vars2 = vars;
        solved = dpll(cnfp2, vars2, -literal);
        if (solved) std::swap(vars, vars2);
    }
    if (!solved && !dpll(cnfp, vars, literal)) return false;
    model.resize(n_vars);
    for (int i = 0; i < n_vars; ++i) {
        if (vars[i].assigned) model[i] = vars[i].value;
        else model[i] = false;
    }
    return true;
}
