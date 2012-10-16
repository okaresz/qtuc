#include "SettingsManagerBase.h"
#include <QDir>
#include <QDomDocument>
#include <QTextStream>

using namespace QtuC;

SettingsManagerBase* SettingsManagerBase::instancePtr = 0;

SettingsManagerBase::SettingsManagerBase(QObject *parent) : QSettings(parent)
{

}

SettingsManagerBase* SettingsManagerBase::instance(QObject *parent)
{
	if( !instancePtr )
	{
		instancePtr = new SettingsManagerBase(parent);
	}
	return instancePtr;
}

