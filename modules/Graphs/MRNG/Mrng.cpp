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

Mrng::Mrng(const std::vector<ImagePtr> &images)
{
    distHelper = ImageDistance::getInstance();

    // brute force. Uncomment to use
    // useBruteForce(images);

    // lsh method. Uncomment to use
    startClock();
    Lsh *lsh = new Lsh(images, 4, 5, (int)images.size(), 2240, (int)images.size() / 8);
    auto lshDuration = stopClock();
    std::cout << "lsh construction finished in: " << lshDuration.count() * 1e-9 << std::endl;

    startClock();

    useLsh(images, lsh);

    auto mrngDuration = stopClock();
    std::cout << "mrng construction finished in: " << mrngDuration.count() * 1e-9 << std::endl;

    delete lsh;
}

void Mrng::useBruteForce(const std::vector<ImagePtr> &images)
{
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

void Mrng::useLsh(const std::vector<ImagePtr> &images, Lsh *lsh)
{
    for (std::size_t i = 0; i < images.size(); i++)
    {
        std::vector<Neighbor> Rp = lsh->Approximate_kNN(images[i]);

        // startClock();

        // Initialize Lp with points that have minimum distance to p (images[i])
        std::vector<ImagePtr> Lp;
        double minApproxDistance = Rp[0].distance;
        for (int j = 0; j < Rp.size(); j++)
        {
            if (Rp[j].distance != minApproxDistance)
            {
                break;
            }
            Lp.push_back(Rp[j].image);
        }

        for (const auto &r : Rp)
        {
            if (std::find(Lp.begin(), Lp.end(), r.image) != Lp.end())
            {
                continue;
            }

            bool condition = true;
            for (const auto &t : Lp)
            {
                double prDistance = distHelper->calculate(images[i], r.image);
                double ptDistance = distHelper->calculate(images[i], t);
                double rtDistance = distHelper->calculate(r.image, t);

                if (prDistance > ptDistance && prDistance > rtDistance)
                {
                    condition = false;
                    break; // pr is the longest edge, so r is not added to Lp.
                }
            }

            if (condition)
            {
                Lp.push_back(r.image);
            }
        }

        // auto duration = stopClock();

        // std::cout << "duration: " << duration.count() * 1e-9 << std::endl;

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
