//
//  algorithms.hpp
//  blocksci
//
//  Created by Harry Kalodner on 11/18/17.
//

#ifndef chain_algorithms_hpp
#define chain_algorithms_hpp

#include <blocksci/blocksci_export.h>
#include <blocksci/chain/chain_fwd.hpp>
#include <blocksci/chain/output.hpp>
#include <blocksci/chain/input.hpp>
#include <blocksci/chain/output.hpp>
#include <blocksci/chain/transaction.hpp>

#include <range/v3/range_for.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/transform.hpp>

#include <cstdint>
#include <type_traits>
#include <utility>

namespace blocksci {
    namespace internal {
        template <typename B, typename = void> struct range_value_or_void {
            using type = void;
        };

        template <typename B> struct range_value_or_void<B, std::void_t<ranges::range_value_t<B>>> {
            using type = ranges::range_value_t<B>;
        };

        template <typename B> using range_value_or_void_t = typename range_value_or_void<B>::type;

    } // namespace internal

    template <typename B, typename T>
    inline constexpr bool isRangeOf = std::is_same_v<internal::range_value_or_void_t<B>, T>;

    template <typename B> inline constexpr bool isOutputPointerRange = isRangeOf<B, OutputPointer>;

    template <typename B> inline constexpr bool isInputRange = isRangeOf<B, Input>;

    template <typename B> inline constexpr bool isOutputRange = isRangeOf<B, Output>;

    template <typename B> inline constexpr bool isTxRange = isRangeOf<B, Transaction>;

    template <typename B> inline constexpr bool isBlockRange = isRangeOf<B, Block>;

    template <typename B> inline constexpr bool isTx = std::is_same_v<B, Transaction>;

    template <typename B> inline constexpr bool isBlockchain = std::is_same_v<B, Blockchain>;

    template <typename B> inline constexpr bool isOptionalInputRange = isRangeOf<B, ranges::optional<Input>>;

    template <typename B> inline constexpr bool isOptionalOutputRange = isRangeOf<B, ranges::optional<Output>>;

    template <typename B> inline constexpr bool isOptionalTxRange = isRangeOf<B, ranges::optional<Transaction>>;

    template <typename B> inline constexpr bool isOptionalBlockRange = isRangeOf<B, ranges::optional<Block>>;

    template<typename T>
    struct fail_helper : std::false_type
    { };

    template <typename B, std::enable_if_t<isTxRange<B> || isBlockRange<B>, int> = 0>
    inline decltype(auto) BLOCKSCI_EXPORT txes(B &&b) {
        if constexpr (isTxRange<B>) {
            return std::forward<B>(b);
        } else {
            return std::forward<B>(b) | ranges::views::join;
        }
    }
    
    inline auto BLOCKSCI_EXPORT inputs(const Transaction &tx) {
        return tx.inputs();
    }
    
    inline auto BLOCKSCI_EXPORT inputs(Transaction &tx) {
        return tx.inputs();
    }

    template <typename B, std::enable_if_t<isInputRange<B> || isTxRange<B> || isBlockRange<B>, int> = 0>
    inline decltype(auto) BLOCKSCI_EXPORT inputs(B &&b) {
        if constexpr (isInputRange<B>) {
            return std::forward<B>(b);
        } else {
            return txes(std::forward<B>(b)) |
                   ranges::views::transform([](const Transaction &tx) { return tx.inputs(); }) | ranges::views::join;
        }
    }
    
    inline auto BLOCKSCI_EXPORT outputs(const Transaction &tx) {
        return tx.outputs();
    }
    
    inline auto BLOCKSCI_EXPORT outputs(Transaction &tx) {
        return tx.outputs();
    }

    template <typename B, std::enable_if_t<isOutputRange<B> || isTxRange<B> || isBlockRange<B>, int> = 0>
    inline decltype(auto) BLOCKSCI_EXPORT outputs(B &&b) {
        if constexpr (isOutputRange<B>) {
            return std::forward<B>(b);
        } else {
            return txes(std::forward<B>(b)) |
                   ranges::views::transform([](const Transaction &tx) { return tx.outputs(); }) | ranges::views::join;
        }
    }

    template <typename B, std::enable_if_t<isOutputPointerRange<B>, int> = 0>
    inline auto BLOCKSCI_EXPORT outputs(B &&b, DataAccess &access) {
        return std::forward<B>(b) |
               ranges::views::transform([&access](const OutputPointer &pointer) { return Output(pointer, access); });
    }
    
    template <typename T>
    inline auto BLOCKSCI_EXPORT outputsUnspent(T && t) {
        return outputs(std::forward<T>(t)) | ranges::views::remove_if([](const Output &output) { return output.isSpent(); });
    }
    
    template <typename T>
    inline auto BLOCKSCI_EXPORT outputsSpentBeforeHeight(T && t, blocksci::BlockHeight blockHeight) {
        return outputs(std::forward<T>(t)) | ranges::views::filter([=](const Output &output) { return output.isSpent() && output.getSpendingTx()->getBlockHeight() < blockHeight; });
    }
    
    template <typename T>
    inline auto BLOCKSCI_EXPORT outputsSpentAfterHeight(T && t, blocksci::BlockHeight blockHeight) {
        return outputs(std::forward<T>(t)) | ranges::views::filter([=](const Output &output) { return output.isSpent() && output.getSpendingTx()->getBlockHeight() >= blockHeight; });
    }
    
    template <typename T>
    inline auto BLOCKSCI_EXPORT inputsCreatedAfterHeight(T && t, blocksci::BlockHeight blockHeight) {
        return inputs(std::forward<T>(t)) | ranges::views::filter([=](const Input &input) { return input.getSpentTx().getBlockHeight() >= blockHeight; });
    }
    
    template <typename T>
    inline auto BLOCKSCI_EXPORT inputsCreatedBeforeHeight(T && t, blocksci::BlockHeight blockHeight) {
        return inputs(std::forward<T>(t)) | ranges::views::filter([=](const Input &input) { return input.getSpentTx().getBlockHeight() < blockHeight; });
    }

    template <typename T>
    inline auto BLOCKSCI_EXPORT outputsSpentWithinRelativeHeight(T && t, blocksci::BlockHeight difference) {
        return outputs(std::forward<T>(t)) | ranges::views::filter([=](const Output &output) {
            return output.isSpent() && output.getSpendingTx()->getBlockHeight() - output.getBlockHeight() < difference;
        });
    }
    
    template <typename T>
    inline auto BLOCKSCI_EXPORT outputsSpentOutsideRelativeHeight(T && t, blocksci::BlockHeight difference) {
        return outputs(std::forward<T>(t)) | ranges::views::filter([=](const Output &output) {
            return output.isSpent() && output.getSpendingTx()->getBlockHeight() - output.getBlockHeight() >= difference;
        });
    }
    
    template <typename T>
    inline auto BLOCKSCI_EXPORT inputsCreatedWithinRelativeHeight(T && t, blocksci::BlockHeight difference) {
        return inputs(std::forward<T>(t)) | ranges::views::filter([=](const Input &input) {
            return input.blockHeight - input.getSpentTx().getBlockHeight() < difference;
        });
    }
    
    template <typename T>
    inline auto BLOCKSCI_EXPORT inputsCreatedOutsideRelativeHeight(T && t, blocksci::BlockHeight difference) {
        return inputs(std::forward<T>(t)) | ranges::views::filter([=](const Input &input) {
            return input.blockHeight - input.getSpentTx().getBlockHeight() >= difference;
        });
    }
    
    template <typename T>
    inline auto BLOCKSCI_EXPORT outputsOfType(T && t, AddressType::Enum type) {
        return outputs(std::forward<T>(t)) | ranges::views::filter([=](const Output &output) { return output.getType() == type; });
    }
    
    template <typename T>
    inline auto BLOCKSCI_EXPORT inputsOfType(T && t, AddressType::Enum type) {
        return inputs(std::forward<T>(t)) | ranges::views::filter([=](const Input &input) { return input.getType() == type; });
    }
    
    template <typename T>
    inline uint64_t BLOCKSCI_EXPORT inputCount(T && t) {
        auto values = txes(std::forward<T>(t)) | ranges::views::transform([](const Transaction &tx) { return tx.inputCount(); });
        return ranges::accumulate(values, uint64_t{0});
    }
    
    template <typename T>
    inline uint64_t BLOCKSCI_EXPORT outputCount(T && t) {
        auto values = txes(std::forward<T>(t)) | ranges::views::transform([](const Transaction &tx) { return tx.outputCount(); });
        return ranges::accumulate(values, uint64_t{0});
    }
    
    template <typename T>
    inline int64_t BLOCKSCI_EXPORT totalInputValue(T && t) {
        auto values = inputs(std::forward<T>(t)) | ranges::views::transform([](const Input &a) { return a.getValue(); });
        return ranges::accumulate(values, int64_t{0});
    }
    
    template <typename T>
    inline int64_t BLOCKSCI_EXPORT totalOutputValue(T && t) {
        auto values = outputs(std::forward<T>(t)) | ranges::views::transform([](const Output &a) { return a.getValue(); });
        return ranges::accumulate(values, int64_t{0});
    }
    
    inline int64_t BLOCKSCI_EXPORT fee(const Transaction &tx) {
        return tx.fee();
    }

    /** Calculate the total balance of a collection of outputs, optionally only up to a given block height */
    template <typename T>
    inline int64_t BLOCKSCI_EXPORT balance(BlockHeight height, T && t) {
        int64_t value = 0;
        if (height == -1) {
            RANGES_FOR(auto output, t) {
                if (!output.isSpent()) {
                    value += output.getValue();
                }
            }
        } else {
            RANGES_FOR(auto output, t) {
                if (output.getBlockHeight() <= height && (!output.isSpent() || output.getSpendingTx()->getBlockHeight() > height)) {
                    value += output.getValue();
                }
            }
        }
        return value;
    }

    template <typename T>
    inline auto BLOCKSCI_EXPORT feeLessThan(T &t, int64_t value) {
        return txes(t) | ranges::views::filter([=](const Transaction &tx) {
            return fee(tx) < value;
        });
    }

    template <typename T>
    inline auto BLOCKSCI_EXPORT feeGreaterThan(T &t, int64_t value) {
        return txes(t) | ranges::views::filter([=](const Transaction &tx) {
            return fee(tx) > value;
        });
    }
    
    template <typename T>
    inline auto BLOCKSCI_EXPORT fees(T &t) {
        return ranges::views::transform(txes(t), [](Transaction && tx) {
            return fee(tx);
        });
    }
    
    inline double BLOCKSCI_EXPORT feePerByte(const Transaction &tx) {
        return static_cast<double>(fee(tx)) / static_cast<double>(tx.sizeBytes());
    }
    
    template <typename T>
    inline auto BLOCKSCI_EXPORT feesPerByte(T &t) {
        return ranges::views::transform(txes(t), feePerByte);
    }
    
    template <typename T>
    inline int64_t BLOCKSCI_EXPORT totalFee(T &t) {
        return ranges::accumulate(fees(t), int64_t{0});
    }
} // namespace blocksci

#endif /* chain_algorithms_hpp */
