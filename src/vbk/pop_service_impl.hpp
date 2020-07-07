// Copyright (c) 2019-2020 Xenios SEZC
// https://www.veriblock.org
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SRC_VBK_POP_SERVICE_POP_SERVICE_IMPL_HPP
#define BITCOIN_SRC_VBK_POP_SERVICE_POP_SERVICE_IMPL_HPP

#include "chainparams.h"
#include "pop_service.hpp"

#include <memory>
#include <mutex>
#include <vector>

#include <util/system.h>
#include <veriblock/altintegration.hpp>
#include <veriblock/blockchain/alt_block_tree.hpp>
#include <veriblock/config.hpp>
#include <veriblock/mempool.hpp>
#include <veriblock/storage/payloads_storage.hpp>

namespace VeriBlock {

class PopServiceImpl : public PopService
{
private:
    std::shared_ptr<altintegration::MemPool> mempool;
    std::shared_ptr<altintegration::AltTree> altTree;
    std::shared_ptr<altintegration::PayloadsStorage> payloads_store;

public:
    std::string toPrettyString() const override
    {
        return altTree->toPrettyString();
    };


    altintegration::AltTree& getAltTree() override
    {
        return *altTree;
    }

    altintegration::MemPool& getMemPool() override
    {
        return *mempool;
    }

    PopServiceImpl(const altintegration::Config& config);

    ~PopServiceImpl() override = default;

    PoPRewards getPopRewards(const CBlockIndex& pindexPrev, const Consensus::Params& consensusParams) override;
    void addPopPayoutsIntoCoinbaseTx(CMutableTransaction& coinbaseTx, const CBlockIndex& pindexPrev, const Consensus::Params& consensusParams) override;
    bool checkCoinbaseTxWithPopRewards(const CTransaction& tx, const CAmount& PoWBlockReward, const CBlockIndex& pindexPrev, const Consensus::Params& consensusParams, BlockValidationState& state) override;

    std::vector<BlockBytes> getLastKnownVBKBlocks(size_t blocks) override;
    std::vector<BlockBytes> getLastKnownBTCBlocks(size_t blocks) override;

    bool acceptBlock(const CBlockIndex& indexNew, BlockValidationState& state) override;
    bool addAllBlockPayloads(const CBlockIndex* indexPrev, const CBlock& fullBlock, BlockValidationState& state) override;
    bool setState(const uint256& block, altintegration::ValidationState& state) override;

    altintegration::PopData getPopData(const CBlockIndex& currentBlockIndex) override;
    void removePayloadsFromMempool(const altintegration::PopData& popData) override;

    int compareForks(const CBlockIndex& left, const CBlockIndex& right) override;
};

bool checkPopDataSize(const altintegration::PopData& popData, altintegration::ValidationState& state);

bool popdataStatelessValidation(const altintegration::PopData& popData, altintegration::ValidationState& state);

bool addAllPayloadsToBlockImpl(altintegration::AltTree& tree, const CBlockIndex* indexPrev, const CBlock& block, BlockValidationState& state);
} // namespace VeriBlock
#endif //BITCOIN_SRC_VBK_POP_SERVICE_POP_SERVICE_IMPL_HPP