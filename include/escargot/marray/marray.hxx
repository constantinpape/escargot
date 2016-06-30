// wrapping andres::marray to have it in the escargot namespace

#pragma once

#define HAVE_CPP11_INITIALIZER_LISTS
#include "andres/marray.hxx"
#include "py_marray.hxx"

namespace escargot{
namespace marray{
    using namespace andres;
}
}
