#ifndef SETTINGSMANAGERBASE_H
#define SETTINGSMANAGERBASE_H

#include <QSettings>
#include "QCommandLine.h"
#include <QStringList>

namespace QtuC
{

/** Base for settings manager classes.
*	Manage file-based settings and command line parameters.
*	The location of the configuration file is printed to stdout when initialized.
*	This class is essentially a singleton wrapper above QSettings.
*	As QSettings is initialized without parameters, according to the Qt manual, you must specify application-related information before using QSettings.*/
class SettingsManagerBase : public QSettings
{
	Q_OBJECT
public:
	explicit SettingsManagerBase(QObject *parent = 0);

	~SettingsManagerBase();

	/** Every settings class must reimplement this function, returning an instance of itself (the derived class).
	 *	@param parent On first call, the object will be created, so a parent argument should  be passed. On subsequent calls, you can omit it.*/
	static SettingsManagerBase* instance(QObject *parent = 0);

	bool parseCmdArgs( QStringList const &argList );

	void showCmdHelp();

protected slots:

	virtual void cmdSwitchFound( const QString & name );
	virtual void cmdOptionFound( const QString & name, const QVariant & value );
	virtual void cmdParamFound( const QString & name, const QVariant & value );
	virtual void cmdParseError( const QString & error );

protected:
	static SettingsManagerBase *instancePtr;

	virtual void initCmdParser() = 0;

	QCommandLine *mCmdParser;
};

}	//QtuC::
#endif // SETTINGSMANAGERBASE_H
