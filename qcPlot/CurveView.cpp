#include "CurveView.h"

using namespace qcPlot;

CurveView::CurveView(QwtPlot *parent) :
	QwtPlotCurve()
{
	attach( parent );
}
