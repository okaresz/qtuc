#include "QcPlotCurve.h"

using namespace qcPlot;

QcPlotCurve::QcPlotCurve() : QwtPlotCurve()
{}

QcPlotCurve::~QcPlotCurve()
{
	/* This is a hack.
	QwtPlotSeriesItem<T>::~QwtPlotSeriesItem() will delete d_series.
	But d_series holds a DeviceStatePlotDataVariable, which is a child of the statemanager, and is deleted by it.
	Moreover, if two curves share the same variable, they both will want to delete the data, leading to a double free error.
	By Assigning a dummy data object to d_series, the DeviceStatePlotDataVariable stays alive, and will be deleted by the stateManager.*/
	d_series = new QwtPointSeriesData();
}
