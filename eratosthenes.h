template<typename Container>
void eratosthenes(Container& c)
{
    c[2] = true;
    for (int i = 3; i < static_cast<int>(c.size()); i += 2) c[i] = true;
    for (int i = 3; i < static_cast<int>(c.size()); i += 2) {
        if (!c[i]) continue;
        int j = i * i;
        if (j >= static_cast<int>(c.size())) break;
        for (; j < static_cast<int>(c.size()); j += 2 * i) c[j] = false;
    }
}
