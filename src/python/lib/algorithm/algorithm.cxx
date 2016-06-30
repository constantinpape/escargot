#include <algorithm>
#include <iostream>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "../converter.hxx"

#include "escargot/algorithm/affinity_watershed.hxx"
#include "escargot/marray/marray.hxx"
#include "escargot/marray/py_marray.hxx"

namespace py = pybind11;

namespace escargot {
namespace algorithm {

// TODO need to revisit this!
// probably some unnecessary copys!
template<class value_type, class label_type> void export_affinity_watershed2d(py::module & algorithm_module) {
    
    algorithm_module.def("affinity_watershed_2d",
            [&](andres::PyView<value_type> inArray, value_type const upper_threshold, value_type const lower_threshold,
                size_t const size_threshold, value_type const region_threshold) {
                
                std::cout << "Blub" << std::endl;
                
                // TODO assert correct shape = (x,y,2)
                
                size_t s_x = inArray.shape(0);
                size_t s_y = inArray.shape(1);

                size_t s[] = {s_x, s_y};

                //NumpyArray<label_type> return_view( {s_x,s_y}, {static_cast<size_t>(1),s_x} );
                andres::PyView<label_type> return_view( s, s + 2 );
                std::cout << "Blub2" << std::endl;
                //std::fill(return_view.begin(), return_view.end(), 0);
                std::cout << "Blub3" << std::endl;
                //auto d = &(*(return_view.end() - 1)) - &(*return_view.begin());

                //std::cout << "Mem Allocated: " << d << " size: " << return_view.size() << std::endl;

                graphWatershed2d<value_type, label_type>(inArray, upper_threshold, lower_threshold, size_threshold, region_threshold,
                    return_view);

                std::cout << "Made it till here" << std::endl;

                return return_view;
            } );
            //"Run affinity watershed on 2d, 2 channel input")


}

void initSubmoduleAlgorithm(py::module & escargotModule) {

    auto algorithm_module = escargotModule.def_submodule("algorithm", "graph algorithm submodule");
    
    export_affinity_watershed2d<double, uint32_t>(algorithm_module);
    //TODO try if this templating works
    //export_affinity_watershed2d<double, uint64_t>(algorithm_module);
    //
    //export_affinity_watershed2d<float, uint64_t>(algorithm_module);
    //export_affinity_watershed2d<float, uint32_t>(algorithm_module);

}


}
}
