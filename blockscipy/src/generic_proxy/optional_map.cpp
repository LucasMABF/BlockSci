//
//  optional_map.cpp
//  blocksci
//
//  Created by Harry Kalodner on 10/31/18.
//
//

#include "optional_map.hpp"

#include "generic_proxy.hpp"
#include "proxy.hpp"
#include "proxy_utils.hpp"

#include <blocksci/chain/block.hpp>
#include <blocksci/chain/input.hpp>
#include <blocksci/chain/output.hpp>
#include <blocksci/chain/transaction.hpp>
#include <blocksci/core/address_types.hpp>
#include <blocksci/core/bitcoin_uint256.hpp>
#include <blocksci/scripts/script_variant.hpp>

#include <range/v3/utility/optional.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>

#include <chrono>
#include <cstdint>

using namespace blocksci;

namespace {
  template <typename R> Proxy<ranges::optional<R>> mapOptional(OptionalProxy &p, Proxy<R> &p2) {
    return liftGeneric(p, [p2](auto &&opt) -> ranges::optional<R> {
      if (opt) {
        return p2(opt->toAny());
      } else {
        return ranges::nullopt;
      }
    });
  }

  template <typename R>
  Proxy<ranges::optional<R>> mapOptionalOptional(OptionalProxy &p, Proxy<ranges::optional<R>> &p2) {
    return liftGeneric(p, [p2](auto &&opt) -> ranges::optional<R> {
      if (opt) {
        return p2(opt->toAny());
      } else {
        return ranges::nullopt;
      }
    });
  }

  template <typename T> void addOptionalProxyMapMethodsImpl(pybind11::class_<OptionalProxy, RangeProxy> &cl) {
    cl.def("_map", mapOptional<T>).def("_map", mapOptionalOptional<T>);
  }
} // namespace

void addOptionalProxyMapMethods(pybind11::class_<OptionalProxy, RangeProxy> &cl) {
  addOptionalProxyMapMethodsImpl<Block>(cl);
  addOptionalProxyMapMethodsImpl<Transaction>(cl);
  addOptionalProxyMapMethodsImpl<Input>(cl);
  addOptionalProxyMapMethodsImpl<Output>(cl);
  addOptionalProxyMapMethodsImpl<AnyScript>(cl);
  addOptionalProxyMapMethodsImpl<AddressType::Enum>(cl);
  addOptionalProxyMapMethodsImpl<int64_t>(cl);
  addOptionalProxyMapMethodsImpl<bool>(cl);
  addOptionalProxyMapMethodsImpl<std::chrono::system_clock::time_point>(cl);
  addOptionalProxyMapMethodsImpl<uint256>(cl);
  addOptionalProxyMapMethodsImpl<uint160>(cl);
  addOptionalProxyMapMethodsImpl<pybind11::bytes>(cl);
}
