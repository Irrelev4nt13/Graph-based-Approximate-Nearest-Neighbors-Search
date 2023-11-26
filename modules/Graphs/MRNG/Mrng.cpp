#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <queue>
#include <unordered_set>

#include "Mrng.hpp"
#include "GraphTypes.hpp"
#include "Utils.hpp"
#include "BruteForce.hpp"

Mrng::Mrng(const std::vector<ImagePtr> &images, int numNn, int l) : numNn(numNn), candidates(l),
                                                                    distHelper(ImageDistance::getInstance()), navNode(nullptr)
{
    startClock();

    for (std::size_t i = 0; i < images.size(); i++)
    {
        std::vector<Neighbor> Rp = BruteForce(images, images[i], images.size() - 1);

        if (!this->navNode)
        {
            this->navNode = Rp[images.size() / 2].image;
        }

        // Initialize Lp with points that have minimum distance to p (images[i])
        std::vector<ImagePtr> Lp;
        double minApproxDistance = Rp[0].distance;
        for (int j = 0; j < (int)Rp.size(); j++)
        {
            if (Rp[j].distance != minApproxDistance)
            {
                break;
            }
            Lp.push_back(Rp[j].image);
        }

        for (int r = 0; r < (int)Rp.size(); r++)
        {
            if (std::find(Lp.begin(), Lp.end(), Rp[r].image) != Lp.end())
            {
                continue;
            }

            bool condition = true;
            for (int t = 0; t < (int)Lp.size(); t++)
            {
                double prDistance = Rp[r].distance;
                double ptDistance = distHelper->calculate(images[i], Lp[t]);
                double rtDistance = distHelper->calculate(Rp[r].image, Lp[t]);

                if (prDistance > ptDistance && prDistance > rtDistance)
                {
                    condition = false;
                    break; // pr is the longest edge, so r is not added to Lp.
                }
            }

            if (condition)
            {
                Lp.push_back(Rp[r].image);
            }
        }

        graph.push_back(Lp);
    }

    auto mrngDuration = stopClock();
    std::cout << "Mrng index construction finished in: " << mrngDuration.count() * 1e-9 << std::endl;
}

Mrng::~Mrng() {}

std::vector<Neighbor> Mrng::Approximate_kNN(ImagePtr query, int k)
{
    std::vector<ImagePtr> R;

    ImagePtr p = navNode;

    R.push_back(p);

    int i = 1;
    int visitedCounter = 0;
    while (i < candidates && (int)R.size() > visitedCounter)
    {
        p = R[visitedCounter];
        visitedCounter++;
        std::vector<ImagePtr> neighbors = graph[p->id];

        for (int k = 0; k < (int)neighbors.size(); k++)
        {
            if (std::find(R.begin(), R.end(), neighbors[k]) != R.end())
            {
                continue;
            }
            R.push_back(neighbors[k]);
            i++;
        }

        std::sort(R.begin(), R.end(), CompDistanceToRefImage(query, distHelper));
    }

    std::vector<Neighbor> kNN;

    for (int i = 0; i < k; i++)
    {
        kNN.push_back(Neighbor(R[i], distHelper->calculate(R[i], query)));
    }

    return kNN;
}
