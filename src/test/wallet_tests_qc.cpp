#include "wallet.h"

#include <set>
#include <stdint.h>
#include <utility>
#include <vector>
#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "cppqc.h"

namespace std {
    std::ostream &operator<<(std::ostream &out, const std::vector<int> &v)
    {
        out << "[";
        for (std::vector<int>::const_iterator it = v.begin(); it != v.end();
                ++it) {
            if (it != v.begin())
                out << ", ";
            out << *it;
        }
        out << "]";
        return out;
    }
}

typedef set<pair<const CWalletTx*,unsigned int> > CoinSet;

BOOST_AUTO_TEST_SUITE(wallet_tests_qc)

static CWallet wallet;
static vector<COutput> vCoins;

static void add_coin(int64_t nValue, int nAge = 6*24, bool fIsFromMe = false, int nInput=0)
{
    static int nextLockTime = 0;
    CTransaction tx;
    tx.nLockTime = nextLockTime++;        // so all transactions get different hashes
    tx.vout.resize(nInput+1);
    tx.vout[nInput].nValue = nValue;
    CWalletTx* wtx = new CWalletTx(&wallet, tx);
    if (fIsFromMe)
    {
        // IsFromMe() returns (GetDebit() > 0), and GetDebit() is 0 if vin.empty(),
        // so stop vin being empty, and cache a non-zero Debit to fake out IsFromMe()
        wtx->vin.resize(1);
        wtx->fDebitCached = true;
        wtx->nDebitCached = 1;
    }
    COutput output(wtx, nInput, nAge);
    vCoins.push_back(output);
}

static void empty_wallet(void)
{
    BOOST_FOREACH(COutput output, vCoins)
        delete output.tx;
    vCoins.clear();
}

static bool equal_sets(CoinSet a, CoinSet b)
{
    pair<CoinSet::iterator, CoinSet::iterator> ret = mismatch(a.begin(), a.end(), b.begin());
    return ret.first == a.end() && ret.second == b.end();
}

struct amount_and_age
{
    int64_t nAmount;
    int     nAge;
};

struct PropCoinMaturity : cppqc::Property<std::vector<int> >
{
    PropCoinMaturity() : Property(cppqc::listOf<int>()) {}
    bool check(const std::vector<int> &a) const
    {
        return true;
    }
    std::string name() const
    {
        return "Test wallet.SelectCoinsMinConf with new and mature coins";
    }
    std::string classify(const std::vector<int> &a) const
    {
        return "todo_classify";
    }
    bool trivial(const std::vector<int> &a) const
    {
        // TODO
        return false;
    }
};

BOOST_AUTO_TEST_CASE(coin_selection_tests)
{
    cppqc::quickCheckOutput(PropCoinMaturity());
}

BOOST_AUTO_TEST_SUITE_END()
