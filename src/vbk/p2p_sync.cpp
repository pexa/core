// Copyright (c) 2019-2020 Xenios SEZC
// https://www.veriblock.org
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "vbk/p2p_sync.hpp"

#include "veriblock/entities/atv.hpp"
#include "veriblock/entities/vbkblock.hpp"
#include "veriblock/entities/vtb.hpp"

namespace VeriBlock {
namespace p2p {

std::map<NodeId, std::shared_ptr<PopDataNodeState>> mapPopDataNodeState;

template <typename PopDataType>
bool processGetPopData(CNode* node, CConnman* connman, CDataStream& vRecv, altintegration::MemPool& pop_mempool) EXCLUSIVE_LOCKS_REQUIRED(cs_main)
{
    AssertLockHeld(cs_main);
    std::vector<std::vector<uint8_t>> requested_data;
    vRecv >> requested_data;

    if (requested_data.size() > MAX_POP_DATA_SENDING_AMOUNT) {
        Misbehaving(node->GetId(), 20, strprintf("message getdata size() = %u", requested_data.size()));
        return false;
    }

    auto& known_map = getPopDataNodeState(node->GetId()).getMap<PopDataType>();

    const CNetMsgMaker msgMaker(PROTOCOL_VERSION);
    for (const auto& data_hash : requested_data) {
        uint32_t ddosPreventionCounter = known_map[data_hash]++;

        if (ddosPreventionCounter > MAX_POP_MESSAGE_SENDING_COUNT) {
            Misbehaving(node->GetId(), 20, strprintf("peer is spamming pop data %s", PopDataType::name()));
            return false;
        }

        const auto* data = pop_mempool.get<PopDataType>(data_hash);
        if (data != nullptr) {
            connman->PushMessage(node, msgMaker.Make(PopDataType::name(), *data));
        }
    }

    return true;
}

template <typename PopDataType>
bool processOfferPopData(CNode* node, CConnman* connman, CDataStream& vRecv, altintegration::MemPool& pop_mempool) EXCLUSIVE_LOCKS_REQUIRED(cs_main)
{
    AssertLockHeld(cs_main);
    LogPrint(BCLog::NET, "received offered pop data: %s, bytes size: %d\n", PopDataType::name(), vRecv.size());
    std::vector<std::vector<uint8_t>> offered_data;
    vRecv >> offered_data;

    if (offered_data.size() > MAX_POP_DATA_SENDING_AMOUNT) {
        Misbehaving(node->GetId(), 20, strprintf("message getdata size() = %u", offered_data.size()));
        return false;
    }

    auto& known_map = getPopDataNodeState(node->GetId()).getMap<PopDataType>();

    std::vector<std::vector<uint8_t>> requested_data;
    const CNetMsgMaker msgMaker(PROTOCOL_VERSION);
    for (const auto& data_hash : offered_data) {
        uint32_t ddosPreventionCounter = known_map[data_hash]++;

        if (!pop_mempool.get<PopDataType>(data_hash)) {
            requested_data.push_back(data_hash);
        } else if (ddosPreventionCounter > MAX_POP_MESSAGE_SENDING_COUNT) {
            Misbehaving(node->GetId(), 20, strprintf("peer is spamming pop data %s", PopDataType::name()));
            return false;
        }
    }

    if (!requested_data.empty()) {
        connman->PushMessage(node, msgMaker.Make(get_prefix + PopDataType::name(), requested_data));
    }

    return true;
}

template <typename PopDataType>
bool processPopData(CNode* node, CDataStream& vRecv, altintegration::MemPool& pop_mempool, altintegration::AltTree& altTree) EXCLUSIVE_LOCKS_REQUIRED(cs_main)
{
    AssertLockHeld(cs_main);
    LogPrint(BCLog::NET, "received pop data: %s, bytes size: %d\n", PopDataType::name(), vRecv.size());
    PopDataType data;
    vRecv >> data;

    auto& known_map = getPopDataNodeState(node->GetId()).getMap<PopDataType>();
    uint32_t ddosPreventionCounter = known_map[data.getId()]++;

    if (ddosPreventionCounter > MAX_POP_MESSAGE_SENDING_COUNT) {
        Misbehaving(node->GetId(), 20, strprintf("peer is spamming pop dsata %s", PopDataType::name()));
        return false;
    }

    altintegration::ValidationState state;
    if (!pop_mempool.submit(data, altTree, state)) {
        LogPrint(BCLog::NET, "peer %d sent statelessly invalid pop data: %s\n", node->GetId(), state.GetPath());
        Misbehaving(node->GetId(), 20, strprintf("invalid pop data getdata, reason: %s", state.GetPath()));
        return false;
    }

    return true;
}

int processPopData(CNode* pfrom, const std::string& strCommand, CDataStream& vRecv, CConnman* connman)
{
    auto& pop_service = VeriBlock::getService<VeriBlock::PopService>();
    auto& pop_mempool = pop_service.getMemPool();
    auto& alt_tree = pop_service.getAltTree();

    // process Pop Data
    if (strCommand == altintegration::ATV::name()) {
        LOCK(cs_main);
        return processPopData<altintegration::ATV>(pfrom, vRecv, pop_mempool, alt_tree);
    }

    if (strCommand == altintegration::VTB::name()) {
        LOCK(cs_main);
        return processPopData<altintegration::VTB>(pfrom, vRecv, pop_mempool, alt_tree);
    }

    if (strCommand == altintegration::VbkBlock::name()) {
        LOCK(cs_main);
        return processPopData<altintegration::VbkBlock>(pfrom, vRecv, pop_mempool, alt_tree);
    }
    //----------------------

    // offer Pop Data
    if (strCommand == offer_prefix + altintegration::ATV::name()) {
        LOCK(cs_main);
        return processOfferPopData<altintegration::ATV>(pfrom, connman, vRecv, pop_mempool);
    }

    if (strCommand == offer_prefix + altintegration::VTB::name()) {
        LOCK(cs_main);
        return processOfferPopData<altintegration::VTB>(pfrom, connman, vRecv, pop_mempool);
    }

    if (strCommand == offer_prefix + altintegration::VbkBlock::name()) {
        LOCK(cs_main);
        return processOfferPopData<altintegration::VbkBlock>(pfrom, connman, vRecv, pop_mempool);
    }
    //-----------------

    // get Pop Data
    if (strCommand == get_prefix + altintegration::ATV::name()) {
        LOCK(cs_main);
        return processGetPopData<altintegration::ATV>(pfrom, connman, vRecv, pop_mempool);
    }

    if (strCommand == get_prefix + altintegration::VTB::name()) {
        LOCK(cs_main);
        return processGetPopData<altintegration::VTB>(pfrom, connman, vRecv, pop_mempool);
    }

    if (strCommand == get_prefix + altintegration::VbkBlock::name()) {
        LOCK(cs_main);
        return processGetPopData<altintegration::VbkBlock>(pfrom, connman, vRecv, pop_mempool);
    }

    return -1;
}


} // namespace p2p

} // namespace VeriBlock