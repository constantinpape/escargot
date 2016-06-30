#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "converter.hxx"

namespace py = pybind11;

namespace escargot {
namespace algorithm {
    void initSubmoduleAlgorithm(py::module & );
}   
}

PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

PYBIND11_PLUGIN(_escargot) {
    py::module escargotModule("_escargot", "escargot python bindings");

    // we don't use this (yet)
    //// float and double arrays
    //py::class_<escargot::NumpyArray<float>>(escargotModule, "NumpyArrayFloat");
    //py::class_<escargot::NumpyArray<double>>(escargotModule, "NumpyArrayDouble");
    //
    //// unsigned int arrays
    //py::class_<escargot::NumpyArray<uint32_t>>(escargotModule, "NumpyArrayUInt32");
    //py::class_<escargot::NumpyArray<uint64_t>>(escargotModule, "NumpyArrayUInt64");

    using namespace escargot;
    algorithm::initSubmoduleAlgorithm(escargotModule);
}
