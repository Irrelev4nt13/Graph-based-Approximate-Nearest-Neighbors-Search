#include <iostream>
#include <vector>

#include "PublicTypes.hpp"
#include "ImageDistance.hpp"
#include "GraphAlgorithm.hpp"

class Mrng : public GraphAlgorithm
{
private:
    ImageDistance *distHelper;
    std::vector<ImagePtr> graph;

public:
    Mrng(const std::vector<ImagePtr> &images);
    ~Mrng();
    std::vector<Neighbor> Approximate_kNN(ImagePtr query);
    std::vector<ImagePtr> Approximate_Range_Search(ImagePtr query, const double radius);
};
