//
//  uint256_proxy.cpp
//  blocksci
//
//  Created by Harry Kalodner on 9/22/18.
//
//

#include "proxy/basic.hpp"
#include "proxy/equality.hpp"
#include "proxy/optional.hpp"
#include "proxy/range.hpp"
#include "proxy_py.hpp"
#include "simple_proxies.hpp"

#include <blocksci/address/equiv_address.hpp>
#include <blocksci/chain/block.hpp>
#include <blocksci/cluster/cluster.hpp>
#include <blocksci/core/bitcoin_uint256.hpp>

void addUint256ProxyMethods(AllProxyClasses<blocksci::uint256> &cls) {
  cls.applyToAll(AddProxyMethods{});
  addProxyEqualityMethods(cls.base);
  addProxyOptionalMethods(cls.optional);
}