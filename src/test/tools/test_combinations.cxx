#include <iostream>
#include <iterator>
#include <vector>
#include <utility>

// TODO proper test case
//#include "gtest/gtest.h"
#include "escargot/tools/tools.hxx"


void print_combination() {
    
    using namespace escargot;

    int numbers[] = {0,1,2};
    
    auto pair_generator = tools::make_combination_generator(numbers, numbers + 3, 2);

    std::ostream_iterator<int> out(std::cout, " ");

    while( pair_generator(out) )
        std::cout << '\n';
        
    std::cout << '\n';

    //for( auto p : pairs)
    //    std::cout << p << std::endl;

}

void test_get_combinations() {
    
    using namespace escargot;

    int numbers[] = {0,1,2};

    auto pairs = tools::get_combinations(numbers, numbers+3, 2);

    for( auto pp : pairs ) {
        for( auto p : pp ) 
            std::cout << p << " ";
        std::cout << '\n';
    }
    
    auto triplets = tools::get_combinations(numbers, numbers+3, 3);

    for( auto tt : triplets ) {
        for( auto t : tt  ) 
            std::cout << t << " ";
        std::cout << '\n';
    }

}

void test_ncr() {
    
    using namespace escargot;

    std::cout << "(4,1) " <<tools::nCr<size_t>(4,1) << std::endl;
    std::cout << "(4,4) " <<tools::nCr<size_t>(4,4) << std::endl;
    
    std::cout << "(5,2) " <<tools::nCr<size_t>(5,2) << std::endl;
    std::cout << "(5,3) " <<tools::nCr<size_t>(5,3) << std::endl;
}


int main() {
    print_combination();
    test_get_combinations();
}

