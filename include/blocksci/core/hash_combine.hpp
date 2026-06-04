//
//  hash_combine.hpp
//  blocksci
//
//  Created by Harry Kalodner on 3/10/17.
//  Copyright © 2017 Harry Kalodner. All rights reserved.
//

#ifndef blocksci_core_hash_combine_hpp
#define blocksci_core_hash_combine_hpp

#include <blocksci/blocksci_export.h>

#include <cstddef>
#include <cstdint>
#include <functional>

namespace blocksci {
  template <class T> inline void BLOCKSCI_EXPORT hash_combine(std::size_t &seed, const T &v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
} // namespace blocksci

#endif /* blocksci_core_hash_combine_hpp */
