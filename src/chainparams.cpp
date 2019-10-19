// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2019 The Pexa Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"
#include "arith_uint256.h"

#include <assert.h>
#include "chainparamsseeds.h"

//TODO: Take these out
extern double algoHashTotal[16];
extern int algoHashHits[16];


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

void CChainParams::UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    consensus.vDeployments[d].nStartTime = nStartTime;
    consensus.vDeployments[d].nTimeout = nTimeout;
}

void CChainParams::TurnOffSegwit() {
	consensus.nSegwitEnabled = false;
}

void CChainParams::TurnOffCSV() {
	consensus.nCSVEnabled = false;
}

void CChainParams::TurnOffBIP34() {
	consensus.nBIP34Enabled = false;
}

void CChainParams::TurnOffBIP65() {
	consensus.nBIP65Enabled = false;
}

void CChainParams::TurnOffBIP66() {
	consensus.nBIP66Enabled = false;
}

bool CChainParams::BIP34() {
	return consensus.nBIP34Enabled;
}

bool CChainParams::BIP65() {
	return consensus.nBIP34Enabled;
}

bool CChainParams::BIP66() {
	return consensus.nBIP34Enabled;
}

bool CChainParams::CSVEnabled() const{
	return consensus.nCSVEnabled;
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 1712290;  //~ 3.26 yrs at 1 min block time
        consensus.nBIP34Enabled = true;
        consensus.nBIP65Enabled = true; // 000000000000000004c2b624ed5d7756c508d90fd0da2c7c679febfa6c4735f0
        consensus.nBIP66Enabled = true;
        consensus.nSegwitEnabled = true;
        consensus.nCSVEnabled = true;
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
        consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].bit = 6;  //Assets (RIP2)
        consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].nStartTime = 1540944000; // Oct 31, 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].nTimeout = 1572480000; // Oct 31, 2019



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

        genesis = CreateGenesisBlock(1562369695, 33393258, 0x1e00ffff, 4, 50 * COIN );
        consensus.hashGenesisBlock = genesis.GetX16RHash();

        assert(consensus.hashGenesisBlock == uint256S("0x000000b3f4b347d4a1fb2f2a8f42d5fc33094a49858608e511c0d45f51628b85"));
        assert(genesis.hashMerkleRoot == uint256S("0x4e5951cce11bbe8d10e3f9e8b584ee2ed3c80583311e06f40d7e6146f2087f9d"));

        //vSeeds.emplace_back("seed-pexa.bitactivate.com", false);
        //vSeeds.emplace_back("seed-pexa.pexacoin.com", false);
        vSeeds.emplace_back("dnsseeds.pexaproject.com", false);
        vSeeds.emplace_back("colorado.dnsseeds.pexaproject.com", false);

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,75);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,122);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fMiningRequiresPeers = true;

        checkpointData = (CCheckpointData) {
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
                { 158000, uint256S("0x00000000013680071f88508861581cc85d13a47a870108f45d7d5fd1eba06679")}
            }
        };

        chainTxData = ChainTxData{
            // Update as we know more about the contents of the Pexa chain
            // Stats as of 000000000000a72545994ce72b25042ea63707fca169ca4deb7f9dab4f1b1798 window size 43200
            1562369211, // * UNIX timestamp of last known number of transactions
            0,    // * total number of transactions between genesis and that timestamp
                        //   (the tx=... number in the SetBestChain debug.log lines)
            0         // * estimated number of transactions per second after that timestamp
        };

        /** PEXA Start **/
        // Burn Amounts
        nIssueAssetBurnAmount = 5 * COIN;
        nReissueAssetBurnAmount = 1 * COIN;
        nIssueSubAssetBurnAmount = 1 * COIN;
        nIssueUniqueAssetBurnAmount = 1 * COIN;

        // Burn Addresses
        strIssueAssetBurnAddress = "XPexaAssetBurnXXXXXXXXXXXXXXdsLnAz";
        strReissueAssetBurnAddress = "XPexaAssetBurnXXXXXXXXXXXXXXdsLnAz";
        strIssueSubAssetBurnAddress = "XPexaAssetBurnXXXXXXXXXXXXXXdsLnAz";
        strIssueUniqueAssetBurnAddress = "XPexaAssetBurnXXXXXXXXXXXXXXdsLnAz";

        //Global Burn Address
        strGlobalBurnAddress = "XPexaAssetBurnXXXXXXXXXXXXXXR2XeUz";

        // DGW Activation
        nDGWActivationBlock = 12000;
        nX16RV2ActivationTime = 1568678400; // Tue 17 Sep 2019 12:00:00 AM UTC

        nMaxReorganizationDepth = 60; // 60 at 1 minute block timespan is +/- 60 minutes.
        nMinReorganizationPeers = 4;
        nMinReorganizationAge = 60 * 60 * 12; // 12 hours
        /** PEXA End **/
    }
};

/**
 * Testnet (v6)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 2100000;  //~ 4 yrs at 1 min block time
        consensus.nBIP34Enabled = true;
        consensus.nBIP65Enabled = true; // 000000000000000004c2b624ed5d7756c508d90fd0da2c7c679febfa6c4735f0
        consensus.nBIP66Enabled = true;
        consensus.nSegwitEnabled = true;
        consensus.nCSVEnabled = true;

        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 2016 * 60; // 1.4 days
        consensus.nPowTargetSpacing = 1 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1310; // Approx 65% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].bit = 5;
        consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].nStartTime = 1533924000; // GMT: Friday, August 10, 2018 6:00:00 PM
        consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].nTimeout = 1538351999; // GMT: Sunday, September 30, 2018 11:59:59 PM


        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");


        pchMessageStart[0] = 0x50; // P
        pchMessageStart[1] = 0x45; // E
        pchMessageStart[2] = 0x58; // X
        pchMessageStart[3] = 0x41; // A
        nDefaultPort = 18770;
        nPruneAfterHeight = 1000;

        uint32_t nGenesisTime = 1524179366;  // Thursday, September 20, 2018 12:00:00 PM GMT-06:00

        // This is used inorder to mine the genesis block. Once found, we can use the nonce and block hash found to create a valid genesis block
    //    /////////////////////////////////////////////////////////////////


    //    arith_uint256 test;
    //    bool fNegative;
    //    bool fOverflow;
    //    test.SetCompact(0x1e00ffff, &fNegative, &fOverflow);
    //    std::cout << "Test threshold: " << test.GetHex() << "\n\n";

    //    int genesisNonce = 0;
    //    uint256 TempHashHolding = uint256S("0x0000000000000000000000000000000000000000000000000000000000000000");
    //    uint256 BestBlockHash = uint256S("0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    //    for (int i=0;i<40000000;i++) {
    //        genesis = CreateGenesisBlock(nGenesisTime, i, 0x1e00ffff, 2, 50 * COIN );
    //        //genesis.hashPrevBlock = TempHashHolding;
    //        consensus.hashGenesisBlock = genesis.GetHash();

    //        arith_uint256 BestBlockHashArith = UintToArith256(BestBlockHash);
    //        if (UintToArith256(consensus.hashGenesisBlock) < BestBlockHashArith) {
    //            BestBlockHash = consensus.hashGenesisBlock;
    //            std::cout << BestBlockHash.GetHex() << " Nonce: " << i << "\n";
    //            std::cout << "   PrevBlockHash: " << genesis.hashPrevBlock.GetHex() << "\n";
    //        }

    //        TempHashHolding = consensus.hashGenesisBlock;

    //        if (BestBlockHashArith < test) {
    //            genesisNonce = i - 1;
    //            break;
    //        }
    //        //std::cout << consensus.hashGenesisBlock.GetHex() << "\n";
    //    }
    //    std::cout << "\n";
    //    std::cout << "\n";
    //    std::cout << "\n";

    //    std::cout << "hashGenesisBlock to 0x" << BestBlockHash.GetHex() << std::endl;
    //    std::cout << "Genesis Nonce to " << genesisNonce << std::endl;
    //    std::cout << "Genesis Merkle " << genesis.hashMerkleRoot.GetHex() << std::endl;

    //    std::cout << "\n";
    //    std::cout << "\n";
    //    int totalHits = 0;
    //    double totalTime = 0.0;

    //    for(int x = 0; x < 16; x++) {
    //        totalHits += algoHashHits[x];
    //        totalTime += algoHashTotal[x];
    //        std::cout << "hash algo " << x << " hits " << algoHashHits[x] << " total " << algoHashTotal[x] << " avg " << algoHashTotal[x]/algoHashHits[x] << std::endl;
    //    }

    //    std::cout << "Totals: hash algo " <<  " hits " << totalHits << " total " << totalTime << " avg " << totalTime/totalHits << std::endl;

    //    genesis.hashPrevBlock = TempHashHolding;

    //    return;

    //    /////////////////////////////////////////////////////////////////

        genesis = CreateGenesisBlock(nGenesisTime, 8889826, 0x1e00ffff, 2, 50 * COIN );
        consensus.hashGenesisBlock = genesis.GetX16RHash();

        //Test MerkleRoot and GenesisBlock
        assert(consensus.hashGenesisBlock == uint256S("0x000000e5c51eb049ef52013df58667cdcdf9fb6a34bc2fdd70a0ea576c207f2a"));
        assert(genesis.hashMerkleRoot == uint256S("4e5951cce11bbe8d10e3f9e8b584ee2ed3c80583311e06f40d7e6146f2087f9d"));

        vFixedSeeds.clear();
        vSeeds.clear();

        vSeeds.emplace_back("seed-testnet.pexa.dev", false);

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,75);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fMiningRequiresPeers = true;

        checkpointData = (CCheckpointData) {
            {
                { 230, uint256S("0x000004cce4dd292058ca040ecb6c2d51a970686d10631030216d6849c7a2227d")},
            }
        };

        chainTxData = ChainTxData{
            // Update as we know more about the contents of the Pexa chain
            // Stats as of 00000023b66f46d74890287a7b1157dd780c7c5fdda2b561eb96684d2b39d62e window size 43200
            0, // * UNIX timestamp of last known number of transactions
            0,     // * total number of transactions between genesis and that timestamp
                        //   (the tx=... number in the SetBestChain debug.log lines)
            0        // * estimated number of transactions per second after that timestamp
        };

        /** PEXA Start **/
        // Burn Amounts
        nIssueAssetBurnAmount = 1 * COIN;
        nReissueAssetBurnAmount = 1 * COIN;
        nIssueSubAssetBurnAmount = 1 * COIN;
        nIssueUniqueAssetBurnAmount = 1 * COIN;

        // Burn Addresses
        strIssueAssetBurnAddress = "XPexaAssetBurnXXXXXXXXXXXXXXdsLnAz";
        strReissueAssetBurnAddress = "XPexaAssetBurnXXXXXXXXXXXXXXdsLnAz";
        strIssueSubAssetBurnAddress = "XPexaAssetBurnXXXXXXXXXXXXXXdsLnAz";
        strIssueUniqueAssetBurnAddress = "XPexaAssetBurnXXXXXXXXXXXXXXdsLnAz";

        // Global Burn Address
        strGlobalBurnAddress = "XPexaAssetBurnXXXXXXXXXXXXXXdsLnAz";

        // DGW Activation
        nDGWActivationBlock = 200;
        nX16RV2ActivationTime = 1568158500;
        
        nMaxReorganizationDepth = 60; // 60 at 1 minute block timespan is +/- 60 minutes.
        nMinReorganizationPeers = 4;
        nMinReorganizationAge = 60 * 60 * 12; // 12 hours
        /** PEXA End **/

    }
};

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nBIP34Enabled = true;
        consensus.nBIP65Enabled = true; // 000000000000000004c2b624ed5d7756c508d90fd0da2c7c679febfa6c4735f0
        consensus.nBIP66Enabled = true;
        consensus.nSegwitEnabled = true;
        consensus.nCSVEnabled = true;
        consensus.nSubsidyHalvingInterval = 150;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 2016 * 60; // 1.4 days
        consensus.nPowTargetSpacing = 1 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].bit = 6;
        consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].nTimeout = 999999999999ULL;



        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0x50; // P
        pchMessageStart[1] = 0x45; // E
        pchMessageStart[2] = 0x58; // X
        pchMessageStart[3] = 0x41; // A
        nDefaultPort = 18444;
        nPruneAfterHeight = 1000;

        // This is used inorder to mine the genesis block. Once found, we can use the nonce and block hash found to create a valid genesis block
       /////////////////////////////////////////////////////////////////


       arith_uint256 test;
       bool fNegative;
       bool fOverflow;
       test.SetCompact(0x207fffff, &fNegative, &fOverflow);
       std::cout << "Test threshold: " << test.GetHex() << "\n\n";

       int genesisNonce = 0;
       uint256 TempHashHolding = uint256S("0x0000000000000000000000000000000000000000000000000000000000000000");
       uint256 BestBlockHash = uint256S("0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
       for (int i=0;i<40000000;i++) {
           genesis = CreateGenesisBlock(1533751200, i, 0x207fffff, 2, 50 * COIN );
           //genesis.hashPrevBlock = TempHashHolding;
           consensus.hashGenesisBlock = genesis.GetHash();

           arith_uint256 BestBlockHashArith = UintToArith256(BestBlockHash);
           if (UintToArith256(consensus.hashGenesisBlock) < BestBlockHashArith) {
               BestBlockHash = consensus.hashGenesisBlock;
               std::cout << BestBlockHash.GetHex() << " Nonce: " << i << "\n";
               std::cout << "   PrevBlockHash: " << genesis.hashPrevBlock.GetHex() << "\n";
           }

           TempHashHolding = consensus.hashGenesisBlock;

           if (BestBlockHashArith < test) {
               genesisNonce = i - 1;
               break;
           }
           //std::cout << consensus.hashGenesisBlock.GetHex() << "\n";
       }
       std::cout << "\n";
       std::cout << "\n";
       std::cout << "\n";

       std::cout << "hashGenesisBlock to 0x" << BestBlockHash.GetHex() << std::endl;
       std::cout << "Genesis Nonce to " << genesisNonce << std::endl;
       std::cout << "Genesis Merkle " << genesis.hashMerkleRoot.GetHex() << std::endl;

       std::cout << "\n";
       std::cout << "\n";
       int totalHits = 0;
       double totalTime = 0.0;

       for(int x = 0; x < 16; x++) {
           totalHits += algoHashHits[x];
           totalTime += algoHashTotal[x];
           std::cout << "hash algo " << x << " hits " << algoHashHits[x] << " total " << algoHashTotal[x] << " avg " << algoHashTotal[x]/algoHashHits[x] << std::endl;
       }

       std::cout << "Totals: hash algo " <<  " hits " << totalHits << " total " << totalTime << " avg " << totalTime/totalHits << std::endl;

       genesis.hashPrevBlock = TempHashHolding;

       return;

       /////////////////////////////////////////////////////////////////


        genesis = CreateGenesisBlock(1537466400, 309205, 0x207fffff, 4, 50 * COIN );
        consensus.hashGenesisBlock = genesis.GetX16RHash();

        assert(consensus.hashGenesisBlock == uint256S("0x00000cb35c53860fb011d1750779e1b0701f5c393fb972c6325c9e8cf729bdad"));
        assert(genesis.hashMerkleRoot == uint256S("0x6fb6a914668b3086790ac435dff5d70be187f47407e7787f991690cb26469ff7"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;

        checkpointData = (CCheckpointData) {
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


        /** PEXA Start **/
        // Burn Amounts
        nIssueAssetBurnAmount = 1 * COIN;
        nReissueAssetBurnAmount = 1 * COIN;
        nIssueSubAssetBurnAmount = 1 * COIN;
        nIssueUniqueAssetBurnAmount = 1 * COIN;

        // Burn Addresses
        strIssueAssetBurnAddress = "XPexaAssetBurnXXXXXXXXXXXXXXdsLnAz";
        strReissueAssetBurnAddress = "XPexaAssetBurnXXXXXXXXXXXXXXdsLnAz";
        strIssueSubAssetBurnAddress = "XPexaAssetBurnXXXXXXXXXXXXXXdsLnAz";
        strIssueUniqueAssetBurnAddress = "XPexaAssetBurnXXXXXXXXXXXXXXdsLnAz";

        // Global Burn Address
        strGlobalBurnAddress = "XPexaAssetBurnXXXXXXXXXXXXXXdsLnAz";

        // DGW Activation
        nDGWActivationBlock = 200;
        nX16RV2ActivationTime = 1566571889;

        nMaxReorganizationDepth = 60; // 60 at 1 minute block timespan is +/- 60 minutes.
        nMinReorganizationPeers = 4;
        nMinReorganizationAge = 60 * 60 * 12; // 12 hours
        /** PEXA End **/
    }
};

static std::unique_ptr<CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<CChainParams> CreateChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams());
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

void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    globalChainParams->UpdateVersionBitsParameters(d, nStartTime, nTimeout);
}

void TurnOffSegwit(){
	globalChainParams->TurnOffSegwit();
}

void TurnOffCSV() {
	globalChainParams->TurnOffCSV();
}

void TurnOffBIP34() {
	globalChainParams->TurnOffBIP34();
}

void TurnOffBIP65() {
	globalChainParams->TurnOffBIP65();
}

void TurnOffBIP66() {
	globalChainParams->TurnOffBIP66();
}
