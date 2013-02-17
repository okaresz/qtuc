#ifndef PLOTMANAGERVIEW_H
#define PLOTMANAGERVIEW_H

#include <QWidget>
#include "Plotter.h"
#include "PlotManager.h"

namespace qcPlot
{

class PlotManagerView : public QWidget
{
	Q_OBJECT
public:
	explicit PlotManagerView( PlotManager *model, QWidget *parent = 0);
	
public slots:
	void onNewPlotterAdded( Plotter* plotter );
	
public slots:
	
private:
	void createGui();

	PlotManager *mModel;
};

}	//qcPlot::
#endif // PLOTMANAGERVIEW_H
