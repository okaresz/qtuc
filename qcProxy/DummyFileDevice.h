#ifndef DUMMYFILEDEVICE_H
#define DUMMYFILEDEVICE_H

#include "DeviceConnectionManagerBase.h"
#include <QFile>

namespace QtuC
{

/** This is a dummy file reader class to fake device connection.
  * Reads and writes a predefined file.*/
class DummyFileDevice : public DeviceConnectionManagerBase
{
	Q_OBJECT
public:
	explicit DummyFileDevice(QObject *parent = 0);

	~DummyFileDevice();

	/// Inherited from DeviceConnectionManagerBase.
	bool sendCommand( const DeviceCommand &cmd );

	/// Inherited from DeviceConnectionManagerBase.
	void closeDevice();

	/// Inherited from DeviceConnectionManagerBase.
	bool openDevice();

	/** Close dummy device file.*/
	void closeFile();

	/** Open dummy device file.
	  *	@return True on success, false otherwise.*/
	bool openFile();

private slots:

	/** Handle a string part received.
	 *	Called on readyRead(), reads available data, checks for newline, and emits commandReceived() if necessary.*/
	void receivePart();

private:
	QString mCmdRxBuffer;
	QString mCmdRxBufferShadow;
	QFile *mDeviceFile;
};

}	//QtuC::
#endif // DUMMYFILEDEVICE_H
