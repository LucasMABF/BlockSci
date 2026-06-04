//
//  address_state.cpp
//  blocksci
//
//  Created by Harry Kalodner on 9/10/17.
//
//

#include "address_state.hpp"

#include "hash_index_creator.hpp"
#include "internal/state.hpp"
#include "parser_configuration.hpp"

#include <blocksci/core/bitcoin_uint256.hpp>
#include <blocksci/core/dedup_address_type.hpp>
#include <blocksci/core/meta.hpp>

#include <internal/address_info.hpp>
#include <wjfilesystem/path.h>

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

namespace {
  static constexpr auto multiAddressFileName = "multi";
  static constexpr auto bloomFileName = "bloom_";
  static constexpr auto scriptCountsFileName = "scriptCounts.txt";
} // namespace

AddressState::AddressState(filesystem::path path_, HashIndexCreator &hashDb)
    : path(std::move(path_)), db(hashDb),
      addressBloomFilters(blocksci::apply(blocksci::DedupAddressType::all(), [&](auto tag) {
        return std::make_unique<AddressBloomFilter<tag>>(path / std::string(bloomFileName));
      })) {
  blocksci::for_each(multiAddressMaps, [&](auto &multiAddressMap) {
    std::stringstream ss;
    ss << multiAddressFileName << "_" << dedupAddressName(multiAddressMap.type) << ".dat";
    multiAddressMap.unserialize((path / ss.str()).str());
  });

  std::ifstream inputFile((path / std::string(scriptCountsFileName)).str());

  if (inputFile) {
    uint32_t value;
    while (inputFile >> value) {
      scriptIndexes.push_back(value);
    }
  } else {
    for (size_t i = 0; i < blocksci::DedupAddressType::size; i++) {
      scriptIndexes.push_back(1);
    }
  }
}

AddressState::~AddressState() {
  blocksci::for_each(multiAddressMaps, [&](auto &multiAddressMap) {
    std::stringstream ss;
    ss << multiAddressFileName << "_" << dedupAddressName(multiAddressMap.type) << ".dat";
    multiAddressMap.serialize((path / ss.str()).str());
  });

  std::ofstream outputFile((path / std::string(scriptCountsFileName)).str());
  for (auto value : scriptIndexes) {
    outputFile << value << " ";
  }
}

uint32_t AddressState::getNewAddressIndex(blocksci::DedupAddressType::Enum type) {
  auto &count = scriptIndexes[static_cast<uint8_t>(type)];
  auto scriptNum = count;
  count++;
  return scriptNum;
}

void AddressState::reset(const blocksci::State &state) {
  reloadBloomFilters();
  scriptIndexes.clear();
  for (auto size : state.scriptCounts) {
    scriptIndexes.push_back(size);
  }
}
