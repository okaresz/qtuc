#ifndef CLIENTCOMMANDREQDEVICEAPI_H
#define CLIENTCOMMANDREQDEVICEAPI_H

#include "ClientCommandBase.h"

namespace QtuC
{

/** reqDeviceAPI command.
  *	Request a deviceAPI command from the proxy.*/
class ClientCommandReqDeviceApi : public ClientCommandBase
{
	Q_OBJECT
public:
	explicit ClientCommandReqDeviceApi();


	/// @name Inherited methods from ClientCommandBase.
	/// @{

	bool applyDomElement( const QDomElement &cmdElement );

	ClientCommandBase *clone();

	ClientCommandBase *exactClone();

	QDomElement getDomElement() const;

	bool isValid() const;

	/// @}
};

}	//QtuC::
#endif // CLIENTCOMMANDREQDEVICEAPI_H
