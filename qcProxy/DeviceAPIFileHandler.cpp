#include "DeviceAPIFileHandler.h"
#include "ProxySettingsManager.h"
#include <QDir>
#include <QFile>
#include <QTextStream>

using namespace QtuC;

DeviceAPIFileHandler::DeviceAPIFileHandler( QObject *parent ) : DeviceAPIParser( parent )
{
	mApiFileWatcher = new QFileSystemWatcher(this);
}

bool DeviceAPIFileHandler::load(const QString &apiFilePath)
{
	if( apiFilePath.isEmpty() )
	{
		QString apiFileDir = ProxySettingsManager::instance()->value( "deviceAPIFile/dirPath" );
		QString apiFileName = ProxySettingsManager::instance()->value( "deviceAPIFile/fileName" );

		if( apiFileDir.isEmpty() )
			{ apiFileDir = QDir::current().absolutePath() + "/"; }

		apiFilePath = QDir::cleanPath( apiFileDir.append(apiFileName) );
	}

	QFileInfo apiFileInfo(apiFilePath);
	if( !(apiFileInfo.isFile() && apiFileInfo.isReadable()) )
	{
		error( QtCriticalMsg, QString("Device API file doesn't exist at path %1").arg(apiFilePath), "load()", errDet );
		return false;
	}

	QFile apiFile(apiFileInfo.filePath());
	if(!apiFile.open(QIODevice::ReadOnly))
	{
		error( QtCriticalMsg, QString("Failed to open device API file at %1").arg(apiFilePath), "load()" );
		return false;
	}

	QTextStream apiFileStream(&apiFile);
	if( !parseAPI( apiFileStream.readAll() ) )
	{
		error( QtCriticalMsg, QString("Failed to parse device API file at %1").arg(apiFilePath), "load()" );
		return false;
	}
	apiFile.close();
	mCurrentAPIFilePath = apiFilePath;

	debug( debugLevelInfo, "Device API loaded", "load()" );
	return true;
}

bool DeviceAPIFileHandler::save(const QString &apiFilePath)
{
	if( apiFilePath.isEmpty() )
	{
		QString apiFileDir = ProxySettingsManager::instance()->value( "deviceAPIFile/dirPath" );
		QString apiFileName = ProxySettingsManager::instance()->value( "deviceAPIFile/fileName" );

		if( apiFileDir.isEmpty() )
			{ apiFileDir = QDir::current().absolutePath() + "/"; }

		apiFilePath = QDir::cleanPath( apiFileDir.append(apiFileName) );
	}
	/// @todo Implement
}

bool DeviceAPIFileHandler::reload()
{
	if( load( mCurrentAPIFilePath ) )
	{
		debug( debugLevelInfo, "Device API reloaded", "reload()" );
		return true;
	}
	else
	{
		error( QtCriticalMsg, "Failed to reload device API", "reload()" );
		return false;
	}
}

void DeviceAPIFileHandler::startWatch()
{
	mApiFileWatcher->removePaths( mApiFileWatcher->files() );
	mApiFileWatcher->addPath(mCurrentAPIFilePath);
	connect( mApiFileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(reload()) );
	debug( debugLevelVerbose, QString("Start to watch deviceAPI for changes at %1").arg(mCurrentAPIFilePath), "startWatch()" );
}

void DeviceAPIFileHandler::stopWatch()
{
	mApiFileWatcher->disconnect();
	debug( debugLevelVerbose, QString("Stopped watching of deviceAPI for changes at %1").arg(mApiFileWatcher->files().at(0)), "stopWatch()" );
}

