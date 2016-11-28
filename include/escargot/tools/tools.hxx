#pragma once

#include <utility>
#include <algorithm>
#include <vector>
#include <functional>

namespace escargot {
namespace tools {
    
typedef std::pair<size_t,size_t> coordinate_type;


// factorial and ncr

template<class int_type> int_type factorial(int_type n) {
    return (n == 0 || n == 1) ? 1 : factorial(n - 1) * n;
}



// calculated with dynamic approach
template<class int_type> int_type nCr(int_type n, int_type r) {
    
    size_t i, j;

    int_type row[101];
    std::fill(row, row+101, 0);

    row[0] = 1;

    for( i = 1; i <= n; i++) {
        for( j = i; j > 0; j--) {
            row[j] += row[j-1];
        }
    }
    return row[r];
}


// sorting function (we sort in DECREASING order !)
template<class value_type>
bool sortByValDecreasing(std::pair<coordinate_type,value_type> first, std::pair<coordinate_type,value_type> second )
{
    return (first >= second) ? false : true;
}

// returns the r combinations of the input
template<class iterator_type>
class combination_generator {
    
    std::vector<bool> use;

    iterator_type first, last;
    size_t r;

public:

    combination_generator(iterator_type first_, iterator_type last_, size_t r_) : first(first_), last(last_), r(r_) {
        
        use.resize( std::distance(first, last), false );
        if( r > use.size() )
            throw std::domain_error("can't select more elements than exist for combination");
        
        //std::fill(use.end()-r, use.end(), true);
        std::fill(use.begin(), use.begin()+r, true);

        }

    template<class output_iterator> bool
    operator()(output_iterator result) {
        
        iterator_type c = first;
        for( size_t i  = 0; i < use.size(); ++i, ++c) {
            
            if( use[i] )
            {
                *(result++) = *c;
            }
        }
        return std::next_permutation(use.begin(), use.end(),std::greater<bool>());
    }
};

template<class iterator_type> combination_generator<iterator_type>
make_combination_generator(iterator_type first, iterator_type last, size_t r) { 
    return combination_generator<iterator_type>(first, last, r); 
}

template<class iterator_type> std::vector<std::vector<typename std::iterator_traits<iterator_type>::value_type>>
get_combinations(iterator_type first, iterator_type last, size_t r)
{
    // get the value type of the iterator type
    typedef typename std::iterator_traits<iterator_type>::value_type element_type;

    auto combination_generator = make_combination_generator(first, last, r);

    // initialize the return vector
    std::vector<std::vector<element_type>> ret; 
    
    std::vector<element_type> current(r);

    while( combination_generator(current.begin()) )
        ret.push_back(current);

    ret.push_back(current);

    return ret;
}




} // namespace tools
} // namespace escargot
