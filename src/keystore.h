// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2016 The Coin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COIN_KEYSTORE_H_
#define COIN_KEYSTORE_H_

#include "key.h"
#include "sync.h"
#include "wallet/walletdb.h"
#include <set>
#include "json/json_spirit_value.h"
#include "json/json_spirit_writer_template.h"
#include "json/json_spirit_utils.h"
#include <boost/signals2/signal.hpp>

using namespace json_spirit;

class CKeyCombi {
 public:
	CKeyCombi();
	CKeyCombi(CKey const &cInkey, CKey const &cMinerKey, int nVersion);
	CKeyCombi(CKey const &cInkey, int nVersion);

	string ToString() const;

	Object ToJsonObj() const;
	bool UnSersailFromJson(const Object&);
	int64_t GetBirthDay() const;
	bool GetCKey(CKey& ckeyOut, bool bIsMine = false) const;
	bool CreateANewKey();
	bool GetPubKey(CPubKey &cOutKey, bool bIsMine = false) const;
	bool CleanMainKey();
	bool CleanAll();
	bool IsContainMinerKey() const;
	bool IsContainMainKey() const;
	CKeyID GetCKeyID() const;
	void SetMainKey(CKey& cMainKey);
	void SetMinerKey(CKey & cMinerKey);

	IMPLEMENT_SERIALIZE
	(
			if(0 == nVersion) {
				READWRITE(m_cMainPKey);
			}
			READWRITE(m_cMainCkey);
			if(0 == nVersion) {
				READWRITE(m_cMinerPKey);
			}
			READWRITE(m_cMinerCkey);
			READWRITE(m_llCreationTime);
	)

 private:
	CPubKey m_cMainPKey;
	CPubKey m_cMinerPKey;
	CKey m_cMainCkey;
	CKey m_cMinerCkey; //only used for miner
	int64_t m_llCreationTime;

};

/** A virtual base class for key stores */
class CKeyStore {
 protected:
	mutable CCriticalSection cs_KeyStore;

 public:
	virtual ~CKeyStore() {
	}

	// Add a key to the store.
	virtual bool AddKeyCombi(const CKeyID & keyId, const CKeyCombi &keyCombi) = 0;
	// virtual bool AddKeyPubKey(const CKey &key, const CPubKey &pubkey) =0;
	// virtual bool AddKey(const CKey &key);

	// Check whether a key corresponding to a given address is present in the store.
	virtual bool HaveKey(const CKeyID &address) const =0;
	virtual bool GetKey(const CKeyID &address, CKey& keyOut, bool IsMine) const =0;
	virtual void GetKeys(set<CKeyID> &setAddress, bool bFlag) const =0;
	virtual bool GetPubKey(const CKeyID &address, CPubKey& vchPubKeyOut, bool IsMine) const;

	// Support for BIP 0013 : see https://github.com/bitcoin/bips/blob/master/bip-0013.mediawiki
	// virtual bool AddCScript(const CScript& redeemScript) =0;
	// virtual bool HaveCScript(const CScriptID &hash) const =0;
	// virtual bool GetCScript(const CScriptID &hash, CScript& redeemScriptOut) const =0;
};

typedef map<CKeyID, CKeyCombi> KeyMap;
//typedef map<CScriptID, CScript > ScriptMap;

/** Basic key store, that keeps keys in an address->secret map */
class CBasicKeyStore : public CKeyStore {
 protected:
	KeyMap mapKeys;
	//  ScriptMap mapScripts;

 public:
	bool AddKeyCombi(const CKeyID & cKeyId, const CKeyCombi &cKeyCombi);
	bool HaveKey(const CKeyID &cAddress) const {
		bool bResult;
		{
			LOCK(cs_KeyStore);
			bResult = (mapKeys.count(cAddress) > 0);
		}
		return bResult;
	}

	void GetKeys(set<CKeyID> &setAddress, bool bFlag = false) const {
		setAddress.clear();
		{
			LOCK(cs_KeyStore);
			KeyMap::const_iterator mi = mapKeys.begin();
			while (mi != mapKeys.end()) {
				if (!bFlag) {
					setAddress.insert((*mi).first);
				} else if (mi->second.IsContainMinerKey() || mi->second.IsContainMainKey()) {
					setAddress.insert((*mi).first); //only return satisfied mining address
				}
				mi++;
			}
		}
	}

	bool GetKey(const CKeyID &cAddress, CKey &cKeyOut, bool bIsMine = false) const {
		{
			LOCK(cs_KeyStore);
			KeyMap::const_iterator mi = mapKeys.find(cAddress);
			if (mi != mapKeys.end()) {
				return mi->second.GetCKey(cKeyOut, bIsMine);
			}
		}
		return false;
	}
	virtual bool GetKeyCombi(const CKeyID &cAddress, CKeyCombi &cKeyCombiOut) const;
	bool IsContainMainKey() {
		for (auto &item : mapKeys) {
			if (item.second.IsContainMainKey()) {
				return true;
			}
		}
		return false;
	}

	// virtual bool AddCScript(const CScript& redeemScript);
	// virtual bool HaveCScript(const CScriptID &hash) const;
	// virtual bool GetCScript(const CScriptID &hash, CScript& redeemScriptOut) const;
};

typedef vector<unsigned char, secure_allocator<unsigned char> > CKeyingMaterial;
typedef map<CKeyID, pair<CPubKey, vector<unsigned char> > > CryptedKeyMap;

#endif
