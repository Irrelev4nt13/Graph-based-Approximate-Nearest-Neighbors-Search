#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <queue>
#include <unordered_set>

#include "Mrng.hpp"
#include "GraphTypes.hpp"
#include "Utils.hpp"
#include "Lsh.hpp"
#include "BruteForce.hpp"

Mrng::Mrng(const std::vector<ImagePtr> &images, int l) : distHelper(ImageDistance::getInstance()), candidates(l)
{
    // brute force. Uncomment to use
    // useBruteForce(images);

    // lsh method. Uncomment to use
    // startClock();
    // Lsh *lsh = new Lsh(images, 4, 5, (int)images.size() - 1, 2240, (int)images.size() / 8);
    // auto lshDuration = stopClock();
    // std::cout << "lsh construction finished in: " << lshDuration.count() * 1e-9 << std::endl;

    startClock();

    useBruteForce(images);

    auto mrngDuration = stopClock();
    std::cout << "mrng construction finished in: " << mrngDuration.count() * 1e-9 << std::endl;

    // delete lsh;
}

// void Mrng::useBruteForce(const std::vector<ImagePtr> &images)
// {
//     for (std::size_t i = 0; i < images.size(); i++)
//     {
//         startClock();
//         std::vector<ImagePtr> Rp = images;

//         Rp.erase(Rp.begin() + i);

//         std::sort(Rp.begin(), Rp.end(), CompDistanceToRefImage(images[i], distHelper));

//         auto duration = stopClock();

//         std::cout << "duration: " << duration.count() * 1e-9 << std::endl;

//         std::vector<ImagePtr> Lp;

//         Lp.push_back(Rp[0]);

//         for (const auto &r : Rp)
//         {
//             if (std::find(Lp.begin(), Lp.end(), r) != Lp.end())
//             {
//                 continue; // r found in Lp
//             }

//             bool condition = true;
//             for (const auto &t : Lp)
//             {
//                 double prDistance = distHelper->calculate(images[i], r);
//                 double ptDistance = distHelper->calculate(images[i], t);
//                 double rtDistance = distHelper->calculate(r, t);

//                 if (prDistance > ptDistance && prDistance > rtDistance)
//                 {
//                     condition = false;
//                     break; // pr is the longest edge, so r is not added to Lp.
//                 }
//             }

//             if (condition)
//             {
//                 Lp.push_back(r);
//             }
//         }

//         graph.push_back(Lp);
//     }
// }

void Mrng::useBruteForce(const std::vector<ImagePtr> &images)
{
    this->navNode = nullptr;

    for (std::size_t i = 0; i < images.size(); i++)
    {
        // Get Rp sorted by Lsh distance approximation algorithm
        // Lsh *tempLsh = new Lsh(images, 4, 5, images.size(), 2240, images.size() / 8);

        std::vector<Neighbor> Rp = BruteForce(images, images[i], images.size() - 1);

        // delete tempLsh;

        if (!this->navNode)
        {
            this->navNode = new Neighbor(Rp[images.size() / 2]);
        }

        // startClock();

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

        // auto duration = stopClock();

        // std::cout << "duration: " << duration.count() * 1e-9 << std::endl;

        graph.push_back(Lp);
    }
}

Mrng::~Mrng()
{
    std::cout << "destructor" << std::endl;
    delete navNode;
}

// std::vector<Neighbor> Mrng::Approximate_kNN(ImagePtr query, int k)
// {
//     std::vector<ImagePtr> R;

//     R.push_back(navNode->image);

//     int i = 1;

//     while (i < candidates)
//     {
//         std::vector<ImagePtr> neighbors = graph[navNode->image->id];

//         for (int k = 0; k < (int)neighbors.size(); k++)
//         {
//             if (std::find(R.begin(), R.end(), neighbors[k]) != R.end())
//             {
//                 continue;
//             }
//             R.push_back(neighbors[k]);
//             i++;
//         }

//         std::sort(R.begin(), R.end(), CompDistanceToRefImage(navNode->image, distHelper));
//     }

//     std::vector<Neighbor> kNN;

//     for (int i = 0; i < k; i++)
//     {
//         kNN.push_back(Neighbor(R[i], distHelper->calculate(R[i], query)));
//     }

//     return kNN;
// }

std::vector<Neighbor> Mrng::Approximate_kNN(ImagePtr query, int k)
{
    std::vector<ImagePtr> R;
    std::unordered_set<int> visited; // To keep track of visited nodes.

    // Start with navNode
    R.push_back(navNode->image);
    visited.insert(navNode->image->id);

    while (R.size() < candidates)
    {
        bool addedNew = false;

        // Temporary vector to hold new neighbors
        std::vector<ImagePtr> newNeighbors;

        for (const auto &imgPtr : R)
        {
            std::vector<ImagePtr> neighbors = graph[imgPtr->id];

            for (const auto &neighbor : neighbors)
            {
                if (visited.find(neighbor->id) == visited.end())
                {
                    newNeighbors.push_back(neighbor);
                    visited.insert(neighbor->id);
                    addedNew = true;
                }
            }
        }

        // Sort and add new neighbors
        std::sort(newNeighbors.begin(), newNeighbors.end(), CompDistanceToRefImage(navNode->image, distHelper));
        R.insert(R.end(), newNeighbors.begin(), newNeighbors.end());

        if (!addedNew)
        {
            break; // Break if no new neighbors are added
        }
    }

    // Now, R has the closest 'candidates' neighbors, sort them by distance to the query image.
    std::sort(R.begin(), R.end(), CompDistanceToRefImage(query, distHelper));

    std::vector<Neighbor> kNN;
    for (int i = 0; i < k && i < R.size(); i++)
    {
        kNN.push_back(Neighbor(R[i], distHelper->calculate(R[i], query)));
    }

    return kNN;
}
