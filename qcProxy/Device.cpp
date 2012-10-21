#include "Device.h"
#include <QDebug>

using namespace QtuC;

Device *Device::mInstance = 0;
bool Device::mCreated = false;
QStringList Device::mHardwareInterfaces = QStringList();
QHash<QString,QString> Device::mHardwareInterfaceInfo = QHash<QString,QString>();
QList<QStringList> Device::mFunctions = QList<QStringList>();
bool Device::mConnected = false;
bool Device::mPositiveAck = false;
QHash<QString,QString> Device::mInfo = QHash<QString,QString>();

Device( QObject *parent = 0 ) : ErrorHandlerBase( parent )
{
	mInfo.insert( "name", QString() );
	mInfo.insert( "description", QString() );
	mInfo.insert( "platform", QString() );
	mInfo.insert( "project", QString() );
}

Device::Device( const QStringList& hwInterfaceList, const QStringList& hwInterfaceInfoList, const QString & deviceName, QObject *parent ) : ErrorHandlerBase( parent )
{
	mHardwareInterfaces = QStringList( hwInterfaceList );
	mHardwareInterfaceInfo = QStringList( hwInterfaceInfoList );

	mInfo.insert( "name", deviceName );
	mInfo.insert( "description", QString() );
	mInfo.insert( "platform", QString() );
	mInfo.insert( "project", QString() );
}

Device* Device::instance( QObject *parent )
{
	if( mCreated )
		{ return 0; }
	else
	{
		if( !mInstance )
			{ mInstance = new Device(parent); }
		return mInstance;
	}
}

Device* Device::swap(Device *newDevice)
{
	Device* oldInstance = mInstance;
	mInstance = newDevice;
	return oldInstance;
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

bool Device::create( const QStringList& hwInterfaceList, const QStringList& hwInterfaceInfoList, const QString & deviceName, QObject *parent )
{
	if( hwInterfaceList.isEmpty() )
	{
		ErrorHandlerBase::error( QtCriticalMsg, "Cannot create Device with no hardware interface!", "Device::create()" );
		return false;
	}

	mInstance = new Device( hwInterfaceList, hwInterfaceInfoList, deviceName, parent);
	return true;
}
