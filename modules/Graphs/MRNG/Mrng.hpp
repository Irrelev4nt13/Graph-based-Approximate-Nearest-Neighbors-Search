#include <iostream>
#include <vector>

#include "PublicTypes.hpp"
#include "ImageDistance.hpp"
#include "GraphAlgorithm.hpp"

class Mrng : public GraphAlgorithm
{
private:
    ImageDistance *distHelper;
    std::vector<std::vector<ImagePtr>> graph;

public:
    Mrng(const std::vector<ImagePtr> &images);
    ~Mrng();
    std::vector<Neighbor> Approximate_kNN(ImagePtr query, int k);
};

class KNNResult
{
public:
    ImagePtr image;
    double distance;
    bool operator>(const KNNResult &other) const
    {
        return distance > other.distance;
    }
};
