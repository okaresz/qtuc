#ifndef PROXYSTATEMANAGER_H
#define PROXYSTATEMANAGER_H

#include "StateManagerBase.h"

namespace QtuC
{

class ProxyStateManager : public StateManagerBase
{
	Q_OBJECT
public:
	explicit ProxyStateManager(QObject *parent = 0);
	
signals:
	
public slots:
	
};

}	//QtuC::
#endif // PROXYSTATEMANAGER_H
