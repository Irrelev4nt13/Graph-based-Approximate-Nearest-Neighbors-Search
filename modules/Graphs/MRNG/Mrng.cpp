#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

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

        std::vector<ImagePtr> Lp;

        double minDistance = std::numeric_limits<double>::infinity();
        for (const auto &point : Rp)
        {
            double distance = distHelper->calculate(point, images[i]);
            if (distance < minDistance)
            {
                minDistance = distance;
                Lp.clear();
                Lp.push_back(point);
            }
            else if (distance == minDistance)
            {
                Lp.push_back(point);
            }
        }

        for (const auto &r : Rp)
        {
            if (std::find(Lp.begin(), Lp.end(), r) != Lp.end())
            {
                continue; // r found in Lp
            }

            bool condition = true;
            for (const auto &t : Lp)
            {
                double prDistance = distHelper->calculate(images[i], r);
                double ptDistance = distHelper->calculate(images[i], t);
                double rtDistance = distHelper->calculate(r, t);

                if (prDistance > ptDistance && prDistance > rtDistance)
                {
                    condition = false;
                    break; // pr is the longest edge, so r is not added to Lp.
                }
            }

            if (condition)
            {
                Lp.push_back(r);
            }
        }

        graph.push_back(Lp);
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