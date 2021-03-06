
// HTTPServer.h

// Declares the cHTTPServer class representing a HTTP webserver that uses cListenThread and cSocketThreads for processing





#pragma once

#include "../OSSupport/Network.h"
#include "../IniFile.h"
#include "PolarSSL++/RsaPrivateKey.h"
#include "PolarSSL++/CryptoKey.h"
#include "PolarSSL++/X509Cert.h"





// fwd:
class cHTTPMessage;
class cHTTPRequest;
class cHTTPResponse;
class cHTTPConnection;

typedef std::vector<cHTTPConnection *> cHTTPConnections;






class cHTTPServer
{
public:
	class cCallbacks
	{
	public:
		virtual ~cCallbacks() {}

		/** Called when a new request arrives over a connection and all its headers have been parsed.
		The request body needn't have arrived yet. */
		virtual void OnRequestBegun(cHTTPConnection & a_Connection, cHTTPRequest & a_Request) = 0;

		/** Called when another part of request body has arrived.
		May be called multiple times for a single request. */
		virtual void OnRequestBody(cHTTPConnection & a_Connection, cHTTPRequest & a_Request, const char * a_Data, size_t a_Size) = 0;

		/** Called when the request body has been fully received in previous calls to OnRequestBody() */
		virtual void OnRequestFinished(cHTTPConnection & a_Connection, cHTTPRequest & a_Request) = 0;
	} ;

	cHTTPServer(void);
	virtual ~cHTTPServer();

	/** Initializes the server - reads the cert files etc. */
	bool Initialize(void);

	/** Starts the server and assigns the callbacks to use for incoming requests */
	bool Start(cCallbacks & a_Callbacks, const AStringVector & a_Ports);

	/** Stops the server, drops all current connections */
	void Stop(void);

protected:
	friend class cHTTPConnection;
	friend class cSslHTTPConnection;
	friend class cHTTPServerListenCallbacks;

	/** The cNetwork API handle for the listening socket. */
	cServerHandlePtrs m_ServerHandles;

	/** The callbacks to call for various events */
	cCallbacks * m_Callbacks;

	/** The server certificate to use for the SSL connections */
	cX509CertPtr m_Cert;

	/** The private key for m_Cert. */
	cCryptoKeyPtr m_CertPrivKey;


	/** Called by cHTTPServerListenCallbacks when there's a new incoming connection.
	Returns the connection instance to be used as the cTCPLink callbacks. */
	cTCPLink::cCallbacksPtr OnIncomingConnection(const AString & a_RemoteIPAddress, UInt16 a_RemotePort);

	/** Called by cHTTPConnection when it finishes parsing the request header */
	void NewRequest(cHTTPConnection & a_Connection, cHTTPRequest & a_Request);

	/** Called by cHTTPConenction when it receives more data for the request body.
	May be called multiple times for a single request. */
	void RequestBody(cHTTPConnection & a_Connection, cHTTPRequest & a_Request, const char * a_Data, size_t a_Size);

	/** Called by cHTTPConnection when it detects that the request has finished (all of its body has been received) */
	void RequestFinished(cHTTPConnection & a_Connection, cHTTPRequest & a_Request);
} ;





