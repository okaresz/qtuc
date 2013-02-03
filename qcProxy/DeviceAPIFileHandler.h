#ifndef DEVICEAPIFILEHANDLER_H
#define DEVICEAPIFILEHANDLER_H

#include "DeviceAPIParser.h"
#include <QFileSystemWatcher>

namespace QtuC
{

/** DeviceAPIFileHandler class.
 *	Handles the device API definition file: load, save, watch for change and update the file if needed.
 *	Derived from DeviceAPIParser, so this class is capable of handling all necessary DeviceAPI-related activity.*/
class DeviceAPIFileHandler : public DeviceAPIParser
{
	Q_OBJECT
public:

	/// Creates the DeviceAPIFileHandler object.
	DeviceAPIFileHandler( QObject *parent = 0 );

	~DeviceAPIFileHandler();

	/** Load the API definition file.
	 *	This function can only be called if this parser object is empty (see isEmpty()).
	 *	If you would like to replace the API, use reload().
	 *	@param apiFilePath The path of the device API file. If omitted, path will be read from settings.
	 *	@return True on success, false otherwise.*/
	bool load( const QString &apiFilePath = QString() );

	/** Save the API definition file.
	 *	@param apiFilePath The path of the device API file. If omitted, it will be the path of the current API, or the one in the settings.
	 *	@return True on success, false otherwise.*/
	bool save( const QString &apiFilePath = QString() );

	/** Return if this DeviceAPIFileHandler is empty.
	  *	Object is empty when there's no stored API and filepath.
	  *	@return True if empty, false if not.*/
	bool isEmpty() const;

	/** Clear the stored file path of the current API.
	*	See also DeviceAPIParser::clear().*/
	void clear();

	/** Watch API file for changes and apply them immediately.
	*	@todo We should ask the user first (from the proxy? how? from the client? which one?...*/
	void startWatch();

	/** Stop watching the API file for changes.*/
	void stopWatch();

public slots:

	/** Reload the API definition file.
	 *	@return True on success, false otherwise.*/
	bool reload();

private:
	QString mCurrentAPIFilePath;			///< Absolute path of the currently used deviceAPI
	QFileSystemWatcher *mApiFileWatcher;	///< FileSystemWatcher object for deviceAPI

};

}	//QtuC::
#endif //DEVICEAPIFILEHANDLER_H

