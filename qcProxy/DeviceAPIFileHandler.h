#ifndef DEVICEAPIFILEHANDLER_H
#define DEVICEAPIFILEHANDLER_H

#include "DeviceAPIParser.h"
#include <QFileSystemWatcher>

namespace QtuC
{

/** DeviceAPIFileHandler class.
 *	Handles the device API definition file: load, save, watch for change and update the file if needed.*/
class DeviceAPIFileHandler : public DeviceAPIParser
{
	Q_OBJECT
public:
	/** C'tor*/
	DeviceAPIFileHandler( QObject *parent = 0 );

	/** Load the API definition file.
	 *	@param apiFilePath The path of the device API file. If omitted, path will be read from settings.
	 *	@return True on success, false otherwise.*/
	bool load( const QString &apiFilePath = QString() );

	/** Save the API definition file.
	 *	@param apiFilePath The path of the device API file. If omitted, it will be the path of the current API, or the on in the settings.
	 *	@return True on success, false otherwise.*/
	bool save( const QString &apiFilePath = QString() );

	/** Watch API file for changes and apply them immediately.*/
	void startWatch();

	/** Stop watching the API file for changes.*/
	void stopWatch();

public slots:

	/** Reload the API definition file.
	 *	@return True on success, false otherwise.*/
	bool reload();

signals:
	void apiFileChanged();

private:
	QString mCurrentAPIFilePath;
	QFileSystemWatcher *mApiFileWatcher;

};

}	//QtuC::
#endif //DEVICEAPIFILEHANDLER_H

