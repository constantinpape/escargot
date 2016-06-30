#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "escargot/marray/marray.hxx"
#include "escargot/marray/py_marray.hxx"
#include "escargot/marray/py_marray.hxx"

#include "escargot/algorithm/affinity_watershed.hxx"

namespace py = pybind11;


namespace escargot {
namespace algorithm {

// TODO need to revisit this!
//template<class value_type, class label_type> void export_algorithm(py::module & mod) {
void export_algorithm(py::module & mod) {
    
    mod.def("affinity_watershed_2d",
            [&](marray::PyView<double> inArray, double const upper_threshold, double const lower_threshold,
                size_t const size_threshold, double const region_threshold) {
                
                std::cout << "Blub" << std::endl;
                
                // TODO assert correct shape = (x,y,2)
                
                return graphWatershed2d<double, uint32_t>(inArray, upper_threshold, lower_threshold, size_threshold, region_threshold);
            },
            "Run affinity watershed on 2d, 2 channel input");
}

} // namespace algorithm
} // namespace escargot


PYBIND11_PLUGIN(escargot_module) {
    py::module escargotModule("escargot_module", "escargot python bindings");

    using namespace escargot;

    algorithm::export_algorithm(escargotModule);

    return escargotModule.ptr();
}
