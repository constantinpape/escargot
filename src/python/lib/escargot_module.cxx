#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "escargot/marray/marray.hxx"

#include "escargot/algorithm/affinity_watershed.hxx"

namespace py = pybind11;


namespace escargot {
namespace algorithm {

// TODO need to revisit this!
template<class value_type, class label_type> 
void export_algorithm(py::module & mod) {
    
    mod.def("affinity_watershed_2d",
            [&](marray::PyView<value_type> inArray, value_type const upper_threshold, value_type const lower_threshold,
                size_t const size_threshold, value_type const region_threshold) {
                
                size_t shape[] = {inArray.shape(0), inArray.shape(1)};
                marray::PyView<label_type> ret(shape, shape + 2);
                std::fill(ret.begin(), ret.end(), 0);
                
                // TODO assert correct shape = (x,y,2)
                
                // release the gil
                py::gil_scoped_release release;
                //return graphWatershed2d<value_type, label_type>(inArray, upper_threshold, lower_threshold, size_threshold, region_threshold);
                graphWatershed2d<value_type, label_type>(inArray, upper_threshold, lower_threshold, size_threshold, region_threshold, ret);
                return ret;
            },
            "Run affinity watershed on 2d, 2 channel input");
}

} // namespace algorithm
} // namespace escargot


PYBIND11_PLUGIN(escargot_module) {
    py::module escargotModule("escargot_module", "escargot python bindings");

    using namespace escargot;

    algorithm::export_algorithm<double, uint32_t>(escargotModule);

    return escargotModule.ptr();
}
