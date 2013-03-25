#ifndef PROXYSETTINGSMANAGER_H
#define PROXYSETTINGSMANAGER_H

#include "SettingsManagerBase.h"
#include <QHash>
#include <QVariant>

namespace QtuC
{

/** Derived class to manage proxy settings.*/
class ProxySettingsManager : public SettingsManagerBase
{
	Q_OBJECT
public:

	typedef enum {
		cmdArgPassthrough = 0,
		cmdArgVerbose
	} cmdArg_t;

	explicit ProxySettingsManager(QObject *parent = 0);

	static ProxySettingsManager *instance(QObject *parent = 0);

	QVariant const getCmdArgValue( cmdArg_t arg );

private:
	void initCmdParser();
	void cmdSwitchFound( const QString & name );
	void cmdOptionFound( const QString & name, const QVariant & value );
	void cmdParamFound( const QString & name, const QVariant & value );
	void cmdParseError( const QString & error );

	QHash<cmdArg_t,QVariant> mCmdArgs;
	static QString mCmdArgNames[];
};

}	//QtuC::
#endif // PROXYSETTINGSMANAGER_H
