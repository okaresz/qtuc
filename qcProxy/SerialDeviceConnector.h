#ifndef SERIALDEVICECONNECTOR_H
#define SERIALDEVICECONNECTOR_H

#include "DeviceConnectionManagerBase.h"
#include <serialport.h>

namespace QtuC
{

class SerialDeviceConnector : public DeviceConnectionManagerBase
{
	Q_OBJECT
public:

	SerialDeviceConnector( QObject *parent );

	~SerialDeviceConnector();

	/// Inherited from DeviceConnectionManagerBase.
	virtual void sendCommand( const DeviceCommand &cmd );

	/// Inherited from DeviceConnectionManagerBase.
	virtual void closeDevice();

	/// Inherited from DeviceConnectionManagerBase.
	virtual bool openDevice();

	/** Close serial port.*/
	void closePort();

	/** Initialize communication (init serial) and open it.
	  *	@return True on success, false otherwise.*/
	bool connectPort();

private slots:

	/** Handle a string part received on the serial port.
	 *	Called on readyRead(), reads available data, checks for newline, and emits commandReceived() if necessary.*/
	void receivePart();

private:
	QString mCmdRxBuffer;
	QString mCmdRxBufferShadow;
	QtAddOn::SerialPort::SerialPort *mSerialPort;
};

}	//QtuC::
#endif //SERIALDEVICECONNECTOR_H

