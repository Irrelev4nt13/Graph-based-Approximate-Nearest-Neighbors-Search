#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <chrono>
#include "Image.hpp"
#include "Utils.hpp"
#include "Gnns.hpp"
#include "Mrng.hpp"
#include "FileParser.hpp"
#include "BruteForce.hpp"
#include "ImageDistance.hpp"

int main(int argc, char const *argv[])
{
    std::string inputFile;
    std::string queryFile;
    int graphNN;
    int expansions;
    int restarts;
    int numNn;
    int l;
    int m;
    bool show = false;

    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-d"))
            inputFile = std::string(argv[i + 1]);
        else if (!strcmp(argv[i], "-q"))
            queryFile = std::string(argv[i + 1]);
        else if (!strcmp(argv[i], "-k"))
            graphNN = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "-E"))
            expansions = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "-R"))
            restarts = atof(argv[i + 1]);
        else if (!strcmp(argv[i], "-N"))
            numNn = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "-l"))
            l = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "-m"))
            m = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "-s"))
            show = true;
    }

    // Parse file and get the images
    FileParser inputParser(inputFile);
    const std::vector<ImagePtr> input_images = inputParser.GetImages();

    // Get query images
    FileParser queryParser(queryFile);
    std::vector<ImagePtr> query_images = queryParser.GetImages();

    // Configure the metric used for the lsh program
    ImageDistance::setMetric(DistanceMetric::EUCLIDEAN);

    // Initialize Graphs
    GraphAlgorithm *algorithm = nullptr;

    double exp_tTotalApproximate = 0;
    double exp_tTotalTrue = 0;
    double exp_AAF = 0;
    double exp_MAF = 0;

    // for (int experiment = 0; experiment < 3; experiment++)
    // {
    if (m == 1)
        // GNNS initialization
        algorithm = new GNNS(input_images, graphNN, expansions, restarts, numNn);
    else if (m == 2)
        // MRNG initialization
        algorithm = new Mrng(input_images, numNn, l);
    auto tTotalApproximate = std::chrono::nanoseconds(0);
    auto tTotalTrue = std::chrono::nanoseconds(0);
    double AAF = 0;
    double MAF = -1;
    int found = 0;
    for (int q = 0; q < 1000; q++)
    {
        ImagePtr query = query_images[q];

        startClock();
        std::vector<Neighbor> approx_vector = algorithm->Approximate_kNN(query);
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
    if (m == 1 && show)
        std::cout << "R:" << restarts << std::endl;
    else if (m == 2 && show)
        std::cout << "l:" << l << std::endl;
    std::cout << "tAverageApproximate:" << tTotalApproximate.count() * 1e-9 / 1000 << std::endl;
    std::cout << "tAverageTrue:" << tTotalTrue.count() * 1e-9 / 1000 << std::endl;
    std::cout << "AAF:" << AAF / found << std::endl;
    std::cout << "MAF:" << MAF; // << std::endl << std::endl;

    // exp_tTotalApproximate += tTotalApproximate.count() * 1e-9 / 1000;
    // exp_tTotalTrue += tTotalTrue.count() * 1e-9 / 1000;
    // exp_AAF += AAF / found;
    // exp_MAF += MAF;
    // }

    // // if (m == 1 && show)
    // //     std::cout << "R:" << restarts << std::endl;
    // // else if (m == 2 && show)
    // //     std::cout << "l:" << l << std::endl;
    // // std::cout << "exp_tAverageApproximate:" << exp_tTotalApproximate / 3 << std::endl;
    // // std::cout << "exp_tAverageTrue:" << exp_tTotalTrue / 3 << std::endl;
    // // std::cout << "exp_AAF:" << exp_AAF / 3 << std::endl;
    // // std::cout << "exp_MAF:" << exp_MAF / 3;
    // if (m == 1 && show)
    //     std::cout << "R:" << restarts << std::endl;
    // else if (m == 2 && show)
    //     std::cout << "l:" << l << std::endl;
    // std::cout << "tAverageApproximate:" << exp_tTotalApproximate / 3 << std::endl;
    // std::cout << "tAverageTrue:" << exp_tTotalTrue / 3 << std::endl;
    // std::cout << "AAF:" << exp_AAF / 3 << std::endl;
    // std::cout << "MAF:" << exp_MAF / 3;
    delete algorithm;

    return EXIT_SUCCESS;
}
