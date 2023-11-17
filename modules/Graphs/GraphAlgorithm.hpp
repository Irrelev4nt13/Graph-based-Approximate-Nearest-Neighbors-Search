#ifndef SEARCH_ALGORITHM_HPP_
#define SEARCH_ALGORITHM_HPP_

#include <vector>
#include "PublicTypes.hpp"

// Search Algorithm interface
class GraphAlgorithm
{
public:
    virtual std::vector<Neighbor> Approximate_kNN(ImagePtr query) = 0;
    virtual std::vector<ImagePtr> Approximate_Range_Search(ImagePtr query, const double radius) = 0;
};

#endif
