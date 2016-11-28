#include <iostream>
#include <vector>

//#include "gtest/gtest.h"
#include "escargot/tools/threadpool.hxx"

// TODO proper test case

int main() {

    using namespace escargot;

    std::vector<size_t> results(10);

    tools::parallel_foreach(-1, 10,
            [&](size_t thread_id, int id){
                results[id] = id;
            });

    for( auto r : results ) 
        std::cout << r << std::endl;
}
