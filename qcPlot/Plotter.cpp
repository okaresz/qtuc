#include "Plotter.h"

using namespace qcPlot;

Plotter::Plotter( PlotConfig *config, QObject *parent ) :
	QObject(parent)
{
	mConfig.copy( *config );
	config->deleteLater();
}
