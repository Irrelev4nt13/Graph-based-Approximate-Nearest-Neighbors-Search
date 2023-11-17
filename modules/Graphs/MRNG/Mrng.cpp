#include <iostream>
#include <vector>
#include <algorithm>

#include "Mrng.hpp"
#include "GraphTypes.hpp"
#include "Utils.hpp"

Mrng::Mrng(const std::vector<ImagePtr> &images)
{
    distHelper = ImageDistance::getInstance();

    for (std::size_t i = 0; i < images.size(); i++)
    {
        std::vector<ImagePtr> Rp;

        for (std::size_t j = 0; j < images.size(); j++)
        {
            if (i != j)
                Rp.push_back(images[j]);
        }

        std::sort(Rp.begin(), Rp.end(), CompDistanceToRefImage(images[i], distHelper));
    }
}

Mrng::~Mrng() {}

// std::vector<Edge> edges;

// // Calculate distances all between image pairs
// for (int i = 0; i < images.size(); i++)
// {

//     for (int j = i + 1; j < images.size(); j++)
//     {
//         double dist = distHelper->calculate(images[i], images[j]);
//         edges.push_back(Edge(images[i], images[j], dist));
//     }
// }

// // Sort the edges
// std::sort(edges.begin(), edges.end(), CompareEdge());

std::vector<Neighbor> Mrng::Approximate_kNN(ImagePtr query)
{
}

std::vector<ImagePtr> Mrng::Approximate_Range_Search(ImagePtr query, const double radius)
{
}