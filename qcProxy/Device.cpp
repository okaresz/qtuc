#include "Device.h"
#include <QDebug>

using namespace QtuC;

Device *Device::mInstance = 0;
QStringList Device::mHardwareInterfaces = QStringList();
QHash<QString,QString> Device::mHardwareInterfaceInfo = QHash<QString,QString>();
QList<QStringList> Device::mFunctions = QList<QStringList>();
bool Device::mPositiveAck = false;
QHash<QString,QString> Device::mInfo = QHash<QString,QString>();

Device::Device( QObject *parent = 0 ) : ErrorHandlerBase( parent )
{
	clear();
	mInfo.insert( "name", QString() );
	mInfo.insert( "description", QString() );
	mInfo.insert( "platform", QString() );
	mInfo.insert( "project", QString() );

	mHardwareInterfaces.append( "!proxy" );
}

Device::Device( const QStringList& hwInterfaceList, const QStringList& hwInterfaceInfoList, const QString & deviceName, QObject *parent ) : ErrorHandlerBase( parent )
{
	clear();
	mHardwareInterfaces = QStringList( hwInterfaceList );
	mHardwareInterfaceInfo = QStringList( hwInterfaceInfoList );

	mInfo.insert( "name", deviceName );
	mInfo.insert( "description", QString() );
	mInfo.insert( "platform", QString() );
	mInfo.insert( "project", QString() );

	mHardwareInterfaces.append( "!proxy" );
}

Device* Device::instance( QObject *parent )
{
	if( mCreated )
		{ return 0; }
	else
	{
		error( QtWarningMsg, "Try to access Device instance, but it hasn't been created yet, use create()", "instance()" );
		return mInstance;
	}
}

Device* Device::swap(Device *newDevice)
{
	Device* oldInstance = mInstance;
	mInstance = newDevice;
	return oldInstance;
}

Device *Device::create(QObject *parent)
{
	mInstance = new Device(parent);
	return mInstance;
}

bool Device::isValidHwInterface( const QString& hwInterfaceName ) const
{
	return mHardwareInterfaces.contains( hwInterfaceName );
}

const QString Device::getHwInterfaceInfo( const QString& name ) const
{
	return mHardwareInterfaceInfo.value( name, QString() );
}

const QString Device::getInfo(const QString &key) const
{
	return mInfo.value( key, QString() );
}

const QString Device::getDescription() const
{
	return mInfo.value( "description" );
}

const QString Device::getPlatform() const
{
	return mInfo.value( "platform" );
}

const QString Device::getProject() const
{
	return mInfo.value( "project" );
}

void Device::clear()
{
	this->disconnect();
	mHardwareInterfaces.clear();
	mHardwareInterfaceInfo.clear();
	mFunctions.clear();
	mInfo.clear();
	mPositiveAck = false;
	mCreated = false;
}

deviceMessageType_t Device::messageTypeFromString(const QString &msgTypeStr)
{
	if( msgTypeStr == "deviceMsgInfo" ) { return deviceMsgInfo; }
	if( msgTypeStr == "deviceMsgDebug" ) { return deviceMsgDebug; }
	if( msgTypeStr == "deviceMsgError" ) { return deviceMsgError; }
	return deviceMsgUndefined;
}

const QString Device::messageTypeToString(deviceMessageType_t msgType)
{
	switch( msgType )
	{
		case deviceMsgInfo: return QString("deviceMsgInfo");
		case deviceMsgDebug: return QString("deviceMsgDebug");
		case deviceMsgError: return QString("deviceMsgError");
		default: return QString("deviceMsgUndefined");
	}
}

const QString Device::getName() const
{
	return mInfo.value( "name" );
}


void Device::addHardwareInterface(const QString &hwInterfaceName)
{
	if( !mCreated )
	{ mHardwareInterfaces.append( hwInterfaceName ); }
}

void Device::addFunction(const QString &hwInterface, const QString &name, const QString &args)
{
	QStringList funcDef;
	funcDef.append(hwInterface);
	funcDef.append(name);
	funcDef.append(args);
	mFunctions.append(funcDef);
}

void Device::addHardwareInterfaceInfo(const QString &hwInterfaceName, const QString &hwInterfaceInfo)
{
	if( !mCreated )
		{ mHardwareInterfaceInfo.insert( hwInterfaceName, hwInterfaceInfo ); }
}

void Device::setName( const QString & deviceName )
{
	if( !mCreated )
		{ mInfo.insert( "name", deviceName ); }
}

void Device::setDescription( const QString & deviceDesc )
{
	if( !mCreated )
		{ mInfo.insert( "description", deviceDesc ); }
}

void Device::setPlatform(const QString &platform)
{
	if( !mCreated )
		{ mInfo.insert( "platform", platform ); }
}

void Device::setProject(const QString &project)
{
	if( !mCreated )
		{ mInfo.insert( "project", project ); }
}

void Device::setPositiveAck(bool posAck)
{
	if( !mCreated )
		{ mPositiveAck = posAck; }
}

Device *Device::create( const QStringList& hwInterfaceList, const QStringList& hwInterfaceInfoList, const QString & deviceName, QObject *parent )
{
	if( hwInterfaceList.isEmpty() )
	{
		ErrorHandlerBase::error( QtCriticalMsg, "Cannot create Device with no hardware interface!", "Device::create()" );
		return false;
	}

	mInstance = new Device( hwInterfaceList, hwInterfaceInfoList, deviceName, parent);
	return mInstance;
}
