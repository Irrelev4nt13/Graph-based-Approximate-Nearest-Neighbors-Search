#include <iostream>
#include <vector>
#include <fstream>

#include "Image.hpp"
#include "Utils.hpp"
#include "Lsh.hpp"
#include "GraphsCmdArgs.hpp"
#include "FileParser.hpp"
#include "BruteForce.hpp"
#include "ImageDistance.hpp"
#include "GraphAlgorithm.hpp"
#include "Mrng.hpp"

int main(int argc, char const *argv[])
{
    // Analyze arguments from command line and store them in a simple object
    GraphsCmdArgs args(argc, argv);

    readFilenameIfEmpty(args.inputFile, "input");

    // Parse file and get the images
    FileParser inputParser(args.inputFile);
    const std::vector<ImagePtr> input_images = inputParser.GetImages();

    readFilenameIfEmpty(args.queryFile, "query");

    readFilenameIfEmpty(args.outputFile, "output");
    std::ofstream output_file;

    // window
    int w = 2240;
    int numBuckets = inputParser.GetMetadata().numOfImages / 8; // n / 8

    // Configure the metric used for the lsh program
    ImageDistance::setMetric(DistanceMetric::EUCLIDEAN);

    // Initialize Graphs
    std::string graph_algorithm_name = "";

    GraphAlgorithm *graphAlgorithm;

    if (args.numNn < 1)
    {
        std::cerr << "Error, the number of nearest neighbors has to be positive" << std::endl;
        return EXIT_FAILURE;
    }

    if (args.m == 1)
    {
        // GNNS initialization
        graph_algorithm_name = "GNNS";
    }
    else if (args.m == 2)
    {
        // MRNG initialization
        if (args.l < args.numNn)
        {
            std::cerr << "Error, the number of candidates must be greater or equal to the number of nearest neighbors" << std::endl;
            return EXIT_FAILURE;
        }
        graph_algorithm_name = "MRNG";
        graphAlgorithm = new Mrng(input_images);
    }
    else
    {
        std::cerr << "Error, unknown type of graph" << std::endl;
        return EXIT_FAILURE;
    }

    // Keep reading new query and output files until the user types "exit"
    while (true)
    {
        // Get query images
        FileParser queryParser(args.queryFile);
        std::vector<ImagePtr> query_images = queryParser.GetImages();

        output_file.open(args.outputFile);

        output_file << graph_algorithm_name << std::endl;

        // For each query data point calculate its approximate k nearesest neighbors with the preferable graph algorithm and compare it to brute force
        // for (int q = 0; q < (int)query_images.size(); q++)
        for (int q = 0; q < 10; q++)
        {
            ImagePtr query = query_images[q];

            startClock();
            std::vector<Neighbor> approx_vector = graphAlgorithm->Approximate_kNN(query, 5);
            auto elapsed_lsh = stopClock();

            startClock();
            std::vector<Neighbor> brute_vector = BruteForce(input_images, query, args.numNn);
            auto elapsed_brute = stopClock();

            output_file << "Query: " << query->id << std::endl;

            int limit = approx_vector.size();
            for (int i = 0; i < limit; i++)
            {
                ImagePtr image = approx_vector[i].image;
                double dist = approx_vector[i].distance;

                output_file << "Nearest neighbor-" << i + 1 << ": " << image->id << std::endl
                            << "distanceGraph: " << dist << "\n";

                dist = brute_vector[i].distance;
                output_file << "distanceTrue: " << dist << "\n";
            }

            output_file << "t " << graph_algorithm_name << ": " << elapsed_lsh.count() * 1e-9 << std::endl;
            output_file << "tTrue: " << elapsed_brute.count() * 1e-9 << std::endl;

            output_file << std::endl;
        }

        output_file << std::endl
                    << std::endl
                    << "Total Statistics: " << std::endl;
        output_file << "\ttTotalApproximate: " << std::endl;   // Total Approximate time
        output_file << "\ttTotalTrue: " << std::endl;          // Total True time
        output_file << "\ttAverageApproximate: " << std::endl; // Average Approximate time
        output_file << "\ttAverageTrue: " << std::endl;        // Average True time
        output_file << "\tAAF: " << std::endl;                 // Average Approximation Factor
        output_file << "\tMAF: " << std::endl;                 // Maximum Approximation Factor

        // Read new query and output files.
        args.queryFile.clear();
        std::cout << "Enter new query file, type exit to stop: ";
        std::getline(std::cin, args.queryFile);

        if (args.queryFile == "exit")
            break;

        args.outputFile.clear();
        readFilenameIfEmpty(args.outputFile, "output");

        output_file.close();
    }

    return EXIT_SUCCESS;
}
