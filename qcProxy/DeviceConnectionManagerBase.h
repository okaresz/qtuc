#ifndef DEVICECONNECTIONMANAGERBASE_H
#define DEVICECONNECTIONMANAGERBASE_H

#include "DeviceCommand.h"

namespace QtuC
{

/** DeviceConnectionManagerBase class.
 *	The base class for implementing a device connection manager class.*/
class DeviceConnectionManagerBase : public ErrorHandlerBase
{
	Q_OBJECT
public:

	/** C'tor*/
	DeviceConnectionManagerBase( QObject *parent = 0 );

	//virtual ~DeviceConnectionManagerBase()=0;

	/** Send command.
	 *	Put the command to the send queue.
	 *	@param cmd The command to send,*/
	virtual bool sendCommand( DeviceCommand *cmd ) = 0;

	/** Close the device connection.*/
	virtual void closeDevice() = 0;

	/** Open device connection.
	 *	@return True on success, false otherwise.*/
	virtual bool openDevice() = 0;

signals:

	/** Emitted when a command is received.
	  *	@param cmd The command object.*/
	void commandReceived( DeviceCommand *cmd );

};

}	//QtuC::
#endif //DEVICECONNECTIONMANAGERBASE_H

