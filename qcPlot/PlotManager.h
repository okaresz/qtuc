#ifndef PLOTMANAGER_H
#define PLOTMANAGER_H

#include <QObject>
#include "PlotConfig.h"
#include "Plotter.h"
#include <QList>

namespace qcPlot
{

class PlotManager : public QObject
{
	Q_OBJECT
public:
	explicit PlotManager(QObject *parent = 0);

	bool addNewPlot( PlotConfig *plotCfg );

	QDomElement getXml() const;
	bool loadXml( QDomElement const &layoutCfgElement );

	uint getPlotterCount() const
		{ return mPlotters.size(); }
	
signals:
	void newPlotterAdded( Plotter* );
	
public slots:

private:
	QList<Plotter*> mPlotters;
	
};

}	//qcPlot::
#endif // PLOTMANAGER_H
