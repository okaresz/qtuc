#ifndef DEVICEAPIFILEHANDLER_H
#define DEVICEAPIFILEHANDLER_H

#include "DeviceAPIParser.h"

namespace QtuC
{

/** DeviceAPIFileHandler class.
 *	Handles the device API definition file: load, save, watch for change and update the file if needed.*/
class DeviceAPIFileHandler : public DeviceAPIParser
{
	Q_OBJECT
public:
	/** C'tor*/
	DeviceAPIFileHandler( QObject *parent );

	/** Load the API definition file.
	 *	Path and file name comes from settings.
	 *	@return True on success, false otherwise.*/
	bool load();

	/** Save the API definition file.
	 *	@return True on success, false otherwise.*/
	bool save();

	/** Reload the API definition file.
	 *	@return True on success, false otherwise.*/
	bool reload();

	/** Watch API file for changes and apply them immediately.*/
	void startWatch();

	/** Stop watching the API file for changes.*/
	void stopWatch();

};

}	//QtuC::
#endif //DEVICEAPIFILEHANDLER_H

