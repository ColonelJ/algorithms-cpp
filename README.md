# algorithms-cpp
Algorithms for competitive programming in C++.

Algorithms are presented as single header file templates that can be `#include`d or copy and pasted into a solution.

## Utilities
- **Adjacency List to Edge List Adapter** – `AdjListEdgeListAdapter.h`
- **Argument Dependent Lookup** (begin, end and swap functions) – `adl.h`
- **Column Iterator** – `ColumnIterator.h`
- **Flatten Adapter** – `FlattenAdapter.h`

## Data Structures
- **Fenwick Tree** (a.k.a Binary Indexed Tree) – `FenwickTree.h`, `FenwickTree2D.h` (2D)
- **Segment Stabbing Tree** (Segment tree for stabbing queries) – `SegmentStabbingTree.h`
- **Segment Tree** – `SegmentTree.h`, `SegmentTree2D.h` (2D as segment tree of segment trees), `SegmentQuadtree.h` (2D as quadtree)
- **Union-Find Disjoint Sets** – `UFDS.h`

## Transformations
- **Complement Graph** – `complement_graph.h`

## Algorithms
- **Bron-Kerbosch Algorithm** (enumerates maximal cliques) – `bron_kerbosch.h`
- **Degeneracy Ordering** – `degeneracy_ordering.h`
- **DPLL Algorithm** (solves CNFSAT) – `dpll.h`
- **Euler Tour** (Hierholzer's algorithm) – `euler_tour.h`
- **Karp's Minimum Mean Weight Cycle Algorithm** – `karp_cycle.h`
- **Merge Sort** (including inversion index calculation) – `merge_sort.h`
- **Sieve of Eratosthenes** – `eratosthenes.h`
