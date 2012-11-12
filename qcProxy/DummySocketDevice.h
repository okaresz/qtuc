#ifndef DUMMYFILEDEVICE_H
#define DUMMYFILEDEVICE_H

#include "DeviceConnectionManagerBase.h"
#include <QTcpSocket>
#include <QTcpServer>

namespace QtuC
{

/** This is a socket reader class for debug purposes to simulate a device.
 *	DummySocketDevice will listen on a local port for a TCP connection. Only one client is accepted. You can send and receive device commands on the open socket.
 *	The /quit/ command closes the connection.*/
class DummySocketDevice : public DeviceConnectionManagerBase
{
	Q_OBJECT
public:
	explicit DummySocketDevice(QObject *parent = 0);

	~DummySocketDevice();

	/// Inherited from DeviceConnectionManagerBase.
	bool sendCommand( DeviceCommandBuilder *cmd );

	/// Inherited from DeviceConnectionManagerBase.
	void closeDevice();

	/// Inherited from DeviceConnectionManagerBase.
	bool openDevice();

signals:
	/// Emitted when a close request is received from the dummy device (quit command)
	void quitRequestFromDevice();

private slots:

	/** Handle device data.
	 *	Called on readyRead(), reads available data, checks for newline, and emits commandReceived() if necessary.*/
	void handleDeviceData();

	/** Handle socket errors.*/
	void handleSocketError( QAbstractSocket::SocketError errorNum );

	/** Handle new connected dummy device*/
	void handleDeviceClientConnected();

	/// Handle dummy device quit.
	void handleQuitRequestFromDevice();

private:
	QString mCmdRxBuffer;
	QString mCmdRxBufferShadow;
	QTcpSocket *mDeviceSocket;
	QTcpServer *mDeviceServer;
};

}	//QtuC::
#endif // DUMMYFILEDEVICE_H
