#ifndef PLOTTERVIEW_H
#define PLOTTERVIEW_H

#include <QWidget>
#include "Plotter.h"
#include "PlotView.h"

namespace qcPlot
{

class PlotterView : public QWidget
{
	Q_OBJECT
public:
	explicit PlotterView( Plotter *model, QWidget *parent = 0);
	
signals:
	
private slots:
	void on_plotterView_toolBar_clearButton_clicked();
	void on_plotterView_toolBar_saveButton_clicked();
	void on_plotterView_toolBar_startPauseButton_toggled( bool checked );
	void on_plotterView_toolBar_autoZoomButton_toggled( bool checked );
	void on_plotterView_toolBar_autoFollowButton_toggled( bool checked );

	void onModelReset();
	
private:
	void createGui();
	void initModelView();

	PlotView *mPlot;
	Plotter *mModel;
};

}	//qcPlot::
#endif // PLOTTERVIEW_H
