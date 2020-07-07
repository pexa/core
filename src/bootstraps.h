// Copyright (c) 2019-2020 Xenios SEZC
// https://www.veriblock.org
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef __BOOTSTRAPS_BTC_VBK
#define __BOOTSTRAPS_BTC_VBK

#include <string>
#include <vector>

#include "chainparams.h"
#include <primitives/block.h>
#include <util/system.h> // for gArgs
#include <veriblock/config.hpp>

#include "vbk/util.hpp"

extern int testnetVBKstartHeight;
extern std::vector<std::string> testnetVBKblocks;

extern int testnetBTCstartHeight;
extern std::vector<std::string> testnetBTCblocks;

struct AltChainParamsPEXA : public altintegration::AltChainParams {
    ~AltChainParamsPEXA() override = default;

    AltChainParamsPEXA(const CBlock& genesis)
    {
        auto hash = genesis.GetX16RV2Hash();
        bootstrap.hash = std::vector<uint8_t>{hash.begin(), hash.end()};
        bootstrap.height = 0; // pop is enabled starting at genesis
        bootstrap.timestamp = genesis.GetBlockTime();
    }

    altintegration::AltBlock getBootstrapBlock() const noexcept override
    {
        return bootstrap;
    }

    uint32_t getIdentifier() const noexcept override
    {
        return 0x304fa45;
    }

    std::vector<uint8_t> getHash(const std::vector<uint8_t>& bytes) const noexcept override
    {
        return VeriBlock::headerFromBytes(bytes).GetHash().asVector();
    }

    altintegration::AltBlock bootstrap;
};

void printConfig(const altintegration::Config& config);
void selectPopConfig(const ArgsManager& mgr);
void selectPopConfig(
    const std::string& btcnet,
    const std::string& vbknet,
    bool popautoconfig = true,
    int btcstart = 0,
    const std::string& btcblocks = {},
    int vbkstart = 0,
    const std::string& vbkblocks = {});
#endif
