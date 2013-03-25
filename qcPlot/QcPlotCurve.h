#ifndef QCPLOTCURVE_H
#define QCPLOTCURVE_H

#include "qwt_plot_curve.h"

namespace qcPlot
{

class QcPlotCurve : public QwtPlotCurve
{
public:
	QcPlotCurve();
	~QcPlotCurve();
};

}	//qcPlot::
#endif // QCPLOTCURVE_H
