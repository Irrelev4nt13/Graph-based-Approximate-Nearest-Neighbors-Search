#ifndef GRAPH_TYPES_HPP_H
#define GRAPH_TYPES_HPP_H

#include "PublicTypes.hpp"
#include "ImageDistance.hpp"
#include "Lsh.hpp"

class CompDistanceToRefImage
{
private:
    ImagePtr referenceImage;
    ImageDistance *distHelper;

public:
    CompDistanceToRefImage(ImagePtr refImage, ImageDistance *helper)
        : referenceImage(refImage), distHelper(helper) {}

    bool operator()(const ImagePtr &img1, const ImagePtr &img2) const
    {
        double dist1 = distHelper->calculate(referenceImage, img1);
        double dist2 = distHelper->calculate(referenceImage, img2);
        return dist1 < dist2;
    }
};

class CompDistanceWithLsh
{
private:
    ImagePtr referenceImage;
    Lsh *lshApproximator;

public:
    CompDistanceWithLsh(ImagePtr refImage, Lsh *lshApproximator)
        : referenceImage(refImage), lshApproximator(lshApproximator) {}

    bool operator()(const ImagePtr &img1, const ImagePtr &img2) const
    {
        lshApproximator->Approximate_kNN(img1);
    }
};

#endif