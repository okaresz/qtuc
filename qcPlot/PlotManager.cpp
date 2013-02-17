#include "PlotManager.h"

using namespace qcPlot;

PlotManager::PlotManager(QObject *parent) :
	QObject(parent)
{
}

bool PlotManager::addNewPlot(PlotConfig *plotCfg)
{
	if( !plotCfg )
		{ return false; }

	Plotter *p = new Plotter( plotCfg, this );
	mPlotters.append( p );
	emit newPlotterAdded( p );
	return true;
}
