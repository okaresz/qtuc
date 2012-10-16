#include "DeviceCommandBase.h"

using namespace QtuC;

DeviceCommandBase::DeviceCommandBase()
{

}

DeviceCommandBase::DeviceCommandBase( Device::commandType cmdType, DeviceStateVariable* stateVar, bool convert )
{

}

DeviceCommandBase::DeviceCommandBase( DeviceCommandBase* deviceCommand )
{

}

DeviceCommandBase::DeviceCommandBase( DeviceCommandBase deviceCommand )
{

}

bool DeviceCommandBase::isValid()
{

}

void DeviceCommandBase::getType() const
{

}

QString DeviceCommandBase::getInterface()
{

}

QString DeviceCommandBase::getVar()
{

}

QString DeviceCommandBase::getArg()
{

}

void DeviceCommandBase::setType( Device::commandType t )
{

}

void DeviceCommandBase::setInterface( const QString & hwi )
{

}

void DeviceCommandBase::setVar( const QString& cv )
{

}

void DeviceCommandBase::setArg( const QString& ca )
{

}

bool DeviceCommandBase::checkArgCount()
{

}

