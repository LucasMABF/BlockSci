import blocksci
import pytest


def test_power_of_ten_change(chain, json_data):
    for i in range(6):
        txid = json_data[f"change-ten-{i}-tx"]
        tx = chain.tx_with_hash(txid)
        result = blocksci.heuristics.change.power_of_ten_value(8 - i, tx).to_list()
        assert len(
            result
        ) == 1, f"Incorrect number of outputs identified with {i} digits"

        idx = json_data[f"change-ten-{i}-position"]
        assert idx == list(result)[0].index, "Incorrect index of change output"


def test_peeling_chain_change(chain, json_data):
    for i in range(3, 8):
        txid = json_data[f"peeling-chain-{i}-tx"]
        tx = chain.tx_with_hash(txid)
        heuristic = (
            blocksci.heuristics.change.peeling_chain
            and blocksci.heuristics.change.spent
        )
        result = heuristic(tx).to_list()
        assert len(result) == 1

        idx = json_data[f"peeling-chain-{i}-position"]
        assert idx == list(result)[0].index


def test_optimal_change(chain, json_data):
    for i in range(2):
        txid = json_data[f"change-optimal-{i}-tx"]
        tx = chain.tx_with_hash(txid)

        result = blocksci.heuristics.change.optimal_change(tx).to_list()
        print(result)
        assert len(result) == 1

        idx = json_data[f"change-optimal-{i}-position"]
        assert idx == list(result)[0].index


def test_address_type_change(chain, json_data):
    for i in range(3):
        txid = json_data[f"change-address-type-tx-{i}"]
        tx = chain.tx_with_hash(txid)

        result = blocksci.heuristics.change.address_type(tx).to_list()
        assert len(result) == 1

        idx = json_data[f"change-address-type-position-{i}"]
        assert idx == list(result)[0].index


def test_locktime_change(chain, json_data):
    for i in range(3):
        txid = json_data[f"change-locktime-tx-{i}"]
        tx = chain.tx_with_hash(txid)

        result = blocksci.heuristics.change.locktime(tx).to_list()
        assert len(result) == 1

        idx = json_data[f"change-locktime-position-{i}"]
        assert idx == list(result)[0].index


def test_address_reuse_change(chain, json_data):
    for i in range(3):
        txid = json_data[f"change-reuse-tx-{i}"]
        tx = chain.tx_with_hash(txid)

        result = blocksci.heuristics.change.address_reuse(tx).to_list()
        assert len(result) == 1

        idx = json_data[f"change-reuse-position-{i}"]
        assert idx == list(result)[0].index


def test_client_behavior_change(chain, json_data):
    for i in range(3):
        txid = json_data[f"change-client-behavior-tx-{i}"]
        tx = chain.tx_with_hash(txid)

        result = blocksci.heuristics.change.client_change_address_behavior(tx).to_list()
        assert len(result) == 1

        idx = json_data[f"change-client-behavior-position-{i}"]
        assert idx == list(result)[0].index


def test_no_change(chain, json_data):
    test_txs = [
        "change-reuse-tx-{}",
        "change-locktime-tx-{}",
        "change-address-type-tx-{}",
    ]
    for test in test_txs:
        for i in range(3):
            txid = json_data[test.format(i)]
            tx = chain.tx_with_hash(txid)

            result = blocksci.heuristics.change.none(tx).to_list()
            assert len(result) == 0


def test_unique_change(chain, json_data):
    for i in range(3):
        txid = json_data[f"change-client-behavior-tx-{i}"]
        tx = chain.tx_with_hash(txid)

        result = blocksci.heuristics.change.client_change_address_behavior.unique_change(
            tx
        ).to_list()
        assert len(result) == 1


def test_negative_testcase(chain, json_data):
    tx = chain.tx_with_hash(json_data["change-negative-testcase-tx"])
    empty_heuristics = [
        blocksci.heuristics.change.peeling_chain,
        blocksci.heuristics.change.optimal_change,
        blocksci.heuristics.change.address_reuse,
        blocksci.heuristics.change.legacy,
        blocksci.heuristics.change.none,
    ]
    for h in empty_heuristics:
        result = h(tx).to_list()
        assert len(result) == 0

    full_heuristics = [
        blocksci.heuristics.change.address_type,
        blocksci.heuristics.change.locktime,
        blocksci.heuristics.change.client_change_address_behavior,
    ]
    for h in full_heuristics:
        result = h(tx).to_list()
        assert len(result) == 2


def test_change_regression(chain, json_data, regtest):
    heuristics = [
        blocksci.heuristics.change.address_type,
        blocksci.heuristics.change.locktime,
        blocksci.heuristics.change.client_change_address_behavior,
        blocksci.heuristics.change.peeling_chain,
        blocksci.heuristics.change.optimal_change,
        blocksci.heuristics.change.address_reuse,
        blocksci.heuristics.change.legacy,
        blocksci.heuristics.change.none,
    ]
    identifiers = [
        "change-negative-testcase-tx",
        "change-reuse-tx-1",
        "change-locktime-tx-1",
        "change-optimal-0-tx",
        "peeling-chain-5-tx",
        "change-ten-2-tx",
        "fan-8-tx",
        "peeling-chain-4-tx",
        "tx-chain-10-tx-1",
        "funding-tx-2-in-2-out",
    ]
    txs = [chain.tx_with_hash(json_data[identifier]) for identifier in identifiers]
    for h in heuristics:
        for tx in txs:
            print(h(tx).index.tolist(), file=regtest)
            print(h.unique_change(tx).index.tolist(), file=regtest)


heuristics = [
    blocksci.heuristics.change.address_reuse,
    blocksci.heuristics.change.address_type,
    blocksci.heuristics.change.optimal_change,
    blocksci.heuristics.change.power_of_ten_value(5),
    blocksci.heuristics.change.power_of_ten_value(7),
    blocksci.heuristics.change.none,
]


@pytest.mark.btc
def test_union(chain):
    for h1 in heuristics:
        for h2 in heuristics:
            hunion = h1 | h2

            for tx in chain.blocks[100:].txes:
                r1 = set(h1(tx).to_list())
                r2 = set(h2(tx).to_list())

                union = set(hunion(tx).to_list())

                assert union == r1.union(r2)


@pytest.mark.btc
def test_intersection(chain):
    for h1 in heuristics:
        for h2 in heuristics:

            hintersect = h1 & h2

            for tx in chain.blocks[100:].txes:
                r1 = set(h1(tx).to_list())
                r2 = set(h2(tx).to_list())

                rintersect = set(hintersect(tx).to_list())
                assert rintersect == r1.intersection(r2)


@pytest.mark.btc
def test_diff(chain):
    for h1 in heuristics:
        for h2 in heuristics:

            hdiff1 = h1 - h2
            hdiff2 = h2 - h1

            for tx in chain.blocks[100:].txes:
                r1 = set(h1(tx).to_list())
                r2 = set(h2(tx).to_list())

                diff1 = set(hdiff1(tx).to_list())
                diff2 = set(hdiff2(tx).to_list())

                assert diff1 == r1.difference(r2)
                assert diff2 == r2.difference(r1)
