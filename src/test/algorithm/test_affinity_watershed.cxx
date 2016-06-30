// TODO use google test for proper tests
//#include "gtest/gtest.h"

#include <iostream>
#include <random>

#include "escargot/marray/marray.hxx"
#include "escargot/algorithm/affinity_watershed.hxx"

using namespace escargot;

// TODO make proper test case
int main() {
    
    size_t shape_eweights[] = {256,256,2};
    marray::Marray<double> edge_weights(shape_eweights, shape_eweights + 3);
    
    // fill with random values between 0 and 1
    
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.,1.);

    for( auto it = edge_weights.begin(); it != edge_weights.end(); it++)
    {
        *it = distribution(generator);
    }

    double upper_threshold = 0.9;
    double lower_threshold = 0.6;
    size_t size_threshold   = 1;
    double region_threshold = 0.8;

    size_t shape_nweights[] = { edge_weights.shape(0), edge_weights.shape(1)};

    marray::Marray<size_t> segmentation( shape_nweights, shape_nweights + 2);
    
    algorithm::graphWatershed2d<double, size_t>(edge_weights, upper_threshold, lower_threshold, size_threshold, region_threshold, segmentation);

    for( size_t d = 0; d < segmentation.dimension(); d++ )
        std::cout << segmentation.shape(d) << std::endl;

}
