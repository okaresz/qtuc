#ifndef DEVICECONNECTIONMANAGERBASE_H
#define DEVICECONNECTIONMANAGERBASE_H

#include <QObject>
#include <DeviceCommand.h>
#include "ErrorHandlerBase.h"

namespace QtuC
{

/** DeviceConnectionManagerBase class.
 *	The base class for implementing a device connection manager class.*/
class DeviceConnectionManagerBase : public ErrorHandlerBase
{
	Q_OBJECT
public:

	/** C'tor*/
	DeviceConnectionManagerBase( QObject *parent );

	~DeviceConnectionManagerBase();

	/** Send command.
	 *	Put the command to the send queue.
	 *	@param cmd The command to send,*/
	virtual void sendCommand( const DeviceCommand &cmd ) = 0;

	/** Close the device connection.*/
	virtual void closeDevice() = 0;

	/** Open device connection.
	 *	@return True on success, false otherwise.*/
	virtual bool openDevice() = 0;

signals:

	/** Emitted when a command is received.
	  *	@param cmd The command object.*/
	void commandReceived( DeviceCommand cmd );

protected:
	/** Build a device command object from string.
	  *	@param cmdStr The string representation of the command.
	  *	@return Pointer to a DeviceCommand object on success, 0 on failure.*/
	DeviceCommand* buildCommandFromString( const QString &cmdStr );
};

}	//QtuC::
#endif //DEVICECONNECTIONMANAGERBASE_H

