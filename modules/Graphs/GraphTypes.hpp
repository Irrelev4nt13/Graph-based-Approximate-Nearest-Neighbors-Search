#ifndef GRAPH_TYPES_HPP_H
#define GRAPH_TYPES_HPP_H

#include "PublicTypes.hpp"
#include "ImageDistance.hpp"

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

// class Edge
// {
// public:
//     ImagePtr image1;
//     ImagePtr image2;
//     double distance;
//     Edge(ImagePtr image1, ImagePtr image2, double distance)
//         : image1(image1), image2(image2), distance(distance){};
//     ~Edge(){};
// };

// class CompareEdge
// {
// public:
//     bool operator()(const Edge &a, const Edge &b) const
//     {
//         return a.distance < b.distance;
//     }
// };

#endif