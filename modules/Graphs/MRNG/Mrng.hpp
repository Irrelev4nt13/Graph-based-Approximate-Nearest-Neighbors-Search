#include <iostream>
#include <vector>

#include "PublicTypes.hpp"
#include "ImageDistance.hpp"
#include "GraphAlgorithm.hpp"
#include "Lsh.hpp"

class Mrng : public GraphAlgorithm
{
private:
    ImageDistance *distHelper;
    std::vector<std::vector<ImagePtr>> graph;
    ImagePtr navNode;
    int candidates;

public:
    Mrng(const std::vector<ImagePtr> &images, int l = 20);
    ~Mrng();
    std::vector<Neighbor> Approximate_kNN(ImagePtr query, int k);
};
