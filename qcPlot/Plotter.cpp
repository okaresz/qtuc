#include "Plotter.h"
#include "ProxyStateManager.h"

using namespace qcPlot;

Plotter::Plotter( PlotConfig *config, QObject *parent ) :
	QObject(parent),
	mConfig(config)
{
	mConfig->setParent(this);
}

DeviceStatePlotDataVariable *Plotter::resolveStateVar(const QString &hwi, const QString &name)
{
	return (DeviceStatePlotDataVariable*)(ProxyStateManager::instance()->getVar( hwi, name ));
}

void Plotter::setEnabled( bool enabled )
{
	QList<CurveConfig*> curves = mConfig->getCurves();
	unsigned int i = curves.size();
	while( i-- )
	{
		DeviceStatePlotDataVariable *var = resolveStateVar(curves.at(i)->stateVariable());
		if( var )
			{ var->setHistoryLog( enabled ); }
	}
}

void Plotter::reset()
{
	clear();
	emit hadReset();
}

void Plotter::clear()
{
	QList<CurveConfig*> curves = mConfig->getCurves();
	unsigned int i = curves.size();
	while( i-- )
	{
		DeviceStatePlotDataVariable *var = resolveStateVar(curves.at(i)->stateVariable());
		if( var )
			{ var->clearHistory(); }
	}
}
