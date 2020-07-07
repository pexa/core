// Copyright (c) 2019-2020 Xenios SEZC
// https://www.veriblock.org
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SRC_VBK_POP_SERVICE_HPP
#define BITCOIN_SRC_VBK_POP_SERVICE_HPP

#include <map>
#include <mutex>
#include <vector>

#include <consensus/validation.h>
#include <script/interpreter.h>
#include <vbk/entity/context_info_container.hpp>

class CBlock;
class CTransaction;
class COutPoint;
class CBlockIndex;
class uint256;
class CScript;

namespace altintegration {
class ValidationState;
struct AltTree;
struct MemPool;
struct PopData;
} // namespace altintegration

namespace Consensus {
struct Params;
}

namespace VeriBlock {

using BlockBytes = std::vector<uint8_t>;
using PoPRewards = std::map<CScript, CAmount>;

struct PopService {
    virtual ~PopService() = default;
    virtual altintegration::AltTree& getAltTree() = 0;
    virtual altintegration::MemPool& getMemPool() = 0;
    virtual PoPRewards getPopRewards(const CBlockIndex& pindexPrev, const Consensus::Params& consensusParams) = 0;
    virtual void addPopPayoutsIntoCoinbaseTx(CMutableTransaction& coinbaseTx, const CBlockIndex& pindexPrev, const Consensus::Params& consensusParams) = 0;
    virtual bool checkCoinbaseTxWithPopRewards(const CTransaction& tx, const CAmount& PoWBlockReward, const CBlockIndex& pindexPrev, const Consensus::Params& consensusParams, BlockValidationState& state) = 0;

    virtual std::vector<BlockBytes> getLastKnownVBKBlocks(size_t blocks) = 0;
    virtual std::vector<BlockBytes> getLastKnownBTCBlocks(size_t blocks) = 0;

    virtual bool acceptBlock(const CBlockIndex& indexNew, BlockValidationState& state) = 0;
    virtual bool addAllBlockPayloads(const CBlockIndex* prevIndex, const CBlock& fullBlock, BlockValidationState& state) = 0;
    virtual bool setState(const uint256& block, altintegration::ValidationState& state) = 0;

    virtual altintegration::PopData getPopData(const CBlockIndex& currentBlockIndex) = 0;
    virtual void removePayloadsFromMempool(const altintegration::PopData& popData) = 0;

    virtual int compareForks(const CBlockIndex& left, const CBlockIndex& right) = 0;

    virtual std::string toPrettyString() const = 0;
};
} // namespace VeriBlock

#endif //BITCOIN_SRC_VBK_POP_SERVICE_HPP
