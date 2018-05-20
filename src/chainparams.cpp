// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2016 The Coin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include "configuration.h"
#include "chainparams.h"

#include "assert.h"
#include "core.h"
#include "protocol.h"
#include "util.h"
#include "key.h"
#include "tx.h"
#include "main.h"

#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string/case_conv.hpp> // for to_lower()
#include <boost/algorithm/string/predicate.hpp> // for startswith() and endswith()
#include <boost/filesystem.hpp>
using namespace boost::assign;
using namespace std;

map<string, string> CBaseParams::m_mapArgs;
map<string, vector<string> > CBaseParams::m_mapMultiArgs;


#if 0
vector<string> intPubKey_mainNet = {
		"026991a31e799f98fc9ca74d00b913040326f1abb99fb55663bce8ad407f299dfa",
		"034ab3bdbcfdd45333ea0fb76dfb3c3c3552ee7ba5da1afe8e5530a40dacbf1a94"
};

vector<string> initPubKey_testNet = { //
		"03ac2ffc1bbae80d1946cba01f13a15e2248c64cc405b43273b9ce737044487e07",
		"03009ee4c9219bd0df8c63c787b044d8fb299caea44bcb29d4b5ee661b3269780f"
};

vector<string> initPubkey_regTest = {
		"032e82a58aba68939ddbd9fe07227d0b0371b406a8009cf1c379dd0ecb271b1de9",
		"03c14a5d733022c598ace7f8db2864a9b973ab30122285ff68e42981e4b8df70f2",
		"02e7e6cb6fe732093caf17ba8e81c5ef576f6d3decb0e7e47f831d7c1a2d88f883",
		"0270066dad57255cd8c3ed80f48d92baaaa70e782b4a16e8f586f4182b4d7b8892",
		"02205b34eeb4dec86102ff1cda6b898638afb5883d173efb2f5b283faae6fdfab0",
};



/*
120.24.244.89 0x59f41878
120.24.244.228 0xe4f41878
120.27.115.103 0x67731b78
139.196.155.204 0xcc9bc48b
139.224.33.4 0x0421e08b
120.27.4.2 0x02041b78
 */

/*
 114.55.115.47 0x2f733772
 114.55.110.185 0xb96e3772

 120.26.96.130 0x82601a78
 121.41.112.111 0x6f702979
 */

unsigned int pnSeed[] = //
		{0x82601a78, 0x6f702979};
#endif

class CMainParams: public CBaseParams {
public:
	CMainParams() {


// The message start string is designed to be unlikely to occur in normal data.
// The characters are rarely used upper ASCII, not valid as UTF-8, and produce
// a large 4-byte int at any alignment.
		memcpy(m_pchMessageStart,IniCfg().GetMagicNumber(EM_MAIN),sizeof(m_pchMessageStart));
//		pchMessageStart[0] = G_CONFIG_TABLE::Message_mainNet[0];
//		pchMessageStart[1] = G_CONFIG_TABLE::Message_mainNet[1];
//		pchMessageStart[2] = G_CONFIG_TABLE::Message_mainNet[2];
//		pchMessageStart[3] = G_CONFIG_TABLE::Message_mainNet[3];
		m_vchAlertPubKey =	ParseHex(IniCfg().GetCheckPointPkey(EM_MAIN));
		m_nDefaultPort = IniCfg().GetnDefaultPort(EM_MAIN) ;
		m_nRPCPort = IniCfg().GetnRPCPort(EM_MAIN);
		m_nUIPort = IniCfg().GetnUIPort(EM_MAIN);
		m_strDataDir = "main";
		m_bnProofOfStakeLimit =~arith_uint256(0) >> 10;        //00 3f ff ff
		m_nSubsidyHalvingInterval = IniCfg().GetHalvingInterval(EM_MAIN);
		assert(CreateGenesisRewardTx(genesis.vptx, IniCfg().GetIntPubKey(EM_MAIN)));
		genesis.SetHashPrevBlock(uint256());
		genesis.SetHashMerkleRoot(genesis.BuildMerkleTree());
		genesis.SetHashPos(uint256());
		genesis.SetVersion(1);
		genesis.SetTime(IniCfg().GetStartTimeInit(EM_MAIN));
		genesis.SetBits(0x1f3fffff);        //00 3f ff
		genesis.SetNonce(108);
		genesis.SetFuelRate(INIT_FUEL_RATES);
		genesis.SetHeight(0);
		genesis.ClearSignature();


		m_cHashGenesisBlock = genesis.GetHash();
		m_strPublicKey=IniCfg().GetCheckPointPkey(EM_MAIN);
//		publicKey = "0228823344c30cdd49a12d12283a9e17b4d428635eb969535dbc3b81a46c5b95fd";
//		{
//			cout << "main hashGenesisBlock:\r\n" << hashGenesisBlock.ToString() << endl;
//			cout << "main hashMerkleRoot:\r\n" << genesis.GetHashMerkleRoot().ToString() << endl;
//		}



//		genesis.printInfo();
		//cout << "main m_cHashGenesisBlock:\r\n" << m_cHashGenesisBlock.ToString() << endl;
		//cout << "main hashMerkleRoot:\r\n" << genesis.GetHashMerkleRoot().ToString() << endl;
		assert(m_cHashGenesisBlock == IniCfg().GetIntHash(EM_MAIN));
		assert(genesis.GetHashMerkleRoot() == IniCfg().GetHashMerkleRoot());
//      vSeeds.push_back(CDNSSeedData("soypay.org.cn", "seed_cn_0.dspay.org"));
//      vSeeds.push_back(CDNSSeedData("soypay.org.us", "seed_us_0.dspay.org"));

        m_vchBase58Prefixes[EM_PUBKEY_ADDRESS] = IniCfg().GetAddressPrefix(EM_MAIN,EM_PUBKEY_ADDRESS);
		m_vchBase58Prefixes[EM_SCRIPT_ADDRESS] = IniCfg().GetAddressPrefix(EM_MAIN,EM_SCRIPT_ADDRESS);
		m_vchBase58Prefixes[EM_SECRET_KEY] = 	 IniCfg().GetAddressPrefix(EM_MAIN,EM_SECRET_KEY);
		m_vchBase58Prefixes[EM_EXT_PUBLIC_KEY] = IniCfg().GetAddressPrefix(EM_MAIN,EM_EXT_PUBLIC_KEY);
		m_vchBase58Prefixes[EM_EXT_SECRET_KEY] = IniCfg().GetAddressPrefix(EM_MAIN,EM_EXT_SECRET_KEY);

//        base58Prefixes[EM_PUBKEY_ADDRESS] = G_CONFIG_TABLE::AddrPrefix_mainNet[EM_PUBKEY_ADDRESS];
//		base58Prefixes[EM_SCRIPT_ADDRESS] = G_CONFIG_TABLE::AddrPrefix_mainNet[EM_SCRIPT_ADDRESS];
//		base58Prefixes[EM_SECRET_KEY] = G_CONFIG_TABLE::AddrPrefix_mainNet[EM_SECRET_KEY];
//		base58Prefixes[EM_EXT_PUBLIC_KEY] = G_CONFIG_TABLE::AddrPrefix_mainNet[EM_EXT_PUBLIC_KEY];
//		base58Prefixes[EM_EXT_SECRET_KEY] = G_CONFIG_TABLE::AddrPrefix_mainNet[EM_EXT_SECRET_KEY];

		// Convert the pnSeeds array into usable address objects.
		for (unsigned int i = 0; i < IniCfg().GetSeedNodeIP().size(); i++) {
			// It'll only connect to one or two seed nodes because once it connects,
			// it'll get a pile of addresses with newer timestamps.
			// Seed nodes are given a random 'last seen time' of between one and two
			// weeks ago.
			const int64_t nOneWeek = 7 * 24 * 60 * 60;
			struct in_addr ip;
			memcpy(&ip, &IniCfg().GetSeedNodeIP()[i], sizeof(ip));
			CAddress addr(CService(ip, GetDefaultPort()));
			addr.m_ullTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
			vFixedSeeds.push_back(addr);
		}
	}

	virtual const CBlock& GenesisBlock() const {
		return genesis;
	}
	virtual emNetWork NetworkID() const {
		return EM_MAIN;
	}
	virtual bool InitalConfig() {
		return CBaseParams::InitalConfig();
	}
	virtual int GetBlockMaxNonce() const {
		return 1000;
	}
	virtual const vector<CAddress>& FixedSeeds() const {
		return vFixedSeeds;
	}
	virtual bool IsInFixedSeeds(CAddress &addr) {
		vector<CAddress>::iterator iterAddr = find(vFixedSeeds.begin(), vFixedSeeds.end(), addr);
		return iterAddr != vFixedSeeds.end();
	}

protected:
	CBlock genesis;
	vector<CAddress> vFixedSeeds;
};

class CTestNetParams: public CMainParams {
public:
	CTestNetParams() {
		// The message start string is designed to be unlikely to occur in normal data.
		// The characters are rarely used upper ASCII, not valid as UTF-8, and produce
		// a large 4-byte int at any alignment.
		memcpy(m_pchMessageStart,IniCfg().GetMagicNumber(EM_TESTNET),sizeof(m_pchMessageStart));
//        pchMessageStart[0] = G_CONFIG_TABLE::Message_testNet[0];
//        pchMessageStart[1] = G_CONFIG_TABLE::Message_testNet[1];
//        pchMessageStart[2] = G_CONFIG_TABLE::Message_testNet[2];
//        pchMessageStart[3] = G_CONFIG_TABLE::Message_testNet[3];
		m_vchAlertPubKey =	ParseHex(IniCfg().GetCheckPointPkey(EM_TESTNET));
		m_nDefaultPort = IniCfg().GetnDefaultPort(EM_TESTNET) ;
		m_nRPCPort = IniCfg().GetnRPCPort(EM_TESTNET);
		m_nUIPort = IniCfg().GetnUIPort(EM_TESTNET);
		m_strDataDir = "testnet";
		m_strPublicKey = IniCfg().GetCheckPointPkey(EM_TESTNET);
		// Modify the testnet genesis block so the timestamp is valid for a later start.
		genesis.SetTime(IniCfg().GetStartTimeInit(EM_TESTNET));
		genesis.SetNonce(99);
		genesis.vptx.clear();
		assert(CreateGenesisRewardTx(genesis.vptx,IniCfg().GetIntPubKey(EM_TESTNET)));
		genesis.SetHashMerkleRoot(genesis.BuildMerkleTree());
		m_cHashGenesisBlock = genesis.GetHash();
		for(auto & item : vFixedSeeds)
			item.SetPort(GetDefaultPort());

//		{
//			cout << "testnet hashGenesisBlock:\r\n" << m_cHashGenesisBlock.ToString() << endl;
//		}
		assert(m_cHashGenesisBlock == IniCfg().GetIntHash(EM_TESTNET));
//		vSeeds.clear();
//		vSeeds.push_back(CDNSSeedData("bluematt.me", "testnet-seed.bluematt.me"));
        m_vchBase58Prefixes[EM_PUBKEY_ADDRESS] = IniCfg().GetAddressPrefix(EM_TESTNET,EM_PUBKEY_ADDRESS);
		m_vchBase58Prefixes[EM_SCRIPT_ADDRESS] = IniCfg().GetAddressPrefix(EM_TESTNET,EM_SCRIPT_ADDRESS);
		m_vchBase58Prefixes[EM_SECRET_KEY] = IniCfg().GetAddressPrefix(EM_TESTNET,EM_SECRET_KEY);
		m_vchBase58Prefixes[EM_EXT_PUBLIC_KEY] = IniCfg().GetAddressPrefix(EM_TESTNET,EM_EXT_PUBLIC_KEY);
		m_vchBase58Prefixes[EM_EXT_SECRET_KEY] = IniCfg().GetAddressPrefix(EM_TESTNET,EM_EXT_SECRET_KEY);
//        base58Prefixes[EM_PUBKEY_ADDRESS] = G_CONFIG_TABLE::AddrPrefix_testNet[EM_PUBKEY_ADDRESS];
//		base58Prefixes[EM_SCRIPT_ADDRESS] = G_CONFIG_TABLE::AddrPrefix_testNet[EM_SCRIPT_ADDRESS];
//		base58Prefixes[EM_SECRET_KEY] = G_CONFIG_TABLE::AddrPrefix_testNet[EM_SECRET_KEY];
//		base58Prefixes[EM_EXT_PUBLIC_KEY] = G_CONFIG_TABLE::AddrPrefix_testNet[EM_EXT_PUBLIC_KEY];
//		base58Prefixes[EM_EXT_SECRET_KEY] = G_CONFIG_TABLE::AddrPrefix_testNet[EM_EXT_SECRET_KEY];
	}
	virtual emNetWork NetworkID() const {return EM_TESTNET;}
	virtual bool InitalConfig()
	{
		CMainParams::InitalConfig();
		m_bServer = true;
		return true;
	}
	virtual int GetBlockMaxNonce() const
	{
		return 1000;
	}
};
//static CTestNetParams testNetParams;

//
// Regression test
//
class CRegTestParams: public CTestNetParams {
public:
	CRegTestParams() {
		memcpy(m_pchMessageStart,IniCfg().GetMagicNumber(EM_REGTEST),sizeof(m_pchMessageStart));
//		pchMessageStart[0] = G_CONFIG_TABLE::Message_regTest[0];
//		pchMessageStart[1] = G_CONFIG_TABLE::Message_regTest[1];
//		pchMessageStart[2] = G_CONFIG_TABLE::Message_regTest[2];
//		pchMessageStart[3] = G_CONFIG_TABLE::Message_regTest[3];
		m_nSubsidyHalvingInterval =  IniCfg().GetHalvingInterval(EM_REGTEST);
		m_bnProofOfStakeLimit = ~arith_uint256(0) >> 6;     //target:00000011 11111111 11111111
		genesis.SetTime(IniCfg().GetStartTimeInit(EM_REGTEST));
		genesis.SetBits(0x2003ffff);
		genesis.SetNonce(68);
		genesis.vptx.clear();
		assert(CreateGenesisRewardTx(genesis.vptx, IniCfg().GetIntPubKey(EM_REGTEST)));
		genesis.SetHashMerkleRoot(genesis.BuildMerkleTree());
		m_cHashGenesisBlock = genesis.GetHash();
		m_nDefaultPort = IniCfg().GetnDefaultPort(EM_REGTEST) ;
		m_llTargetSpacing = 20;
		m_llTargetTimespan = 30 * 20;
		m_strDataDir = "regtest";
//		{
//			CBigNum bnTarget;
//			bnTarget.SetCompact(genesis.GetBits());
//			cout << "regtest bnTarget:" << bnTarget.getuint256().GetHex() << endl;
//			cout << "regtest hashGenesisBlock:\r\n" << m_cHashGenesisBlock.ToString() << endl;

//		}
		assert(m_cHashGenesisBlock == IniCfg().GetIntHash(EM_REGTEST));

		vFixedSeeds.clear();
		m_vSeeds.clear();  // Regtest mode doesn't have any DNS seeds.
	}

	virtual bool RequireRPCPassword() const {
		return false;
	}
	virtual emNetWork NetworkID() const {
		return EM_REGTEST;
	}
	virtual bool InitalConfig() {
		CTestNetParams::InitalConfig();
		m_bServer = true;
		return true;
	}
};

/********************************************************************************/
const vector<string> &CBaseParams::GetMultiArgs(const string& strArg) {
	return m_mapMultiArgs[strArg];
}
int CBaseParams::GetArgsSize() {
	return m_mapArgs.size();
}
int CBaseParams::GetMultiArgsSize() {
	return m_mapMultiArgs.size();
}

string CBaseParams::GetArg(const string& strArg, const string& strDefault) {
	if (m_mapArgs.count(strArg)) {
		return m_mapArgs[strArg];
	}
	return strDefault;
}

int64_t CBaseParams::GetArg(const string& strArg, int64_t llDefault) {
	if (m_mapArgs.count(strArg)) {
		return atoi64(m_mapArgs[strArg]);
	}
	return llDefault;
}

bool CBaseParams::GetBoolArg(const string& strArg, bool bDefault) {
	if (m_mapArgs.count(strArg)) {
		if (m_mapArgs[strArg].empty()) {
			return true;
		}
		return (atoi(m_mapArgs[strArg]) != 0);
	}
	return bDefault;
}

bool CBaseParams::SoftSetArg(const string& strArg, const string& strValue) {
	if (m_mapArgs.count(strArg)) {
		return false;
	}
	m_mapArgs[strArg] = strValue;
	return true;
}

bool CBaseParams::SoftSetArgCover(const string& strArg, const string& strValue) {
	m_mapArgs[strArg] = strValue;
	return true;
}

void CBaseParams::EraseArg(const string& strArgKey) {
	m_mapArgs.erase(strArgKey);
}

bool CBaseParams::SoftSetBoolArg(const string& strArg, bool bValue) {
	if (bValue) {
		return SoftSetArg(strArg, string("1"));
	} else {
		return SoftSetArg(strArg, string("0"));
	}
}

bool CBaseParams::IsArgCount(const string& strArg) {
	if (m_mapArgs.count(strArg)) {
		return true;
	}
	return false;
}

CBaseParams &SysCfg() {
	static shared_ptr<CBaseParams> pParams;

	if (pParams.get() == NULL) {
		bool bRegTest = CBaseParams::GetBoolArg("-regtest", false);
		bool bTestNet = CBaseParams::GetBoolArg("-testnet", false);
		if (bTestNet && bRegTest) {
			fprintf(stderr, "Error: Invalid combination of -regtest and -testnet.\n");
//			assert(0);
		}

		if (bRegTest) {
			//LogPrint("spark", "In Reg Test Net\n");
			//printf("In Reg Test Net\n");
			pParams = std::make_shared<CRegTestParams>();
		} else if (bTestNet) {
			//LogPrint("spark", "In Test Net\n");
			//printf("In  Test Net\n");
			pParams = std::make_shared<CTestNetParams>();
		} else {
			//LogPrint("spark", "In Main Net\n");
			//printf("In Main Net\n");
			pParams = std::make_shared<CMainParams>();
		}
/*
		CKey key;
		    key.MakeNewKey(true);

			CPubKey pubKey = key.GetPubKey();
			CPrivKey privKey = key.GetPrivKey();

			printf("pub key:\n");
			printf("%s\n", pubKey.ToString().c_str());
			printf("\n");

			CKeyID keyID = pubKey.GetKeyID();
			CCoinAddress b;
			b.Set(keyID);
			printf("base58 addr:\n");
			printf("%s\n", b.ToString().c_str());

			printf("priv key:\n");
			for(int j = 0; j < privKey.size(); j++) {
				printf("%02x", privKey[j]);
			}
			printf("\n");

			CCoinSecret s;
			s.SetKey(key);
			printf("secret key:\n");
			printf("%s\n", s.ToString().c_str());

			printf("===============\n");
*/			

	}
	assert(pParams != NULL);
	return *pParams.get();
}

//write for test code
const CBaseParams &SysParamsMain() {
	static std::shared_ptr<CBaseParams> pParams;
	pParams = std::make_shared<CMainParams>();
	assert(pParams != NULL);
	return *pParams.get();
}

//write for test code
const CBaseParams &SysParamsTest() {
	static std::shared_ptr<CBaseParams> pParams;
	pParams = std::make_shared<CTestNetParams>();
	assert(pParams != NULL);
	return *pParams.get();
}

//write for test code
const CBaseParams &SysParamsReg() {
	static std::shared_ptr<CBaseParams> pParams;
	pParams = std::make_shared<CRegTestParams>();
	assert(pParams != NULL);
	return *pParams.get();
}

static void InterpretNegativeSetting(string name, map<string, string>& mapSettingsRet) {
	// interpret -nofoo as -foo=0 (and -nofoo=0 as -foo=1) as long as -foo not set
	if (name.find("-no") == 0) {
		string positive("-");
		positive.append(name.begin() + 3, name.end());
		if (mapSettingsRet.count(positive) == 0) {
			bool value = !SysCfg().GetBoolArg(name, false);
			mapSettingsRet[positive] = (value ? "1" : "0");
		}
	}
}

void CBaseParams::ParseParameters(int argc, const char* const argv[]) {
	m_mapArgs.clear();
	m_mapMultiArgs.clear();
	for (int i = 1; i < argc; i++) {
		string str(argv[i]);
		string strValue;
		size_t is_index = str.find('=');
		if (is_index != string::npos) {
			strValue = str.substr(is_index + 1);
			str = str.substr(0, is_index);
		}
#ifdef WIN32
		boost::to_lower(str);
		if (boost::algorithm::starts_with(str, "/")) {
			str = "-" + str.substr(1);
		}
#endif
		if (str[0] != '-') {
			break;
		}
		m_mapArgs[str] = strValue;
		m_mapMultiArgs[str].push_back(strValue);
	}

	// New 0.6 features:
//	BOOST_FOREACH(const PAIRTYPE(string,string)& entry, m_mapArgs) {
	for (auto& entry : m_mapArgs) {
		string name = entry.first;

		//  interpret --foo as -foo (as long as both are not set)
		if (name.find("--") == 0) {
			string singleDash(name.begin() + 1, name.end());
			if (m_mapArgs.count(singleDash) == 0) {
				m_mapArgs[singleDash] = entry.second;
			}
			name = singleDash;
		}

		// interpret -nofoo as -foo=0 (and -nofoo=0 as -foo=1) as long as -foo not set
		InterpretNegativeSetting(name, m_mapArgs);
	}
#if 0
	for(const auto& tmp:m_mapArgs) {
		printf("key:%s - value:%s\n", tmp.first.c_str(), tmp.second.c_str());
	}
#endif
}

bool CBaseParams::CreateGenesisRewardTx(vector<std::shared_ptr<CBaseTransaction> > &vRewardTx, const vector<string> &vInitPubKey) {
	int nLength = vInitPubKey.size();
	for (int i = 0; i < nLength; ++i) {
		int64_t money(0);
		if (i > 0) {
			if(i == 1) 
			{
				int64_t t = 1500000;
				money = t * COIN;
			}
			else 
			{
				money = IniCfg().GetCoinInitValue() * COIN;
			}
		}
		shared_ptr<CRewardTransaction> pRewardTx = std::make_shared<CRewardTransaction>(ParseHex(vInitPubKey[i].c_str()), money, 0);
		pRewardTx->m_nVersion = g_sTxVersion1;
		if (pRewardTx.get())
			vRewardTx.push_back(pRewardTx);
		else
			return false;
	}
	return true;

};

bool CBaseParams::IntialParams(int argc, const char* const argv[]) {
	ParseParameters(argc, argv);
	if (!boost::filesystem::is_directory(GetDataDir(false))) {
		fprintf(stderr, "Error: Specified data directory \"%s\" does not exist.\n", CBaseParams::m_mapArgs["-datadir"].c_str());
		return false;
	}
	try {
		ReadConfigFile(CBaseParams::m_mapArgs, CBaseParams::m_mapMultiArgs);
	} catch (exception &e) {
		fprintf(stderr, "Error reading configuration file: %s\n", e.what());
		return false;
	}
	return true;
}

int64_t CBaseParams::GetTxFee() const{
     return m_llpaytxfee;
}
int64_t CBaseParams::SetDeflautTxFee(int64_t llFee)const{
	m_llpaytxfee = llFee;

	return llFee;
}

CBaseParams::CBaseParams() {
	m_bImporting 			= false;
	m_bReindex 				= false;
	m_bBenchmark 			= false;
	m_bTxIndex 				= false;
	m_nIntervalPos 			= 1;
	m_nLogmaxsize 			= 100 * 1024 * 1024;//100M
	m_nTxCacheHeight 		= 500;
	m_llTimeBestReceived 	= 0;
	m_unScriptCheckThreads 	= 0;
	m_llViewCacheSize 		= 2000000;
	m_llTargetSpacing 		= 60;
	m_llTargetTimespan 		= 30 * 60;
	m_nSubsidyHalvingInterval = 0;
	m_llpaytxfee 			= 10000;
	m_nDefaultPort 			= 0;
	m_bPrintToConsole		= 0;
	m_bPrintToToFile 		= 0;
	m_bLogTimestamps 		= 0;
	m_bLogPrintFileLine 	= 0;
	m_bDebug 				= 0;
	m_bDebugAll				= 0 ;
	m_bServer 				= 0 ;
	m_bServer 				= 0 ;
	m_nRPCPort 				= 0;
	m_bOutPut 				= false;
	m_nUIPort 				= 0;

}
/********************************************************************************/

