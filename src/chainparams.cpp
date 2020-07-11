// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2020 The Pexa Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>

#include <chainparamsseeds.h>
#include <consensus/merkle.h>
#include <tinyformat.h>
#include <util/system.h>
#include <util/strencodings.h>
#include <versionbitsinfo.h>
#include <arith_uint256.h>

#include <assert.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << CScriptNum(0) << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "FOX News 6/29/2019 Trump heads to DMZ possibility of meeting Kim crossing into North Korea";
    const CScript genesisOutputScript = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = CBaseChainParams::MAIN;
        consensus.nSubsidyHalvingInterval = 1712290;
        consensus.BIP16Exception = uint256S("0x000000b3f4b347d4a1fb2f2a8f42d5fc33094a49858608e511c0d45f51628b85");
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256S("0x000000b3f4b347d4a1fb2f2a8f42d5fc33094a49858608e511c0d45f51628b85");
        consensus.BIP65Height = 0;
        consensus.BIP66Height = 0;
        consensus.CSVHeight = 0;
        consensus.SegwitHeight = 0;
        consensus.MinBIP9WarningHeight = 0; // segwit activation height + miner confirmation window
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 2016 * 60; // 1.4 days
        consensus.nPowTargetSpacing = 1 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1814; // Approx 90% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0x50; // P
        pchMessageStart[1] = 0x45; // E
        pchMessageStart[2] = 0x58; // X
        pchMessageStart[3] = 0x41; // A
        nDefaultPort = 8235;
        nPruneAfterHeight = 100000;
        m_assumed_blockchain_size = 320;
        m_assumed_chain_state_size = 4;

        genesis = CreateGenesisBlock(1562369695, 33393258, 0x1e00ffff, 4, 50 * COIN );
        consensus.hashGenesisBlock = genesis.GetX16RHash();
        assert(consensus.hashGenesisBlock == uint256S("0x000000b3f4b347d4a1fb2f2a8f42d5fc33094a49858608e511c0d45f51628b85"));
        assert(genesis.hashMerkleRoot == uint256S("0x4e5951cce11bbe8d10e3f9e8b584ee2ed3c80583311e06f40d7e6146f2087f9d"));

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as a oneshot if they don't support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.
        vSeeds.emplace_back("dnsseeds.pexaproject.com", false);

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,75);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,122);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        bech32_hrp = "x";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        m_is_test_chain = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                { 0, uint256S("0x000000b3f4b347d4a1fb2f2a8f42d5fc33094a49858608e511c0d45f51628b85")},
                { 15000, uint256S("0x0000000002cea9e1360328a2f74e6c2276b42b95f382063374124b7c05389668")},
                { 25000, uint256S("0x000000000071ddf335efe23b771f1b18c0431fcf81293225ede1c860c1f9bcbb")},
                { 30000, uint256S("0x0000000001892fd7234514e4a3788aa090fe4f04a33c8407e6c2cf911374574b")},
                { 50000, uint256S("0x00000000003cc95ac63774699923f74d863ee47d373165fe187ec05cf4c60410")},
                { 100000, uint256S("0x00000000000447e6cc530f1e5dd634474e8a17c0691f7d71938919684482e37f")},
                { 110000, uint256S("0x0000000001538cf86005fb3e74e1fce31beca5b91308a8bcad8ed1d443ca6927")},
                { 120000, uint256S("0x0000000000163cc34a9d9be033461a7b2ae25143269fcfb0c302a15e3184ce51")},
                { 135000, uint256S("0x0000000001ac44a61d2bfd0642c7a6c06acd6fa2e4825075231f24d87b8c1f1f")},
                { 162000, uint256S("0x0000000002dfacc55ac43107b3df39f3ade1cd9cf7b51f7dd223afe8cd1e6925")},
                { 200000, uint256S("0x0000000000023dee0c2e55e83a49d59c83f72dd3533eaa031f52138149ac9c12")},
                { 250000, uint256S("0x00000000009c42e9d918ed2824a7348042358b96e68bba6274e6decbb206dd14")},
                { 300000, uint256S("0x0000000000225aae3a6bca08335cec757fb35df7e2a1c9b51c5ccc766011af51")},
                { 339000, uint256S("0x00000000019dbada9e888d005391ee7551724dd16b514cd02a8b05e32d4f7655")},
            }
        };

        chainTxData = ChainTxData{
            // Data from RPC:: getchaintxstats 4096
            // Stats as of 000000000000a72545994ce72b25042ea63707fca169ca4deb7f9dab4f1b1798 window size 43200
            /* nTime    */ 1562369211,
            /* dTxRate  */ 0,
            0
        };

        /** PEXA Start **/

        // DGW Activation.

        nDGWActivationBlock = 12000;
        nAssetsDeactivationBlock = 508000;

        #ifndef WIN32
        snapshotUrl = "https://snapshots.pexaproject.com/mainnet/current";
        versionInfoUrl = "https://snapshots.pexaproject.com/mainnet/versioninfo";
        #else
        snapshotUrl = "http://snapshots.pexaproject.com/mainnet/current";
        versionInfoUrl = "http://snapshots.pexaproject.com/mainnet/versioninfo";
        #endif // WIN 32

        /** PEXA End **/
    }
};

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = CBaseChainParams::TESTNET;
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP16Exception = uint256S("0x000000e5c51eb049ef52013df58667cdcdf9fb6a34bc2fdd70a0ea576c207f2a");
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256S("0x000000e5c51eb049ef52013df58667cdcdf9fb6a34bc2fdd70a0ea576c207f2a");
        consensus.BIP65Height = 0;
        consensus.BIP66Height = 0;
        consensus.CSVHeight = 0;
        consensus.SegwitHeight = 0;
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 2016 * 60; // 1.4 days
        consensus.nPowTargetSpacing = 1 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1310; // Approx 65% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0x50; // P
        pchMessageStart[1] = 0x43; // C
        pchMessageStart[2] = 0x54; // T
        pchMessageStart[3] = 0x4e; // N
        nDefaultPort = 18770;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 40;
        m_assumed_chain_state_size = 2;

        uint32_t nGenesisTime = 1524179366;  // Thursday, September 20, 2018 12:00:00 PM GMT-06:00
        genesis = CreateGenesisBlock(nGenesisTime, 8889826, 0x1e00ffff, 2, 50 * COIN );
        consensus.hashGenesisBlock = genesis.GetX16RV2Hash();
        assert(consensus.hashGenesisBlock == uint256S("0x000000e5c51eb049ef52013df58667cdcdf9fb6a34bc2fdd70a0ea576c207f2a"));
        assert(genesis.hashMerkleRoot == uint256S("4e5951cce11bbe8d10e3f9e8b584ee2ed3c80583311e06f40d7e6146f2087f9d"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("seed-testnet.pexa.dev", false);

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "xt";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        m_is_test_chain = true;
        m_is_mockable_chain = false;

        checkpointData = {
            {
            }
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096
            /* nTime    */ 0,
            /* nTxCount */ 0,
            /* dTxRate  */ 0,
        };

        /** PEXA Start **/

        // DGW Activation
        nDGWActivationBlock = 200;
        nAssetsDeactivationBlock = 0;

        #ifndef WIN32
        snapshotUrl = "https://snapshots.pexaproject.com/testnet/current";
        #else
        snapshotUrl = "http://snapshots.pexaproject.com/testnet/current";
        #endif // WIN 32

        /** PEXA End **/
    }
};

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    explicit CRegTestParams(const ArgsManager& args) {
        strNetworkID =  CBaseChainParams::REGTEST;
        consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP16Exception = uint256();
        consensus.BIP34Height = 500; // BIP34 activated on regtest (Used in functional tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1; // BIP65 activated on regtest (Used in functional tests)
        consensus.BIP66Height = 1; // BIP66 activated on regtest (Used in functional tests)
        consensus.CSVHeight = 1; // CSV activated on regtest (Used in rpc activation tests)
        consensus.SegwitHeight = 0; // SEGWIT is always activated on regtest unless overridden
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 2016 * 60; // 1.4 days
        consensus.nPowTargetSpacing = 1 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0x50; // P
        pchMessageStart[1] = 0x43; // C
        pchMessageStart[2] = 0x52; // R
        pchMessageStart[3] = 0x44; // T
        nDefaultPort = 18444;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        UpdateActivationParametersFromArgs(args);

            genesis = CreateGenesisBlock(1582483185, 2, 0x207fffff, 2, 5000 * COIN);
        consensus.hashGenesisBlock = genesis.GetX16RHash();
        assert(consensus.hashGenesisBlock == uint256S("0x500d7bfd140e3cb24681d334a000d898057250513dc975894009146c61293b3b"));
        assert(genesis.hashMerkleRoot == uint256S("0x9933ca70914e63d4c81a9d8f2d16263f51b22b3aa2c50f763228d160c06ae84f"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fDefaultConsistencyChecks = true;
        fRequireStandard = true;
        m_is_test_chain = true;
        m_is_mockable_chain = true;

        checkpointData = {
            {
            }
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "xcrt";

        // DGW Activation
        nDGWActivationBlock = 200;
    }

    /**
     * Allows modifying the Version Bits regtest parameters.
     */
    void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }
    void UpdateActivationParametersFromArgs(const ArgsManager& args);
};

void CRegTestParams::UpdateActivationParametersFromArgs(const ArgsManager& args)
{
    if (gArgs.IsArgSet("-segwitheight")) {
        int64_t height = gArgs.GetArg("-segwitheight", consensus.SegwitHeight);
        if (height < -1 || height >= std::numeric_limits<int>::max()) {
            throw std::runtime_error(strprintf("Activation height %ld for segwit is out of valid range. Use -1 to disable segwit.", height));
        } else if (height == -1) {
            LogPrintf("Segwit disabled for testing\n");
            height = std::numeric_limits<int>::max();
        }
        consensus.SegwitHeight = static_cast<int>(height);
    }

    if (!args.IsArgSet("-vbparams")) return;

    for (const std::string& strDeployment : args.GetArgs("-vbparams")) {
        std::vector<std::string> vDeploymentParams;
        boost::split(vDeploymentParams, strDeployment, boost::is_any_of(":"));
        if (vDeploymentParams.size() != 3) {
            throw std::runtime_error("Version bits parameters malformed, expecting deployment:start:end");
        }
        int64_t nStartTime, nTimeout;
        if (!ParseInt64(vDeploymentParams[1], &nStartTime)) {
            throw std::runtime_error(strprintf("Invalid nStartTime (%s)", vDeploymentParams[1]));
        }
        if (!ParseInt64(vDeploymentParams[2], &nTimeout)) {
            throw std::runtime_error(strprintf("Invalid nTimeout (%s)", vDeploymentParams[2]));
        }
        bool found = false;
        for (int j=0; j < (int)Consensus::MAX_VERSION_BITS_DEPLOYMENTS; ++j) {
            if (vDeploymentParams[0] == VersionBitsDeploymentInfo[j].name) {
                UpdateVersionBitsParameters(Consensus::DeploymentPos(j), nStartTime, nTimeout);
                found = true;
                LogPrintf("Setting version bits activation parameters for %s to start=%ld, timeout=%ld\n", vDeploymentParams[0], nStartTime, nTimeout);
                break;
            }
        }
        if (!found) {
            throw std::runtime_error(strprintf("Invalid deployment (%s)", vDeploymentParams[0]));
        }
    }
}

static std::unique_ptr<const CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<const CChainParams> CreateChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams(gArgs));
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network, bool fForceBlockNetwork)
{
    SelectBaseParams(network);
    if (fForceBlockNetwork) {
        bNetwork.SetNetwork(network);
    }
    globalChainParams = CreateChainParams(network);
}
