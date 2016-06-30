#pragma once

#include<utility>

namespace escargot {
namespace tools {
    
typedef std::pair<size_t,size_t> coordinate_type;

// sorting function (we sort in DECREASING order !)
template<class value_type>
bool sortByValDecreasing(std::pair<coordinate_type,value_type> first, std::pair<coordinate_type,value_type> second )
{
    return (first >= second) ? false : true;
}

}
}
