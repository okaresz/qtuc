#include "SettingsManagerBase.h"
#include <QDir>
#include <QDomDocument>
#include <QTextStream>
#include "ErrorHandlerBase.h"

using namespace QtuC;

SettingsManagerBase* SettingsManagerBase::instancePtr = 0;

SettingsManagerBase::SettingsManagerBase(QObject *parent) : QSettings(parent)
{
	mCmdParser = new QCommandLine(this);
	mCmdParser->enableHelp(true);
	mCmdParser->enableVersion(true);

	connect( mCmdParser, SIGNAL(switchFound(const QString &)), this, SLOT(cmdSwitchFound(const QString &)) );
	connect( mCmdParser, SIGNAL(optionFound(const QString &, const QVariant &)), this, SLOT(cmdOptionFound(const QString &, const QVariant &)) );
	connect( mCmdParser, SIGNAL(paramFound(const QString &, const QVariant &)), this, SLOT(cmdParamFound(const QString &, const QVariant &)) );
	connect( mCmdParser, SIGNAL(parseError(const QString &)), this, SLOT(cmdParseError(const QString &)) );

	ErrorHandlerBase::debug( debugLevelVerbose, QString("Settings are loaded from: %1").arg(fileName()), "SettingsManagerBase()", "SettingsManagerBase" );
}

SettingsManagerBase::~SettingsManagerBase()
{}

SettingsManagerBase *SettingsManagerBase::instance(QObject *parent)
{
	Q_UNUSED(parent);
	ErrorHandlerBase::error( QtCriticalMsg, "no derived settings object! you must reimplement SettingsManagerBase::instance() to return a derived settings instance.", "instance()", "SettingsManagerBase" );
	return 0;
}

bool SettingsManagerBase::parseCmdArgs(const QStringList &argList)
{
	mCmdParser->setArguments( argList );
	return mCmdParser->parse();
}

void SettingsManagerBase::showCmdHelp()
{
	mCmdParser->showHelp(false);
}

void SettingsManagerBase::cmdSwitchFound(const QString &name)
{
	Q_UNUSED(name);
	ErrorHandlerBase::error( QtWarningMsg, "Command line switch found, but SettingsManagerBase::cmdSwitchFound() is not re-implemented.", "cmdSwitchFound()", "SettingsManagerBase" );
}

void SettingsManagerBase::cmdOptionFound(const QString &name, const QVariant &value)
{
	Q_UNUSED(name);
	Q_UNUSED(value);
	ErrorHandlerBase::error( QtWarningMsg, "Command line option found, but SettingsManagerBase::cmdOptionFound() is not re-implemented.", "cmdOptionFound()", "SettingsManagerBase" );
}

void SettingsManagerBase::cmdParamFound(const QString &name, const QVariant &value)
{
	Q_UNUSED(name);
	Q_UNUSED(value);
	ErrorHandlerBase::error( QtWarningMsg, "Command line param found, but SettingsManagerBase::cmdParamFound() is not re-implemented.", "cmdParamFound()", "SettingsManagerBase" );
}

void SettingsManagerBase::cmdParseError(const QString &error)
{
	Q_UNUSED(error);
	ErrorHandlerBase::error( QtWarningMsg, "Command line error found, but SettingsManagerBase::cmdParseError() is not re-implemented.", "cmdParseError()", "SettingsManagerBase" );
}
