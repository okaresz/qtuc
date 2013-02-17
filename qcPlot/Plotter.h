#ifndef PLOTTER_H
#define PLOTTER_H

#include <QObject>
#include "PlotConfig.h"

namespace qcPlot
{

class Plotter : public QObject
{
	Q_OBJECT
public:
	/// Takes ownership of the config
	explicit Plotter( PlotConfig *config, QObject *parent = 0 );

	PlotConfig const * cfg() const
		{ return &mConfig; }
	
signals:
	
public slots:

private:
	PlotConfig mConfig;
	
};

}	//qcPlot::
#endif // PLOTTER_H
