#ifndef PLOTVIEW_H
#define PLOTVIEW_H

#include <qwt_plot.h>
#include <QList>
#include "PlotConfig.h"
#include "Plotter.h"
#include "qwt_plot_curve.h"
#include <QList>
#include <QPair>

namespace qcPlot
{

class PlotView : public QwtPlot
{
	Q_OBJECT
public:
	PlotView( Plotter *model, QWidget *parent );

	bool getAutoZoom() const
		{ return mAutoZoom; }

	bool getAutoFollow() const
		{ return mAutoFollow; }

public slots:
	void setConfig( const PlotConfig *config );

	void setAutoZoom( bool zoom )
		{ mAutoZoom = zoom; }

	void setAutoFollow( bool follow )
		{ mAutoFollow = follow; }

protected slots:
	void onDataChanged();

private:
	QwtPlotCurve *curve( uint curveId );

	QList<QPair<uint,QwtPlotCurve*> > mCurves;
	Plotter *mModel;
	bool mAutoZoom;
	bool mAutoFollow;
};

}	//qcPlot::
#endif // PLOTVIEW_H
