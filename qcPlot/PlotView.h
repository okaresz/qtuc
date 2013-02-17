#ifndef PLOTVIEW_H
#define PLOTVIEW_H

#include <qwt_plot.h>
#include <QList>
#include "PlotConfig.h"
#include "Plotter.h"

namespace qcPlot
{

class PlotView : public QwtPlot
{
	Q_OBJECT
public:
	PlotView( Plotter *model, QWidget *parent = 0 );

protected slots:
	void onDataChanged();

private:
	void setConfig( PlotConfig *config );

	Plotter *mModel;

};

}	//qcPlot::
#endif // PLOTVIEW_H
