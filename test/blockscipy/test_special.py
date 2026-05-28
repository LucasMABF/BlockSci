"""
Tests a number of less common characteristics of blocks and transactions
(not necessarily nonstandard as in what Bitcoin Core defines as nonstandard scripts).
"""

import blocksci
import pytest

from util import Coin


@pytest.mark.btc
def test_reward_partially_claimed(chain, json_data):
    height = json_data["block-partial-reward-height"]
    coinbase = chain[height].txes[0]
    reward = Coin(50 - 10) if height < 150 else Coin(25 - 10)
    assert reward == sum(output.value for output in coinbase.outputs)


@pytest.mark.btc
def test_fees_not_claimed(chain, json_data):
    height = json_data["block-fee-unclaimed-height"]
    coinbase = chain[height].txes[0]
    reward = Coin(50) if height < 150 else Coin(25)
    assert reward == sum(output.value for output in coinbase.outputs)

    # sanity check that there were actually fees to be claimed
    total_fees_in_block = sum(tx.fee for tx in chain[height].txes)
    assert total_fees_in_block > 0


def test_positive_locktime(chain, json_data):
    tx = chain.tx_with_hash(json_data["change-locktime-tx-1"])
    assert tx.locktime == 110


def test_non_max_nsequence_no(chain, json_data):
    tx = chain.tx_with_hash(json_data["nsequence-fffffffe-tx"])
    inpt = tx.inputs[0]
    assert inpt.sequence_num == 2 ** 32 - 2


def test_op_return(chain, json_data):
    tx = chain.tx_with_hash(json_data["op-return-tx"])
    out = tx.outputs[0]
    op_return = tx.op_return

    assert op_return
    assert out == op_return
    assert op_return.address.data == b"Lord Voldemort"


def test_raw_multisig(chain, json_data):
    tx = chain.tx_with_hash(json_data["raw-multisig-tx"])
    out = tx.outputs[0]

    assert blocksci.address_type.multisig == out.address_type

    addr = out.address
    assert addr.required == 2
    assert addr.total == 3


def test_p2sh_multisig(chain, json_data):
    tx = chain.tx_with_hash(json_data["p2sh-multisig-tx"])
    out = tx.outputs[0]

    assert blocksci.address_type.scripthash == out.address_type

    wrapped = out.address.wrapped_address
    assert blocksci.address_type.multisig == wrapped.type
    assert wrapped.required == 2
    assert wrapped.total == 3
