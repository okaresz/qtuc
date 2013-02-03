#include "SettingsManagerBase.h"
#include <QDir>
#include <QDomDocument>
#include <QTextStream>
#include "ErrorHandlerBase.h"

using namespace QtuC;

SettingsManagerBase* SettingsManagerBase::instancePtr = 0;

SettingsManagerBase::SettingsManagerBase(QObject *parent) : QSettings(parent)
{
	ErrorHandlerBase::debug( debugLevelVerbose, QString("Settings are loaded from: %1").arg(fileName()), "SettingsManagerBase()", "SettingsManagerBase" );
}

SettingsManagerBase::~SettingsManagerBase()
{}

SettingsManagerBase *SettingsManagerBase::instance(QObject *parent)
{
	ErrorHandlerBase::error( QtCriticalMsg, "no derived settings object! you must reimplement SettingsManagerBase::instance() to return a derived settings instance.", "instance()", "SettingsManagerBase" );
	return 0;
}
