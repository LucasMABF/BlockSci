//
//  range_map_simple_range.cpp
//  blocksci
//
//  Created by Harry Kalodner on 9/22/18.
//
//

#include "generic_proxy.hpp"
#include "python_fwd.hpp"
#include "range_map.hpp"
#include "range_map_simple_impl.hpp"

#include <blocksci/address/equiv_address.hpp>
#include <blocksci/cluster/cluster.hpp>

#include <pybind11/pybind11.h>

void applyProxyMapFuncsScripts(pybind11::class_<RangeProxy, IteratorProxy> &cl) {
  addProxyMapFuncsMethodsScripts<random_access_sized>(cl);
}
