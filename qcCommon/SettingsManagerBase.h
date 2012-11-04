#ifndef SETTINGSMANAGERBASE_H
#define SETTINGSMANAGERBASE_H

#include <QSettings>

namespace QtuC
{

/** Base for settings manager classes.
*	This class is essentially a singleton wrapper above QSettings.
*	As QSettings is initialized without parameters, according to the Qt manual, you must specify application-related information before using QSettings.*/
class SettingsManagerBase : public QSettings
{
	Q_OBJECT
public:
	explicit SettingsManagerBase(QObject *parent = 0);

	~SettingsManagerBase();

	/** Every settings class must reimplement this function, returning an instance of itself (the derived class).
	 *	@param On first call, the object will be created, so a parent argument should  be passed. On subsequent calls, you can omit it.*/
	static SettingsManagerBase* instance(QObject *parent = 0);

protected:
	static SettingsManagerBase *instancePtr;

	//bool readXML(QIODevice &device, QSettings::SettingsMap &map);
	//bool writeXML(QIODevice &device, const QSettings::SettingsMap &map);
};

}	//QtuC::
#endif // SETTINGSMANAGERBASE_H
