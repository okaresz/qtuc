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

QDomElement PlotManager::getXml() const
{
	QDomDocument dom;
	QDomElement layoutEl = dom.createElement( "layout" );
	for( unsigned short int i=0; i<mPlotters.size(); ++i )
	{
		layoutEl.appendChild( mPlotters.at(i)->cfg()->getXml() );
	}
	return layoutEl;
}

bool PlotManager::loadXml(const QDomElement &layoutCfgElement)
{
	QDomElement plotCfgEl = layoutCfgElement.firstChildElement( "plot" );
	while( !plotCfgEl.isNull() )
	{
		PlotConfig *cfg = new PlotConfig();
		if( !cfg->loadXml( plotCfgEl ) )
			{ return false; }
		else
			{ addNewPlot( cfg ); }
		plotCfgEl = plotCfgEl.nextSiblingElement( "plot" );
	}
	return true;
}
