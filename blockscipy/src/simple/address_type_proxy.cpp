//
//  address_type_proxy.cpp
//  blocksci
//
//  Created by Harry Kalodner on 9/22/18.
//
//

#include "proxy/basic.hpp"
#include "proxy/comparison.hpp"
#include "proxy/equality.hpp"
#include "proxy/optional.hpp"
#include "proxy/range.hpp"
#include "proxy_py.hpp"
#include "simple_proxies.hpp"

#include <blocksci/address/equiv_address.hpp>
#include <blocksci/chain/block.hpp>
#include <blocksci/cluster/cluster.hpp>
#include <blocksci/core/address_types.hpp>

void addAddressTypeProxyMethods(AllProxyClasses<blocksci::AddressType::Enum> &cls) {
  cls.applyToAll(AddProxyMethods{});
  addProxyEqualityMethods(cls.base);
  addProxyComparisonMethods(cls.base);
  addProxyOptionalMethods(cls.optional);
}
