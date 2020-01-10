#include <vector>

template<typename T, typename BinaryOperation = std::plus<T>,
         typename InverseOperation = std::minus<T>>
class FenwickTree
{
public:
    explicit FenwickTree(int n, T identity = T(),
                         const BinaryOperation& op = BinaryOperation(),
                         const InverseOperation& op_i = InverseOperation())
        : ft(n + 1, identity), id(identity), f(op), f_i(op_i) {}

    T query(int b) const
    {
        T sum = id;
        for (; b; b -= b & -b) sum = f(sum, ft[b]);
        return sum;
    }

    T query(int a, int b) const
    {
        if (a == 1) return query(b);
        return f_i(query(b), query(a - 1));
    }

    void adjust(int k, T v)
    {
        for (; k < static_cast<int>(ft.size()); k += k & -k) {
            ft[k] = f(ft[k], v);
        }
    }

    T getSingle(int k) const
    {
        T sum = ft[k];
        int z = k - (k & -k);
        --k;
        while (k != z) {
            sum = f_i(sum, ft[k]);
            k -= k & -k;
        }
        return sum;
    }

private:
    std::vector<T> ft;
    T id;
    BinaryOperation f;
    InverseOperation f_i;
};
