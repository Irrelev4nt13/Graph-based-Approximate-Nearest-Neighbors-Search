#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <queue>
#include <unordered_set>

#include "Mrng.hpp"
#include "GraphTypes.hpp"
#include "Utils.hpp"

Mrng::Mrng(const std::vector<ImagePtr> &images)
{
    distHelper = ImageDistance::getInstance();

    for (std::size_t i = 0; i < images.size(); i++)
    {
        startClock();
        std::vector<ImagePtr> Rp = images;

        Rp.erase(Rp.begin() + i);

        std::sort(Rp.begin(), Rp.end(), CompDistanceToRefImage(images[i], distHelper));

        auto duration = stopClock();

        std::cout << "duration: " << duration.count() * 1e-9 << std::endl;

        std::vector<ImagePtr> Lp;

        Lp.push_back(Rp[0]);

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

std::vector<Neighbor> Mrng::Approximate_kNN(ImagePtr query, int k)
{
    std::priority_queue<KNNResult, std::vector<KNNResult>, std::greater<KNNResult>> pq;
    std::unordered_set<size_t> visited;
    std::vector<KNNResult> results;
}
