#ifndef CLIENTCOMMANDDEVICEAPI_H
#define CLIENTCOMMANDDEVICEAPI_H

#include "ClientCommandBase.h"

namespace QtuC
{

/** DeviceAPI command.
  *	In a deviceAPI command, the current device API string can be sent to a client.*/
class ClientCommandDeviceApi : public ClientCommandBase
{
public:
	ClientCommandDeviceApi( const QString &deviceApiString = QString() );

	const QByteArray getHash() const
		{ return mApiHash; }

	const QByteArray getEncodedApi() const
		{ return mApiB64; }

	/** Get if the API data is valid.
	  *	Checks the API hash against the base64 encoded API data.
	  *	@return True if the API data matches the hash, so the data is valid, return false if not.*/
	bool isDataValid() const;


	/// @name Inherited methods from ClientCommandBase.
	/// @{

	bool applyDomElement( const QDomElement &cmdElement );

	ClientCommandBase *clone();

	ClientCommandBase *exactClone();

	QDomElement getDomElement() const;

	bool isValid() const;

	/// @}

private:
	bool mIsPermanent;
	QByteArray mApiB64;
	QByteArray mApiHash;
};

}	//QtuC::
#endif // CLIENTCOMMANDDEVICEAPI_H
