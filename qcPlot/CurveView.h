#ifndef CURVEVIEW_H
#define CURVEVIEW_H

#include <qwt_plot_curve.h>
#include "CurveConfig.h"

namespace qcPlot
{

class CurveView : public QwtPlotCurve
{
public:
	CurveView(QwtPlot *parent = 0);
	
signals:
	
public slots:

private:
	void setConfig( CurveConfig *config );

};

}	//qcPlot::
#endif // CURVEVIEW_H
