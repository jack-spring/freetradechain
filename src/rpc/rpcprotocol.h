// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2016 The Coin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COIN_RPC_RPCPROTOCOL_H_
#define COIN_RPC_RPCPROTOCOL_H_ 1

#include <list>
#include <map>
#include <stdint.h>
#include <string>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "json/json_spirit_reader_template.h"
#include "json/json_spirit_utils.h"
#include "json/json_spirit_writer_template.h"
using namespace std;
// HTTP status codes
enum HTTPStatusCode {
    HTTP_OK                    = 200,
    HTTP_BAD_REQUEST           = 400,
    HTTP_UNAUTHORIZED          = 401,
    HTTP_FORBIDDEN             = 403,
    HTTP_NOT_FOUND             = 404,
    HTTP_INTERNAL_SERVER_ERROR = 500,
};

// Coin RPC error codes
enum RPCErrorCode {
    // Standard JSON-RPC 2.0 errors
    RPC_INVALID_REQUEST  = -32600,
    RPC_METHOD_NOT_FOUND = -32601,
    RPC_INVALID_PARAMS   = -32602,
    RPC_INTERNAL_ERROR   = -32603,
    RPC_PARSE_ERROR      = -32700,

    // General application defined errors
    RPC_MISC_ERROR                  = -1,  // exception thrown in command handling
    RPC_FORBIDDEN_BY_SAFE_MODE      = -2,  // Server is in safe mode, and command is not allowed in safe mode
    RPC_TYPE_ERROR                  = -3,  // Unexpected type was passed as parameter
    RPC_INVALID_ADDRESS_OR_KEY      = -5,  // Invalid address or key
    RPC_OUT_OF_MEMORY               = -7,  // Ran out of memory during operation
    RPC_INVALID_PARAMETER           = -8,  // Invalid, missing or duplicate parameter
    RPC_DATABASE_ERROR              = -20, // Database error
    RPC_DESERIALIZATION_ERROR       = -22, // Error parsing or validating structure in raw format
    RPC_TRANSACTION_ERROR           = -25, // General error during transaction submission
    RPC_TRANSACTION_REJECTED        = -26, // Transaction was rejected by network rules
    RPC_TRANSACTION_ALREADY_IN_CHAIN= -27, // Transaction already in chain

    // P2P client errors
    RPC_CLIENT_NOT_CONNECTED        = -9,  // Coin is not connected
    RPC_CLIENT_IN_INITIAL_DOWNLOAD  = -10, // Still downloading initial blocks
    RPC_CLIENT_NODE_ALREADY_ADDED   = -23, // Node is already added
    RPC_CLIENT_NODE_NOT_ADDED       = -24, // Node has not been added before

    // Wallet errors
    RPC_WALLET_ERROR                = -4,  // Unspecified problem with wallet (key not found etc.)
    RPC_WALLET_INSUFFICIENT_FUNDS   = -6,  // Not enough funds in wallet or account
    RPC_WALLET_INVALID_ACCOUNT_NAME = -11, // Invalid account name
    RPC_WALLET_KEYPOOL_RAN_OUT      = -12, // Keypool ran out, call keypoolrefill first
    RPC_WALLET_UNLOCK_NEEDED        = -13, // Enter the wallet passphrase with walletpassphrase first
    RPC_WALLET_PASSPHRASE_INCORRECT = -14, // The wallet passphrase entered was incorrect
    RPC_WALLET_WRONG_ENC_STATE      = -15, // Command given in wrong wallet encryption state (encrypting an encrypted wallet etc.)
    RPC_WALLET_ENCRYPTION_FAILED    = -16, // Failed to encrypt the wallet
    RPC_WALLET_ALREADY_UNLOCKED     = -17, // Wallet is already unlocked
};

//
// IOStream device that speaks SSL but can also speak non-SSL
//
template <typename Protocol>
class SSLIOStreamDevice : public boost::iostreams::device<boost::iostreams::bidirectional> {
 public:
    SSLIOStreamDevice(boost::asio::ssl::stream<typename Protocol::socket> &streamIn, bool bUseSSLIn) : m_stream(streamIn) {
        m_bUseSSL = bUseSSLIn;
        m_bNeedHandshake = bUseSSLIn;
    }

	void handshake(boost::asio::ssl::stream_base::handshake_type role) {
		if (!m_bNeedHandshake) {
			return;
		}
		m_bNeedHandshake = false;
		m_stream.handshake(role);
	}

	streamsize read(char* s, streamsize n) {
		handshake(boost::asio::ssl::stream_base::server); // HTTPS servers read first
		if (m_bUseSSL) {
			return m_stream.read_some(boost::asio::buffer(s, n));
		}
		return m_stream.next_layer().read_some(boost::asio::buffer(s, n));

	}

	streamsize write(const char* s, streamsize n) {
		handshake(boost::asio::ssl::stream_base::client); // HTTPS clients write first
		if (m_bUseSSL) {
			return boost::asio::write(m_stream, boost::asio::buffer(s, n));
		}
		return boost::asio::write(m_stream.next_layer(), boost::asio::buffer(s, n));
	}

	bool connect(const string& strServer, const string& strPort) {
		boost::asio::ip::tcp::resolver resolver(m_stream.get_io_service());
		boost::asio::ip::tcp::resolver::query query(strServer.c_str(), strPort.c_str());
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		boost::asio::ip::tcp::resolver::iterator end;
		boost::system::error_code error = boost::asio::error::host_not_found;
		while (error && endpoint_iterator != end) {
			m_stream.lowest_layer().close();
			m_stream.lowest_layer().connect(*endpoint_iterator++, error);
		}
		if (error) {
			return false;
		}
		return true;
	}

 private:
    bool m_bNeedHandshake;
    bool m_bUseSSL;
    boost::asio::ssl::stream<typename Protocol::socket>& m_stream;
};

string HTTPPost(const string& strMsg, const map<string,string>& mapRequestHeaders);
string HTTPReply(int nStatus, const string& strMsg, bool bKeepalive);
bool ReadHTTPRequestLine(basic_istream<char>& stream, int &nProto,string& strHttpMethod, string& strHttpUri);
int ReadHTTPStatus(basic_istream<char>& stream, int &nProto);
int ReadHTTPHeaders(basic_istream<char>& stream, map<string, string>& mapHeadersRet);
int ReadHTTPMessage(basic_istream<char>& stream, map<string, string>& mapHeadersRet,string& strMessageRet, int nProto);
string JSONRPCRequest(const string& strMethod, const json_spirit::Array& params, const json_spirit::Value& id);
json_spirit::Object JSONRPCReplyObj(const json_spirit::Value& result, const json_spirit::Value& error, const json_spirit::Value& id);
string JSONRPCReply(const json_spirit::Value& result, const json_spirit::Value& error, const json_spirit::Value& id);
json_spirit::Object JSONRPCError(int nCode, const string& strMessage);

#endif