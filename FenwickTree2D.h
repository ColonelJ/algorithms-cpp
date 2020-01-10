#include <vector>

template<typename T, typename BinaryOperation = std::plus<T>,
         typename InverseOperation = std::minus<T>>
class FenwickTree2D
{
public:
    FenwickTree2D(int nx, int ny, T identity = T(),
                  const BinaryOperation& op = BinaryOperation(),
                  const InverseOperation& op_i = InverseOperation())
        : ft(ny + 1, std::vector<T>(nx + 1, identity)),
          id(identity), f(op), f_i(op_i) {}

    T query(int x, int y) const
    {
        T sum = id;
        for (; y; y -= y & -y) {
            for (int x1 = x; x1; x1 -= x1 & -x1) sum = f(sum, ft[y][x1]);
        }
        return sum;
    }

    T query(int x1, int y1, int x2, int y2) const
    {
        if (x1 == 1) {
            if (y1 == 1) return query(x2, y2);
            return f_i(query(x2, y2), query(x2, y1 - 1));
        }
        if (y1 == 1) return f_i(query(x2, y2), query(x1 - 1, y2));
        return f_i(f_i(f(query(x2, y2), query(x1 - 1, y1 - 1)),
                       query(x1 - 1, y2)), query(x2, y1 - 1));
    }

    void adjust(int x, int y, T v)
    {
        for (; y < static_cast<int>(ft.size()); y += y & -y) {
            for (int x1 = x; x1 < static_cast<int>(ft[0].size());
                 x1 += x1 & -x1) {
                ft[y][x1] = f(ft[y][x1], v);
            }
        }
    }

private:
    std::vector<std::vector<T>> ft;
    T id;
    BinaryOperation f;
    InverseOperation f_i;
};
