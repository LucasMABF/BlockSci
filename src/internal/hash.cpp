//
//  hash.cpp
//  blocksci_devel
//
//  Created by Harry Kalodner on 3/10/17.
//  Copyright © 2017 Harry Kalodner. All rights reserved.
//

#include "hash.hpp"

#include <blocksci/core/bitcoin_uint256.hpp>

#include <cstring>
#include <openssl/evp.h>
#include <stdexcept>


Sha256Stream::Sha256Stream() : ctx(EVP_MD_CTX_new()) {
    if (ctx == nullptr) {
        throw std::runtime_error("EVP_MD_CTX_new failed");
    }
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("EVP_DigestInit_ex failed");
    }
}

Sha256Stream::~Sha256Stream() {
    EVP_MD_CTX_free(ctx);
}

void Sha256Stream::update(const void *data, size_t len) {
    if (EVP_DigestUpdate(ctx, data, len) != 1) {
        throw std::runtime_error("EVP_DigestUpdate failed");
    }
}

blocksci::uint256 Sha256Stream::finalize() {
    blocksci::uint256 hash;
    unsigned int hash_len;
    if (EVP_DigestFinal_ex(ctx, reinterpret_cast<unsigned char *>(&hash), &hash_len) != 1) {
        throw std::runtime_error("EVP_DigestFinal_ex failed");
    }
    return hash;
}

blocksci::uint256 sha256(const uint8_t *data, size_t len) {
    Sha256Stream stream;
    stream.update(data, len);
    return stream.finalize();
}

blocksci::uint256 sha256(const unsigned char *begin, const unsigned char *end) {
    return sha256(static_cast<const uint8_t *>(begin), static_cast<size_t>(end - begin));
}

bool base58_sha256(void *digest, const void *data, size_t datasz) {
    auto hash = sha256(reinterpret_cast<const uint8_t *>(data), datasz);
    memcpy(digest, &hash, sizeof(hash));
    return true;
}

blocksci::uint256 doubleSha256(const char *data, uint64_t len) {
    blocksci::uint256 txHash = sha256(reinterpret_cast<const uint8_t *>(data), len);
    txHash = sha256(reinterpret_cast<const uint8_t *>(&txHash), sizeof(blocksci::uint256));
    return txHash;
}

blocksci::uint160 ripemd160(const char *data, uint64_t len) {
    blocksci::uint160 hash;
    unsigned int hash_len;
    if (EVP_Digest(data, len, reinterpret_cast<unsigned char *>(&hash), &hash_len, EVP_ripemd160(), nullptr) != 1) {
        throw std::runtime_error("EVP_Digest failed");
    }
    return hash;
}

blocksci::uint160 hash160(const void *data, uint64_t len) {
    auto hashed = sha256(reinterpret_cast<const uint8_t *>(data), len);
    return ripemd160(reinterpret_cast<const char *>(&hashed), sizeof(hashed));
}
