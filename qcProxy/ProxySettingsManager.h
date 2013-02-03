#ifndef PROXYSETTINGSMANAGER_H
#define PROXYSETTINGSMANAGER_H

#include "SettingsManagerBase.h"

namespace QtuC
{

/** Derived class to manage proxy settings.*/
class ProxySettingsManager : public SettingsManagerBase
{
	Q_OBJECT
public:

	explicit ProxySettingsManager(QObject *parent = 0);

	static ProxySettingsManager *instance(QObject *parent = 0);
	
};

}	//QtuC::
#endif // PROXYSETTINGSMANAGER_H
