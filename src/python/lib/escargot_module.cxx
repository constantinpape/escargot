#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "escargot/marray/marray.hxx"

#include "escargot/algorithm/affinity_watershed.hxx"
#include "escargot/tools/ufd.hxx"

namespace py = pybind11;


namespace escargot {
namespace algorithm {

// TODO need to revisit this!
template<class value_type, class label_type> 
void export_algorithm(py::module & mod) {

    mod.def("affinity_watershed_2d",
            [&](marray::PyView<value_type> inArray, value_type const upper_threshold, value_type const lower_threshold, size_t const size_threshold, value_type const region_threshold) {
                
                std::cout << "Starting affinity watershed" << std::endl;
    
                size_t shape[] = {inArray.shape(0), inArray.shape(1)};
                marray::PyView<label_type> ret(shape, shape + 2);
                std::fill(ret.begin(), ret.end(), 0);
                
                // TODO assert correct shape = (x,y,2)
                
                // release the gil
                py::gil_scoped_release release;
                //return graphWatershed2d<value_type, label_type>(inArray, upper_threshold, lower_threshold, size_threshold, region_threshold);
                graphWatershed2d<value_type, label_type>(inArray, upper_threshold, lower_threshold, size_threshold, region_threshold, ret);
                return ret;
            
            }, "Run affinity watershed on 2d, 2 channel input");

    
    mod.def("get_node_weights",
            [&](marray::PyView<value_type> inArray, value_type const upper_threshold, value_type const lower_threshold, bool const ignore_border) {
                
                size_t shape[] = {inArray.shape(0), inArray.shape(1)};
                marray::PyView<value_type> ret(shape, shape + 2);
                std::fill(ret.begin(), ret.end(), 0.);
                nodeWeightsFromEdgeWeights<value_type>(inArray, upper_threshold, lower_threshold, ret, ignore_border);
                return ret;
            
            } );

    
    mod.def("run_graph_watershed",
            [&](marray::PyView<value_type> edge_weights, marray::PyView<value_type> node_weights, bool const ignore_border) { 
                
                size_t shape[] = {edge_weights.shape(0), edge_weights.shape(1)};
                marray::PyView<label_type> ret(shape, shape + 2);
                std::fill(ret.begin(), ret.end(), 0);
                py::gil_scoped_release release;
                runGraphWatershed2d<value_type,label_type>(edge_weights, node_weights, ret, ignore_border);
                return ret;
            
            } );

    mod.def("merge_subblocks",
            [&](marray::PyView<label_type> labeling1, marray::PyView<label_type> labeling2,
                std::vector<size_t> range1, std::vector<size_t> range2,
                marray::PyView<label_type> global_labeling) {
                
                std::cout << "Start Merging subblocks" << std::endl;
                return merge_subblocks<label_type>( labeling1, labeling2, range1, range2, global_labeling); 

            });

    // TODO export size filter
    //mod.def("apply_size_filter",
    //        [&](marray::PyView
    //        
    //        );
}

} // namespace algorithm

//namespace tools {
//
//template<class label_type> 
//void export_tools(py::module & mod) {
//
//    py::class_<Ufd<label_type>>(mod, "Ufd")
//        .def( py::init<const label_type>() )
//        .def("find", &Ufd<label_type>::find)
//        .def("merge", &Ufd<label_type>::merge)
//        .def("representativeLabeling", &Ufd<label_type>::representativeLabeling);
//
//}
//} // namespace tools

} // namespace escargot


PYBIND11_PLUGIN(escargot_module) {
    py::module escargotModule("escargot_module", "escargot python bindings");

    using namespace escargot;

    // export algorithms: affinity_ws_2d
    algorithm::export_algorithm<double, uint32_t>(escargotModule);
    
    //tools::export_tools<uint32_t>(escargotModule);

    return escargotModule.ptr();
}
