//
//  pubkey_base_script.cpp
//  blocksci
//
//  Created by Harry Kalodner on 4/23/18.
//

#include <blocksci/address/address.hpp>
#include <blocksci/core/bitcoin_uint256.hpp>
#include <blocksci/core/raw_address.hpp>
#include <blocksci/scripts/bitcoin_pubkey.hpp>
#include <blocksci/scripts/pubkey_base_script.hpp>

#include <range/v3/utility/optional.hpp>
#include <range/v3/view/any_view.hpp>
#include <range/v3/view/transform.hpp>

#include <internal/address_index.hpp>
#include <internal/data_access.hpp>

namespace blocksci {

  ranges::optional<CPubKey> PubkeyAddressBase::getPubkey() const {
    if (getData()->hasPubkey) {
      auto &rawPubkey = getData()->pubkey;
      return CPubKey{rawPubkey.begin(), rawPubkey.end()};
    } else {
      return ranges::nullopt;
    }
  }

  uint160 PubkeyAddressBase::getPubkeyHash() const {
    auto pubkey = getPubkey();
    if (pubkey) {
      return pubkey->GetID();
    } else {
      return getData()->address;
    }
  }

  ranges::any_view<Address> PubkeyAddressBase::getIncludingMultisigs() const {
    auto access_ = &getAccess();
    return getAccess().getAddressIndex().getIncludingMultisigs(*this) |
           ranges::views::transform([access_](const RawAddress &raw) { return Address{raw, *access_}; });
  }
} // namespace blocksci
