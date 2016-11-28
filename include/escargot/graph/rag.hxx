#pragma once

#include <vector>
#include <utility>

#include "escargot/marray/marray.h"

namespace escargot {
namespace graph {

template<class label_type>
class Rag {

public:
    
    typedef std::pair<label_type, label_type> edge_type;

    Rag();

    // extract all the edges from a (sub) volume
    std::vector<edge_type> edgesFromLabelVolume(marray::View const & labelVolume) const;

    // merge edges from different subvolumes
    std::vector<edge_type> mergeEdgesFromSubvolumes(std::vector<std::vector<edge_type>> const & subEdges) const;

    // set edges
    void setEdges(std::vector<edge_type> edges);

    // extract edge features from a (sub) volume 
    template<class value_type>
    std::vector<value_type> featuresFromVolume(marray::View const & labelVolume, marray::View const & values) const;

    template<class value_type>
    std::vector<value_type> mergeFeaturesFromSubvolumes(std::vector<std::vector<value_type>>) const;


private:

    std::vector<edge_type> edges_;


};


} // namespace graph
} // namespace escargot
