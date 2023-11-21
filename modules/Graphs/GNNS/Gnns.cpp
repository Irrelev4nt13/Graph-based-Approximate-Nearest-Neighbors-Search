#include <vector>
#include <set>

#include "Image.hpp"
#include "Lsh.hpp"
#include "PublicTypes.hpp"
#include "ImageDistance.hpp"
#include "Gnns.hpp"
#include "Utils.hpp"

GNNS::GNNS(const std::vector<ImagePtr> &images, int graphNN, int expansions, int restarts, int numNn)
    : graphNN(graphNN), expansions(expansions), restarts(restarts), numNn(numNn)
{
    // Initialize the general distance
    this->distance = ImageDistance::getInstance();

    // Initialize lsh which will be used to initialize the graph
    Lsh lsh(images, 4, 5, graphNN + 1, 2240, (int)images.size() / 8);

    // In order to avoid multiple reallocs we will resize the vector
    PointsWithNeighbors.resize((int)images.size());

    // We need to initialize our graph, for every image in the input file we will get its neighbors
    // and store them in a 2d vector the first dimension will represent the Image in the input file
    // and the second its neighbors
    for (int i = 0; i < (int)images.size(); i++)
        PointsWithNeighbors[i] = lsh.Approximate_kNN(images[i]);
}

GNNS::~GNNS() {}

std::vector<Neighbor> GNNS::Approximate_kNN(ImagePtr query)
{
    // We are using a set to store the objects efficiently with a custom compare class
    std::set<Neighbor, CompareNeighbor> nearestNeighbors;

    // We will do the same update process for all restarts
    for (int r = 0; r < restarts; r++)
    {
        // find Y_0 uniformly over D
        int Y_prev = IntDistribution(0, (int)PointsWithNeighbors.size() - 1);

        // We will find new points t-1 times because the first one was done outside the loop
        for (int t = 1; t <= 30; t++)
        {
            double min = -1;
            int index = -1;
            // We skip the first neighbor because it is itself with distance 0 and we go until + 1
            // to take all the expanded neighbors
            for (int i = 1; i < expansions + 1; i++)
            {
                // Calculate the distance of the neighbor with the query
                double dist = distance->calculate(PointsWithNeighbors[Y_prev][i].image, query);
                // Update set with S U N(Y_t-1,E,G)
                nearestNeighbors.insert(Neighbor(PointsWithNeighbors[Y_prev][i].image, dist));
                // Find Y_t = argmin_Y_in_N(Y_t-1,E,G) δ(Y,query)
                if (min == -1 || dist < min)
                {
                    min = dist;
                    index = PointsWithNeighbors[Y_prev][i].image->id;
                }
            }
            Y_prev = index;
        }
    }
    // The set is already sorted so we can skip the sorting step
    // Lastly we want to make a vector from those neighbors
    // std::vector<Neighbor> KnearestNeighbors(nearestNeighbors.begin(), nearestNeighbors.end());
    std::vector<Neighbor> KnearestNeighbors(nearestNeighbors.begin(), std::next(nearestNeighbors.begin(), std::min(numNn, static_cast<int>(nearestNeighbors.size()))));
    return KnearestNeighbors;
}