# Graph-based-Approximate-Nearest-Neighbors-Search

Implement the `GNNS` nearest neighbor search algorithm for vectors in d-dimensional space based on the euclidean metric (L2), which uses the `LSH` algorithm to construct the k-nearest neighbor (k-NN) graph during the index construction process. The program will be implemented so that, taking as input a vector `q` and `N` integers, it returns approximately:

1. The Nearest Neighbor to `q`
2. The `N` Nearest Neighbors to `q`.

Implement the `Search-on-Graph` nearest-neighbor search algorithm for vectors in d-dimensional space based on the euclidean metric (L2), using a monotone randomized neighborhood graph (`MRNG`) as an index. The program will be implemented so that, taking as input a vector q and integers N, it returns approximately:

1. The Nearest Neighbor to `q`
2. The `N` Nearest Neighbors to `q`.

The design of the code should allow its easy extension to vector spaces with other metrics, e.g. p-norm, or different spaces.

Compare the performances of the algorithms implemented in the queries among themselves and with the `LSH` and `Hypercuber` algorithms implemented in previous homework, in terms of search time and in terms of the maximum approximation fraction for different values ​​of the parameters of the algorithms.
