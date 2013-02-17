#ifndef PROXYSTATEMANAGER_H
#define PROXYSTATEMANAGER_H

#include "StateManagerBase.h"

namespace qcPlot
{

class ProxyStateManager : public QtuC::StateManagerBase
{
	Q_OBJECT
public:
	explicit ProxyStateManager(QObject *parent = 0);
	
signals:
	
public slots:
	
};

}	//QcPlot::
#endif // PROXYSTATEMANAGER_H
