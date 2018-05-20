/*
 * configuration.h
 *
 *  Created on: 2016年9月8日
 *      Author: ranger.shi
 */

#ifndef COIN_CONFIGURATION_H_
#define COIN_CONFIGURATION_H_

#include <memory>
#include "bignum.h"
#include "uint256.h"
#include "arith_uint256.h"
#include "util.h"
#include "chainparams.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <vector>
#include <map>

using namespace std;

class CBlockIndex;
class uint256;

/** Block-chain checkpoints are compiled-in sanity checks.
 * They are updated every release or three.
 */
namespace Checkpoints {

bool CheckBlock(int nHeight, const uint256& hash);// Returns true if block passes checkpoint checks

int GetTotalBlocksEstimate();// Return conservative estimate of total number of blocks, 0 if unknown

CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex);// Returns last CBlockIndex* in mapBlockIndex that is a checkpoint

double GuessVerificationProgress(CBlockIndex *pindex, bool fSigchecks = true);

bool AddCheckpoint(int nHeight, uint256 hash);

bool GetCheckpointByHeight(const int nHeight, std::vector<int> &vCheckpoints);

bool LoadCheckpoint();

void GetCheckpointMap(std::map<int, uint256> &checkpoints);

extern bool g_bEnabled;
}

typedef enum  {
	emNODE_1,
	emNODE_2,
	MAX_NODE//!< EM_MAX_NETWORK_TYPES
}NODE_NUMBER;


class G_CONFIG_TABLE {
 public:
	string GetCoinName() const {
		return /*std::move(COIN_NAME)*/COIN_NAME;
	}
	const vector<string> GetIntPubKey(emNetWork type) const;
	const uint256 GetIntHash(emNetWork type) const;
	const string GetCheckPointPkey(emNetWork type) const;
	const uint256 GetHashMerkleRoot() const;
	vector<unsigned int> GetSeedNodeIP() const;
	unsigned char* GetMagicNumber(emNetWork type) const;
	vector<unsigned char> GetAddressPrefix(emNetWork type, emBase58Type BaseType) const;
	unsigned int GetnDefaultPort(emNetWork type) const;
	unsigned int GetnRPCPort(emNetWork type) const;
	unsigned int GetnUIPort(emNetWork type) const;
	unsigned int GetStartTimeInit(emNetWork type) const;
	unsigned int GetHalvingInterval(emNetWork type) const;
	uint64_t GetCoinInitValue() const;
	uint64_t GetBlockSubsidyCfg(int nHeight) const;

 private:
	//名称：
	static string COIN_NAME;

	//公钥
	static vector<string> intPubKey_mainNet;
	static vector<string> initPubKey_testNet;
	static vector<string> initPubkey_regTest;

	//创世块HASH
	static string hashGenesisBlock_mainNet;
	static string hashGenesisBlock_testNet;
	static string hashGenesisBlock_regTest;

	//check point 公钥
	static string CheckPointPK_MainNet;
	static string CheckPointPK_TestNet;

	//梅根HASH
	static string HashMerkleRoot;

	//IP地址
	static vector<unsigned int> pnSeed;

	//网络协议魔数
	static unsigned char Message_mainNet[MESSAGE_START_SIZE];
	static unsigned char Message_testNet[MESSAGE_START_SIZE];
	static unsigned char Message_regTest[MESSAGE_START_SIZE];

	//修改地址前缀
	static vector<unsigned char> AddrPrefix_mainNet[EM_MAX_BASE58_TYPES];
	static vector<unsigned char> AddrPrefix_testNet[EM_MAX_BASE58_TYPES];

	//网络端口
	static unsigned int nDefaultPort_mainNet;
	static unsigned int nDefaultPort_testNet;
	static unsigned int nDefaultPort_regTest;

	static unsigned int nRPCPort_mainNet;
	static unsigned int nRPCPort_testNet;

	static unsigned int nUIPort_mainNet;
	static unsigned int nUIPort_testNet;

	//修改时间
	static unsigned int StartTime_mainNet;
	static unsigned int StartTime_testNet;
	static unsigned int StartTime_regTest;

	//半衰期
	static unsigned int nSubsidyHalvingInterval_mainNet;
	static unsigned int nSubsidyHalvingInterval_regNet;

	//修改发币总量
	static uint64_t InitialCoin;

	//矿工费用
	static uint64_t DefaultFee;
};

const G_CONFIG_TABLE &IniCfg();



#endif /* COIN_CONFIGURATION_H_ */
