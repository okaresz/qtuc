#ifndef PROXYSETTINGSMANAGER_H
#define PROXYSETTINGSMANAGER_H

#include "SettingsManagerBase.h"

namespace QtuC
{

class ProxySettingsManager : public SettingsManagerBase
{
	Q_OBJECT
public:
	explicit ProxySettingsManager(QObject *parent = 0);
	
signals:
	
public slots:
	
};

}	//QtuC::
#endif // PROXYSETTINGSMANAGER_H
