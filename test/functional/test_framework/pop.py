#!/usr/bin/env python3
# Copyright (c) 2019-2020 Xenios SEZC
# https://www.veriblock.org
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test framework addition to include VeriBlock PoP functions"""
import struct
import time
from typing import List

from .messages import ser_uint256, hash256, uint256_from_str
from .script import hash160, CScript, OP_DUP, OP_HASH160, OP_EQUALVERIFY, OP_CHECKSIG
from .test_node import TestNode
from .util import hex_str_to_bytes

KEYSTONE_INTERVAL = 5
POP_PAYOUT_DELAY = 50


def isKeystone(height):
    return height % KEYSTONE_INTERVAL == 0


def getPreviousKeystoneHeight(height):
    diff = height % KEYSTONE_INTERVAL
    if diff == 0:
        diff = KEYSTONE_INTERVAL
    prevks = height - diff
    return max(0, prevks)


def getKeystones(node, height):
    zero = '00' * 32
    if height == 0:
        return [zero, zero]

    prev1 = getPreviousKeystoneHeight(height)
    a = node.getblockhash(prev1)

    assert prev1 >= 0
    if prev1 == 0:
        return [a, zero]

    prev2 = getPreviousKeystoneHeight(prev1)
    b = node.getblockhash(prev2)

    return [a, b]


# size = size of chain to be created
def create_endorsed_chain(node, apm, size: int, addr: str) -> None:
    hash = node.getbestblockhash()
    block = node.getblock(hash)
    height = block['height']

    for i in range(size):
        txid = endorse_block(node, apm, height, addr)
        containinghash = node.generate(nblocks=1)[0]
        # endorsing prev tip
        node.waitforblockheight(height + 1)
        containing = node.getblock(containinghash)
        assert txid in containing['tx'], \
            "iteration {}: containing block at height {}" \
            "does not contain pop tx {}".format(i, containing['height'], txid)

        # we advanced 1 block further
        height += 1


def endorse_block(node, apm, height: int, addr: str) -> str:
    from pypopminer import PublicationData

    # get pubkey for that address
    pubkey = node.getaddressinfo(addr)['pubkey']
    pkh = hash160(hex_str_to_bytes(pubkey))
    script = CScript([OP_DUP, OP_HASH160, pkh, OP_EQUALVERIFY, OP_CHECKSIG])
    payoutInfo = script.hex()

    popdata = node.getpopdata(height)
    last_vbk = popdata['last_known_veriblock_blocks'][0]
    header = popdata['block_header']
    pub = PublicationData()
    pub.header = header
    pub.payoutInfo = payoutInfo
    pub.identifier = 0x304fa45
    payloads = apm.endorseAltBlock(pub, last_vbk)
    vtbs = [x.toHex() for x in payloads.vtbs]
    node.submitpop([], vtbs, [payloads.atv.toHex()])
    return payloads.atv.getId()


class ContextInfoContainer:
    __slots__ = ("height", "keystone1", "keystone2", "txRoot")

    @staticmethod
    def create(node, prev=None, prevheight=None):
        assert isinstance(node, TestNode)
        if prevheight:
            return ContextInfoContainer.createFromHeight(node, prevheight + 1)

        if isinstance(prev, int):
            prev = ser_uint256(prev)[::-1].hex()

        assert (isinstance(prev, str))
        best = node.getblock(prev)
        return ContextInfoContainer.createFromHeight(node, best['height'] + 1)

    @staticmethod
    def createFromHeight(node, height):
        assert isinstance(node, TestNode)
        assert (isinstance(height, int))
        p1, p2 = getKeystones(node, height)
        return ContextInfoContainer(height, p1, p2)

    def __init__(self, height=None, keystone1=None, keystone2=None):
        self.height: int = height
        self.keystone1: str = keystone1
        self.keystone2: str = keystone2
        self.txRoot: int = 0

    def getUnauthenticated(self):
        data = b''
        data += struct.pack(">I", self.height)
        data += bytes.fromhex(self.keystone1)[::-1]
        data += bytes.fromhex(self.keystone2)[::-1]
        return data

    def getUnauthenticatedHash(self):
        # a double sha of unauthenticated context
        return hash256(self.getUnauthenticated())

    def getTopLevelMerkleRoot(self):
        data = b''
        data += ser_uint256(self.txRoot)
        data += self.getUnauthenticatedHash()
        return uint256_from_str(hash256(data))

    def setTxRootInt(self, txRoot: int):
        assert isinstance(txRoot, int)
        self.txRoot = txRoot

    def setTxRootHex(self, txRoot: str):
        assert isinstance(txRoot, str)
        assert len(txRoot) == 64
        self.txRoot = int(txRoot, 16)

    def __repr__(self):
        return "ContextInfo(height={}, ks1={}, ks2={}, mroot={})".format(self.height, self.keystone1,
                                                                         self.keystone2,
                                                                         self.txRoot)


def sync_pop_mempools(rpc_connections, *, wait=1, timeout=60, flush_scheduler=True):
    """
    Wait until everybody has the same POP data in their POP mempools
    """

    def test(s):
        return s.count(s[0]) == len(rpc_connections)

    stop_time = time.time() + timeout
    while time.time() <= stop_time:
        mpooldata = [r.getrawpopmempool() for r in rpc_connections]
        atvs = [set(data['atvs']) for data in mpooldata]
        vtbs = [set(data['vtbs']) for data in mpooldata]
        vbkblocks = [set(data['vbkblocks']) for data in mpooldata]

        if test(atvs) and test(vtbs) and test(vbkblocks):
            if flush_scheduler:
                for r in rpc_connections:
                    r.syncwithvalidationinterfacequeue()
            return
        time.sleep(wait)
    raise AssertionError("POP mempool sync timed out: \natvs: {}\nvtbs: {}\nvbkblocks:{}".format(
        "".join("\n  {!r}".format(m) for m in atvs),
        "".join("\n  {!r}".format(m) for m in vtbs),
        "".join("\n  {!r}".format(m) for m in vbkblocks)
    ))
