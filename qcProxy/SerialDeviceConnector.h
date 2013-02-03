#ifndef SERIALDEVICECONNECTOR_H
#define SERIALDEVICECONNECTOR_H

#include "DeviceConnectionManagerBase.h"
#include <serialport.h>

namespace QtuC
{

/** Class to connect a device via a serial link.*/
class SerialDeviceConnector : public DeviceConnectionManagerBase
{
	Q_OBJECT
public:

	/** Create.*/
	SerialDeviceConnector( QObject *parent = 0 );

	~SerialDeviceConnector();

	/** @name Inherited from DeviceConnectionManagerBase.
	  *	@{*/
	bool sendCommand( DeviceCommand *cmd );

	void closeDevice();		///< Close serial port.

	/** Open and initialize serial port.
	  *	@return True on success, false otherwise.*/
	bool openDevice();
	/// @}

private slots:

	/** Handle a string part received on the serial port.
	 *	Called on readyRead(), reads available data, checks for newline, and emits commandReceived() a full command has been received.*/
	void receivePart();

private:
	QString mCmdRxBuffer;
	QtAddOn::SerialPort::SerialPort *mSerialPort;
};

}	//QtuC::
#endif //SERIALDEVICECONNECTOR_H

