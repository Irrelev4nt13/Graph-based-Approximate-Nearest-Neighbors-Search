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

    std::vector<std::vector<Neighbor>> Rps(images.size()); // All Rp vectors

    for (std::size_t i = 0; i < images.size(); i++)
    {
        // Get sorted Rp with brute force
        Rps[i] = BruteForce(images, images[i], images.size());
        Rps[i].erase(Rps[i].begin()); // Erase first element that is the same as the query
    }

    for (std::size_t i = 0; i < images.size(); i++)
    {
        // Calculate only once the navigating node
        if (!this->navNode)
        {
            // Set the median of the dataset as the navigating node
            this->navNode = Rps[i][images.size() / 2].image;
        }

        // Initialize Lp with points that have the minimum distance to p (images[i])
        std::vector<ImagePtr> Lp;
        double minDistance = Rps[i][0].distance;
        for (int j = 0; j < (int)Rps[i].size(); j++)
        {
            if (Rps[i][j].distance > minDistance)
            {
                break; // No more points with the minimum distance to add
            }
            Lp.push_back(Rps[i][j].image);
        }

        // For each element of Rp check the Mrng condition and add it to Lp
        for (int r = 0; r < (int)Rps[i].size(); r++)
        {
            if (std::find(Lp.begin(), Lp.end(), Rps[i][r].image) != Lp.end())
            {
                continue; // Point already in Lp
            }

            // Mrng condition to ensure monotonic path
            bool condition = true;
            for (int t = 0; t < (int)Lp.size(); t++)
            {
                double prDistance = Rps[i][r].distance; // same as dist(images[i], Rp[r])
                // double ptDistance = minDistance;    // same as dist(images[i], Lp[t])
                double rtDistance = distHelper->calculate(Rps[i][r].image, Lp[t]);

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
                Lp.push_back(Rps[i][r].image);
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
