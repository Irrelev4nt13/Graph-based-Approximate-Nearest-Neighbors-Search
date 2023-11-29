#include <iostream>
#include <vector>
#include <fstream>

#include "Image.hpp"
#include "Utils.hpp"
#include "Lsh.hpp"
#include "LshCmdArgs.hpp"
#include "FileParser.hpp"
#include "BruteForce.hpp"
#include "ImageDistance.hpp"

int main(int argc, char const *argv[])
{
    int numHashFuncs;
    int numHtables;
    int numNn;
    std::string inputFile;
    std::string queryFile;
    int w;
    bool show = false;

    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-d"))
            inputFile = std::string(argv[i + 1]);
        else if (!strcmp(argv[i], "-q"))
            queryFile = std::string(argv[i + 1]);
        else if (!strcmp(argv[i], "-k"))
            numHashFuncs = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "-L"))
            numHtables = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "-N"))
            numNn = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "-w"))
            w = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "-s"))
            show = true;
    }

    FileParser inputParser(inputFile);
    const std::vector<ImagePtr> input_images = inputParser.GetImages();

    FileParser queryParser(queryFile);
    std::vector<ImagePtr> query_images = queryParser.GetImages();

    int numBuckets = inputParser.GetMetadata().numOfImages / 8;

    ImageDistance::setMetric(DistanceMetric::EUCLIDEAN);

    Lsh lsh(input_images, numHashFuncs, numHtables, numNn, w, numBuckets);

    auto tTotalApproximate = std::chrono::nanoseconds(0);
    auto tTotalTrue = std::chrono::nanoseconds(0);
    double AAF = 0;
    double MAF = -1;
    int found = 0;
    for (int q = 0; q < 1000; q++)
    {
        ImagePtr query = query_images[q];

        startClock();
        std::vector<Neighbor> approx_vector = lsh.Approximate_kNN(query);
        auto elapsed_graph = stopClock();
        tTotalApproximate += elapsed_graph;

        startClock();
        std::vector<Neighbor> brute_vector = BruteForce(input_images, query, numNn);
        auto elapsed_brute = stopClock();
        tTotalTrue += elapsed_brute;
        // std::cout << "Query: " << query->id << std::endl;
        int limit = approx_vector.size();
        for (int i = 0; i < limit; i++)
        {
            ImagePtr image = approx_vector[i].image;
            double aproxDist = approx_vector[i].distance;

            // std::cout << "Nearest neighbor-" << i + 1 << ": " << image->id << std::endl
            //           << "distanceApproximate: " << aproxDist << "\n";

            double trueDist = brute_vector[i].distance;
            // std::cout << "distanceTrue: " << trueDist << "\n";

            if (aproxDist / trueDist > MAF || MAF == -1)
                MAF = aproxDist / trueDist;

            AAF += aproxDist / trueDist;
        }
        found += limit;
        // std::cout << "tApprox: " << elapsed_graph.count() * 1e-9 << std::endl;
        // std::cout << "tTrue: " << elapsed_brute.count() * 1e-9 << std::endl;
        // std::cout << std::endl;
    }
    if (show)
        std::cout << "w:" << w << std::endl;
    std::cout << "tAverageApproximate:" << tTotalApproximate.count() * 1e-9 / 1000 << std::endl;
    std::cout << "tAverageTrue:" << tTotalTrue.count() * 1e-9 / 1000 << std::endl;
    std::cout << "AAF:" << AAF / found << std::endl;
    std::cout << "MAF:" << MAF; // << std::endl << std::endl;

    return EXIT_SUCCESS;
}
