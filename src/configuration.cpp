/*
 * configuration.h
 *
 *  Created on: 2016年9月8日
 *      Author: ranger.shi
 */

#include "configuration.h"

#include <memory>
#include "bignum.h"
#include "uint256.h"
#include "arith_uint256.h"
#include "util.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <vector>

using namespace std;

#include "main.h"
#include "uint256.h"

#include <stdint.h>
#include "syncdatadb.h"

#include <boost/assign/list_of.hpp> // for 'map_list_of()'

#define MAX_SUBSIDY_HEIGHT (1440 * 365 * 10)

namespace Checkpoints {
typedef map<int, uint256> MapCheckPoints; // the first parameter is  nHeight;
CCriticalSection g_cs_checkPoint;

// How many times we expect transactions after the last checkpoint to
// be slower. This number is a compromise, as it can't be accurate for
// every system. When reindexing from a fast disk with a slow CPU, it
// can be up to 20, while when downloading from a slow network with a
// fast multicore CPU, it won't be much higher than 1.
static const double SIGCHECK_VERIFICATION_FACTOR = 5.0;

struct CCheckpointData {
	MapCheckPoints *mapCheckpoints;
	int64_t nTimeLastCheckpoint;
	int64_t nTransactionsLastCheckpoint;
	double fTransactionsPerDay;
};

bool g_bEnabled = true;

// What makes a good checkpoint block?
// + Is surrounded by blocks with reasonable timestamps
//   (no blocks before with a timestamp after, none after with
//    timestamp before)
// + Contains no strange transactions
static MapCheckPoints mapCheckpoints = boost::assign::map_list_of(0,
		uint256S("d58ffdd8534a6aa9b2fbf5a5bff495f6dc559cecc6a06f7066819e9ca2ed215d"));
static const CCheckpointData data = { &mapCheckpoints, 0,      // * UNIX timestamp of last checkpoint block
		0,      // * total number of transactions between genesis and last checkpoint
				//   (the tx=... number in the SetBestChain debug.log lines)
		0       // * estimated number of transactions per day after checkpoint
		};

static MapCheckPoints mapCheckpointsTestnet = boost::assign::map_list_of(0,
		uint256S("8db73dd7bf5ee3fd9a2c2d584e3dfde4bccf206a1e4d483d3473e98baeb55ceb"));

static const CCheckpointData dataTestnet = { &mapCheckpointsTestnet, 0, 0, 0 };

static MapCheckPoints mapCheckpointsRegtest = boost::assign::map_list_of(0,
		uint256S("e682df48d25894dde8202b9d7f93cc05369ad53e537371e7d45b8a73f7c39faf"));
static const CCheckpointData dataRegtest = { &mapCheckpointsRegtest, 0, 0, 0 };

const CCheckpointData &Checkpoints() {
	if (SysCfg().NetworkID() == EM_TESTNET) {
		return dataTestnet;
	} else if (SysCfg().NetworkID() == EM_MAIN) {
		return data;
	} else {
		return dataRegtest;
	}
}

bool CheckBlock(int nHeight, const uint256& hash) { //nHeight 找不到或 高度和hash都能找到，则返回true
	if (!g_bEnabled) {
		return true;
	}
	const MapCheckPoints& checkpoints = *Checkpoints().mapCheckpoints;

	MapCheckPoints::const_iterator i = checkpoints.find(nHeight);
	if (i == checkpoints.end()) {
		return true;
	}
	return hash == i->second;
}

// Guess how far we are in the verification process at the given block index
double GuessVerificationProgress(CBlockIndex *pindex, bool fSigchecks) {
	if (pindex == NULL) {
		return 0.0;
	}
	int64_t nNow = time(NULL);

	double fSigcheckVerificationFactor = fSigchecks ? SIGCHECK_VERIFICATION_FACTOR : 1.0;
	double fWorkBefore = 0.0; // Amount of work done before pindex
	double fWorkAfter = 0.0;  // Amount of work left after pindex (estimated)
	// Work is defined as: 1.0 per transaction before the last checkpoint, and
	// fSigcheckVerificationFactor per transaction after.

	const CCheckpointData &data = Checkpoints();

	if (pindex->m_unChainTx <= data.nTransactionsLastCheckpoint) {
		double nCheapBefore = pindex->m_unChainTx;
		double nCheapAfter = data.nTransactionsLastCheckpoint - pindex->m_unChainTx;
		double nExpensiveAfter = (nNow - data.nTimeLastCheckpoint) / 86400.0 * data.fTransactionsPerDay;
		fWorkBefore = nCheapBefore;
		fWorkAfter = nCheapAfter + nExpensiveAfter * fSigcheckVerificationFactor;
	} else {
		double nCheapBefore = data.nTransactionsLastCheckpoint;
		double nExpensiveBefore = pindex->m_unChainTx - data.nTransactionsLastCheckpoint;
		double nExpensiveAfter = (nNow - pindex->m_unTime) / 86400.0 * data.fTransactionsPerDay;
		fWorkBefore = nCheapBefore + nExpensiveBefore * fSigcheckVerificationFactor;
		fWorkAfter = nExpensiveAfter * fSigcheckVerificationFactor;
	}

	return fWorkBefore / (fWorkBefore + fWorkAfter);
}

int GetTotalBlocksEstimate() {    // 获取mapCheckpoints 中保存最后一个checkpoint 的高度
	if (!g_bEnabled) {
		return 0;
	}

	const MapCheckPoints& checkpoints = *Checkpoints().mapCheckpoints;

	return checkpoints.rbegin()->first;
}

CBlockIndex* GetLastCheckpoint(const map<uint256, CBlockIndex*>& mapBlockIndex) {
	if (!g_bEnabled) {
		return NULL;
	}

	const MapCheckPoints& checkpoints = *Checkpoints().mapCheckpoints;

	BOOST_REVERSE_FOREACH(const MapCheckPoints::value_type& i, checkpoints){
	const uint256& hash = i.second;
	map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
	if (t != mapBlockIndex.end()) {
		return t->second;
	}
}
	return NULL;
}

bool LoadCheckpoint() {
	LOCK(g_cs_checkPoint);
	SyncData::CSyncDataDb db;
	return db.LoadCheckPoint(*Checkpoints().mapCheckpoints);
}

bool GetCheckpointByHeight(const int nHeight, std::vector<int> &vCheckpoints) {
	LOCK(g_cs_checkPoint);
	MapCheckPoints& checkpoints = *Checkpoints().mapCheckpoints;
	std::map<int, uint256>::iterator iterMap = checkpoints.upper_bound(nHeight);
	while (iterMap != checkpoints.end()) {
		vCheckpoints.push_back(iterMap->first);
		++iterMap;
	}
	return !vCheckpoints.empty();
}

bool AddCheckpoint(int nHeight, uint256 hash) {
	LOCK(g_cs_checkPoint);
	MapCheckPoints& checkpoints = *Checkpoints().mapCheckpoints;
	checkpoints.insert(checkpoints.end(), make_pair(nHeight, hash));
	return true;
}

void GetCheckpointMap(std::map<int, uint256> &mapCheckpoints) {
	LOCK(g_cs_checkPoint);
	const MapCheckPoints& checkpoints = *Checkpoints().mapCheckpoints;
	mapCheckpoints = checkpoints;
}

}

//=========================================================================
//========以下是静态成员初始化的值=====================================================

const G_CONFIG_TABLE &IniCfg() {
	static G_CONFIG_TABLE * psCfg = NULL;
	if (psCfg == NULL) {
		psCfg = new G_CONFIG_TABLE();
	}
	assert(psCfg != NULL);
	return *psCfg;

}
const uint256 G_CONFIG_TABLE::GetIntHash(emNetWork type) const {

	switch (type) {
	case EM_MAIN: {
		return (uint256S((hashGenesisBlock_mainNet)));
	}
	case EM_TESTNET: {
		return (uint256S((hashGenesisBlock_testNet)));
	}
	case EM_REGTEST: {
		return (uint256S((hashGenesisBlock_regTest)));
	}
	default:
		assert(0);
	}
	return uint256S("");

}
const string G_CONFIG_TABLE::GetCheckPointPkey(emNetWork type) const {
	switch (type) {
	case EM_MAIN: {
		return CheckPointPK_MainNet;
	}
	case EM_TESTNET: {
		return CheckPointPK_TestNet;
	}
//			case EM_REGTEST: {
//				return std::move(uint256S(std::move(hashGenesisBlock_regTest)));
//			}
	default:
		assert(0);
	}
	return "";
}

const vector<string> G_CONFIG_TABLE::GetIntPubKey(emNetWork type) const {

	switch (type) {
	case EM_MAIN: {
		return (intPubKey_mainNet);
	}
	case EM_TESTNET: {
		return (initPubKey_testNet);
	}
	case EM_REGTEST: {
		return (initPubkey_regTest);
	}
	default:
		assert(0);
	}
	return vector<string>();
}

const uint256 G_CONFIG_TABLE::GetHashMerkleRoot() const {
	return (uint256S((HashMerkleRoot)));
}

vector<unsigned int> G_CONFIG_TABLE::GetSeedNodeIP() const {
	return pnSeed;
}

unsigned char* G_CONFIG_TABLE::GetMagicNumber(emNetWork type) const {
	switch (type) {
	case EM_MAIN: {
		return Message_mainNet;
	}
	case EM_TESTNET: {
		return Message_testNet;
	}
	case EM_REGTEST: {
		return Message_regTest;
	}
	default:
		assert(0);
	}
	return NULL;
}

vector<unsigned char> G_CONFIG_TABLE::GetAddressPrefix(emNetWork type, emBase58Type BaseType) const {

	switch (type) {
	case EM_MAIN: {
		return AddrPrefix_mainNet[BaseType];
	}
	case EM_TESTNET: {
		return AddrPrefix_testNet[BaseType];
	}
//			case EM_REGTEST: {
//				return Message_regTest;
//			}
	default:
		assert(0);
	}
	return vector<unsigned char>();

}

unsigned int G_CONFIG_TABLE::GetnDefaultPort(emNetWork type) const {
	switch (type) {
	case EM_MAIN: {
		return nDefaultPort_mainNet;
	}
	case EM_TESTNET: {
		return nDefaultPort_testNet;
	}
	case EM_REGTEST: {
		return nDefaultPort_regTest;
	}
	default:
		assert(0);
	}
	return 0;
}
unsigned int G_CONFIG_TABLE::GetnRPCPort(emNetWork type) const {
	switch (type) {
	case EM_MAIN: {
		return nRPCPort_mainNet;
	}
	case EM_TESTNET: {
		return nRPCPort_testNet;
	}
//			case EM_REGTEST: {
//				return Message_regTest;
//			}
	default:
		assert(0);
	}
	return 0;
}
unsigned int G_CONFIG_TABLE::GetnUIPort(emNetWork type) const {
	switch (type) {
	case EM_MAIN: {
		return nUIPort_mainNet;
	}
	case EM_TESTNET: {
		return nUIPort_testNet;
	}
	case EM_REGTEST: {
		return nUIPort_testNet;
	}
	default:
		assert(0);
	}
	return 0;
}
unsigned int G_CONFIG_TABLE::GetStartTimeInit(emNetWork type) const {
	switch (type) {
	case EM_MAIN: {
		return StartTime_mainNet;
	}
	case EM_TESTNET: {
		return StartTime_testNet;
	}
	case EM_REGTEST: {
		return StartTime_regTest;
	}
	default:
		assert(0);
	}
	return 0;
}

unsigned int G_CONFIG_TABLE::GetHalvingInterval(emNetWork type) const {
	switch (type) {
	case EM_MAIN: {
		return nSubsidyHalvingInterval_mainNet;
	}
//				case EM_TESTNET: {
//					return nSubsidyHalvingInterval_testNet;
//				}
	case EM_REGTEST: {
		return nSubsidyHalvingInterval_regNet;
	}
	default:
		assert(0);
	}
	return 0;
}

uint64_t G_CONFIG_TABLE::GetCoinInitValue() const {
	return InitialCoin;
}

uint64_t G_CONFIG_TABLE::GetBlockSubsidyCfg(int nHeight) const {

	if(nHeight <= MAX_SUBSIDY_HEIGHT) {

		return DefaultFee * COIN / 1000000;
	}
	return 0;
}

//=========================================================================
//========以下是静态成员初始化的值=====================================================
//=========================================================================

//名称
string G_CONFIG_TABLE::COIN_NAME = "freetradechain";

//公钥-主网络
vector<string> G_CONFIG_TABLE::intPubKey_mainNet = {
		"02394b76f06c9859999b2fda8e615735fc63104ffbbf46b5d89bde4efed1af3d4c",
		"036d2f093c77fbdd00353249d9e63ef6f6a85e8ed75ba7bf653fca6ae3862c628a",
		"03d33440fa6442f11dc7767e8add0b701f29aa0804f8ec3c484af5f1fda4be8a2b" };
//公钥-测试网络
vector<string> G_CONFIG_TABLE::initPubKey_testNet = {
		"03dacc41ad3e59f367bd887f92265c33d84a40501987a115ce0574354ebd9ab01c",
		"03a19503a38485a327051f435154ebe7666c5a1263c4f599a843ef953c30e27dea",
		"027c4bfb42181ffb96a1e0358af63e180b2d73f14dfc6a9819f5d67ef67d4a29f9" };
//公钥-局域网络
vector<string> G_CONFIG_TABLE::initPubkey_regTest = {
		"039142d8c8b7f754741f46ec158e15acfca6f1b86e36625e932bf7264cdfaf4cb2",
		"0262e18786266ee2b5ae3f7a370382a3af8a4f61c11534cb6e96f35f4d1217bb31",
		"02da788297e71606e3dbdc489da85847444cca54f8753294bae4533af489b67a6f",
		"0319465dccd5e02264f9b76d95027cbc8f2814c48f4be5bb1128d5a482518f839d",
		"034935d0fa6be3eece8db2a50835193336b9e52cda659c588fbdba4f75b5c9cbfa", };

//公钥
string G_CONFIG_TABLE::CheckPointPK_MainNet = "036d2f093c77fbdd00353249d9e63ef6f6a85e8ed75ba7bf653fca6ae3862c628a";
string G_CONFIG_TABLE::CheckPointPK_TestNet = "03a19503a38485a327051f435154ebe7666c5a1263c4f599a843ef953c30e27dea";

//创世块HASH
string G_CONFIG_TABLE::hashGenesisBlock_mainNet = "0xd58ffdd8534a6aa9b2fbf5a5bff495f6dc559cecc6a06f7066819e9ca2ed215d";
string G_CONFIG_TABLE::hashGenesisBlock_testNet = "0x8db73dd7bf5ee3fd9a2c2d584e3dfde4bccf206a1e4d483d3473e98baeb55ceb";
string G_CONFIG_TABLE::hashGenesisBlock_regTest = "0xe682df48d25894dde8202b9d7f93cc05369ad53e537371e7d45b8a73f7c39faf";

//梅根HASH

string G_CONFIG_TABLE::HashMerkleRoot = "0xf0f5f6334e943c1a568d0131c9c1eefe35d98ddb7e62a8d16b462c606ceb566e";

//IP地址
/*
 * 47.93.96.33 0x21605d2f
 * 47.93.43.154 0x9a2b5d2f
 * 118.31.64.31 0x1f401f76
 * 106.15.200.115 0x73c80f6a
 * 112.74.32.158 0x9e204a70
 * 119.23.109.240 0xf06d1777
 */
vector<unsigned int> G_CONFIG_TABLE::pnSeed = { 0x6e6e6927, 0x01636927, 0x32e75e2f, 0x8e396927, 0x409e6e3b, 0xb74a6927, 0x5f6f6927};

//网络协议魔=
unsigned char G_CONFIG_TABLE::Message_mainNet[MESSAGE_START_SIZE] = { 0xaa, 0x44, 0x1b, 0xcd };
unsigned char G_CONFIG_TABLE::Message_testNet[MESSAGE_START_SIZE] = { 0xbb, 0xdc, 0x54, 0xb2 };
unsigned char G_CONFIG_TABLE::Message_regTest[MESSAGE_START_SIZE] = { 0xcc, 0x5b, 0x6b, 0xa6 };

//修改地址前缀
vector<unsigned char> G_CONFIG_TABLE::AddrPrefix_mainNet[EM_MAX_BASE58_TYPES] = { { 36 }, { 55 }, { 128 }, { 0x41, 0x1C,
		0xCB, 0x3F }, { 0x41, 0x1C, 0x3D, 0x4A }, { 0 } };
vector<unsigned char> G_CONFIG_TABLE::AddrPrefix_testNet[EM_MAX_BASE58_TYPES] = { { 95 }, { 99 }, { 210 }, { 0x7F,
		0x57, 0x3F, 0x4D }, { 0x7F, 0x57, 0x5A, 0x2C }, { 0 } };

//网络端口
unsigned int G_CONFIG_TABLE::nDefaultPort_mainNet = 7795;
unsigned int G_CONFIG_TABLE::nDefaultPort_testNet = 17799;
unsigned int G_CONFIG_TABLE::nDefaultPort_regTest = 17791;

unsigned int G_CONFIG_TABLE::nRPCPort_mainNet = 17935;
unsigned int G_CONFIG_TABLE::nRPCPort_testNet = 17980;

unsigned int G_CONFIG_TABLE::nUIPort_mainNet = 5944;
unsigned int G_CONFIG_TABLE::nUIPort_testNet = 5962;
//修改时间
unsigned int G_CONFIG_TABLE::StartTime_mainNet = 1525718667;
unsigned int G_CONFIG_TABLE::StartTime_testNet = 1525709542;
unsigned int G_CONFIG_TABLE::StartTime_regTest = 1525708221;

//半衰期
unsigned int G_CONFIG_TABLE::nSubsidyHalvingInterval_mainNet = 2590000;
unsigned int G_CONFIG_TABLE::nSubsidyHalvingInterval_regNet = 500;

//修改发币初始值
uint64_t G_CONFIG_TABLE::InitialCoin = 1000000000;

//矿工费用
uint64_t G_CONFIG_TABLE::DefaultFee = 0;
