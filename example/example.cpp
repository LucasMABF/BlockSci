//
//  main.cpp
//  blocksci-test
//
//  Created by Harry Kalodner on 1/3/17.
//  Copyright © 2017 Harry Kalodner. All rights reserved.
//

#include <blocksci/address/address.hpp>
#include <blocksci/blocksci.hpp>
#include <blocksci/chain/algorithms.hpp>
#include <blocksci/chain/block.hpp>
#include <blocksci/chain/blockchain.hpp>
#include <blocksci/chain/transaction.hpp>
#include <blocksci/chain/transaction_range.hpp>
#include <blocksci/heuristics/taint.hpp>
#include <blocksci/script.hpp>

#include <range/v3/all.hpp>
#include <range/v3/range_for.hpp>

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_map>

using namespace blocksci;

int main(int argc, const char *argv[]) {

  Blockchain chain(argv[1]);

  int64_t blockCount = 0;
  int64_t txCount = 0;
  int64_t inputCount = 0;
  int64_t outputCount = 0;

  for (auto block : chain) {
    blockCount += 1;
    RANGES_FOR(auto tx, block) {
      txCount += 1;
      inputCount += tx.inputCount();
      outputCount += tx.outputCount();
    }
  }

  std::cout << "Chain contains:" << std::endl;
  std::cout << blockCount << " blocks" << std::endl;
  std::cout << txCount << " transactions" << std::endl;
  std::cout << inputCount << " inputs" << std::endl;
  std::cout << outputCount << " outputs" << std::endl;

  return 0;
}
