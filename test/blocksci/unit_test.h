//
//  unit_test.h
//  blocksci
//
//  Created by Malte Möser on 4/27/20.
//

#ifndef unit_test_h
#define unit_test_h

#include "gtest/gtest.h"

#include <blocksci/blocksci.hpp>

#include <string>

extern std::string configFilePath;

class BlockSciTest : public ::testing::Test {

protected:
  blocksci::Blockchain chain;

  BlockSciTest() : chain(configFilePath) {
  }
};

#endif /* unit_test_h */
