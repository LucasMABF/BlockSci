//
//  range_map_optional_range.cpp
//  blocksci
//
//  Created by Harry Kalodner on 9/22/18.
//
//

#include "generic_proxy.hpp"
#include "range_map.hpp"
#include "range_map_optional_impl.hpp"

#include <blocksci/address/equiv_address.hpp>
#include <blocksci/chain/block.hpp>
#include <blocksci/chain/transaction.hpp>
#include <blocksci/chain/input.hpp>
#include <blocksci/chain/output.hpp>
#include <blocksci/cluster/cluster.hpp>
#include <pybind11/pybind11.h>
#include <blocksci/scripts/script_variant.hpp>

void applyProxyMapOptionalFuncsCore(pybind11::class_<IteratorProxy, GenericProxy> &cl) {
	using namespace blocksci;
	cl
	.def("_map_optional", mapOptional<Block>)
	.def("_map_optional", mapOptional<Transaction>)
	.def("_map_optional", mapOptional<Input>)
	.def("_map_optional", mapOptional<Output>)
	.def("_map_optional", mapOptional<AnyScript>)
	.def("_map_optional", mapOptional<EquivAddress>)
	.def("_map_optional", mapOptional<Cluster>)
	.def("_map_optional", mapOptional<TaggedCluster>)
	.def("_map_optional", mapOptional<TaggedAddress>)
	;
}
