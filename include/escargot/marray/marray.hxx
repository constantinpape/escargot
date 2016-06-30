// wrapping andres::marray to have it in the escargot namespace

#pragma once

#include "andres/marray.hxx"

namespace escargot{
namespace marray{
    using namespace andres;
}
}
