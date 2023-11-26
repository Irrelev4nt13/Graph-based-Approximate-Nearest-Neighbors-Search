#include <iostream>
#include <vector>
#include <algorithm>
#include <set>

#include "Mrng.hpp"
#include "Utils.hpp"
#include "BruteForce.hpp"

Mrng::Mrng(const std::vector<ImagePtr> &images, int numNn, int l) : numNn(numNn), candidates(l),
                                                                    distHelper(ImageDistance::getInstance()), navNode(nullptr)
{
    startClock();

    for (std::size_t i = 0; i < images.size(); i++)
    {
        // Get sorted Rp with brute force
        std::vector<Neighbor> Rp = BruteForce(images, images[i], images.size());

        Rp.erase(Rp.begin()); // Erase first element that is the same as the query

        // Calculate only once the navigating node
        if (!this->navNode)
        {
            // Set the median of the dataset as the navigating node
            this->navNode = Rp[images.size() / 2].image;
        }

        // Initialize Lp with points that have the minimum distance to p (images[i])
        std::vector<ImagePtr> Lp;
        double minDistance = Rp[0].distance;
        for (int j = 0; j < (int)Rp.size(); j++)
        {
            if (Rp[j].distance > minDistance)
            {
                break; // No more points with the minimum distance to add
            }
            Lp.push_back(Rp[j].image);
        }

        // For each element of Rp check the Mrng condition and add it to Lp
        for (int r = 0; r < (int)Rp.size(); r++)
        {
            if (std::find(Lp.begin(), Lp.end(), Rp[r].image) != Lp.end())
            {
                continue; // Point already in Lp
            }

            // Mrng condition to ensure monotonic path
            bool condition = true;
            for (int t = 0; t < (int)Lp.size(); t++)
            {
                double prDistance = Rp[r].distance; // same as dist(images[i], Rp[r])
                double ptDistance = minDistance;    // same as dist(images[i], Lp[t])
                double rtDistance = distHelper->calculate(Rp[r].image, Lp[t]);

                // prDistance is always greater than ptDistance = minDistance since Rp is sorted
                // Need to check between prDistance and rtDistance for triangle prt
                if (prDistance > rtDistance)
                {
                    // pr is the longest edge, so it is not a valid neighbor for Mrng
                    condition = false;
                    break;
                }
            }

            if (condition)
            {
                Lp.push_back(Rp[r].image);
            }
        }

        graph.push_back(Lp); // Add neighbors of current image to graph
    }

    auto mrngDuration = stopClock();
    std::cout << "Mrng index construction finished in: " << mrngDuration.count() * 1e-9 << std::endl;
}

Mrng::~Mrng() {}

std::vector<Neighbor> Mrng::Approximate_kNN(ImagePtr query)
{
    // Initialize R to an empty set
    std::set<Neighbor, CompareNeighbor> R;

    // Start with the navigating node
    Neighbor p = Neighbor(navNode, distHelper->calculate(navNode, query));
    R.insert(p);

    int i = 1;
    int visitedNodes = 0;

    // Search for the number of candidates
    while (i < candidates && (int)R.size() > visitedNodes)
    {
        // Each time get the next element of R
        p = *(std::next(R.begin(), visitedNodes));
        visitedNodes++;

        // Get neighbors based on the graph index
        std::vector<ImagePtr> neighborImages = graph[p.image->id];
        for (int k = 0; k < (int)neighborImages.size(); k++)
        {
            // Insert
            auto result = R.insert(Neighbor(neighborImages[k], distHelper->calculate(neighborImages[k], query)));

            // Insert succeeded (not a duplicate)
            if (result.second)
            {
                i++; // Increment the number of candidates checked
            }
        }
    }

    // Copy the nearest neighbors until numNn or size of R
    std::vector<Neighbor> KnearestNeighbors(R.begin(), std::next(R.begin(), std::min(numNn, static_cast<int>(R.size()))));

    return KnearestNeighbors;
}
