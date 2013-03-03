#ifndef CLIENTCOMMANDREQDEVICEINFO_H
#define CLIENTCOMMANDREQDEVICEINFO_H

#include "ClientCommandBase.h"

namespace QtuC
{

/** reqDeviceInfo command.
  *	Request a deviceInfo command from the proxy.*/
class ClientCommandReqDeviceInfo : public ClientCommandBase
{
	Q_OBJECT
public:
	explicit ClientCommandReqDeviceInfo();

	/// @name Inherited methods from ClientCommandBase.
	/// @{

	bool applyDomElement( const QDomElement &cmdElement );

	ClientCommandBase *clone();

	ClientCommandBase *exactClone();

	QDomElement getDomElement() const;

	inline bool isValid() const
		{ return ClientCommandBase::isValid(); }

	/// @}
};

}	//QtuC::
#endif // CLIENTCOMMANDREQDEVICEINFO_H
