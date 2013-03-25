#ifndef DEVICESTATEPLOTDATAVARIABLE_H
#define DEVICESTATEPLOTDATAVARIABLE_H

#include "DeviceStateHistoryVariable.h"
#include <qwt_series_data.h>

namespace qcPlot
{

/** Class to provide data from a history variable to a QwtPlotCurve<T>.
*	By deriving from QwtSeriesData, QwtPlotSeriesItem<T> (and ultimately QwtPlotCurve) can use the data object instance without copying the data.*/
class DeviceStatePlotDataVariable : public DeviceStateHistoryVariable, public QwtSeriesData<QPointF>
{
	Q_OBJECT
public:
	explicit DeviceStatePlotDataVariable( const QString& varHwInterface, const QString& varName, const QString& varType, const QString &accessModeStr );

	~DeviceStatePlotDataVariable(){}

	/** Inherited from QwtSeriesData.
	  *	@{*/
	QRectF boundingRect() const;
	QPointF sample(size_t i) const;
	size_t size() const
		{ return mHistory.size(); }
	/// @}

private:

};

}	//qcPlot::
#endif // DEVICESTATEPLOTDATAVARIABLE_H
