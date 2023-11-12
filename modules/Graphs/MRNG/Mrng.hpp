#include <iostream>
#include <vector>

#include "PublicTypes.hpp"
#include "ImageDistance.hpp"

class Mrng
{
private:
    ImageDistance *distHelper;
    std::vector<ImagePtr> graph;

public:
    Mrng(const std::vector<ImagePtr> &images);
    ~Mrng();
};
