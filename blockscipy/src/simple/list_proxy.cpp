//
//  list_proxy.cpp
//  blocksci
//
//  Created by Harry Kalodner on 9/22/18.
//
//

#include "proxy_py.hpp"
#include "simple_proxies.hpp"
#include "proxy/range.hpp"
#include "proxy/basic.hpp"
#include "proxy/optional.hpp"

#include <blocksci/chain/block.hpp>
#include <blocksci/cluster/cluster.hpp>
#include <blocksci/address/equiv_address.hpp>
#include <pybind11/pytypes.h>

void addListProxyMethods(AllProxyClasses<pybind11::list> &cls) {
	cls.applyToAll(AddProxyMethods{});
	addProxyOptionalMethods(cls.optional);
}