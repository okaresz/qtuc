#include "DeviceStatePlotDataVariable.h"

using namespace qcPlot;

DeviceStatePlotDataVariable::DeviceStatePlotDataVariable( const QString &varHwInterface, const QString &varName, const QString &varType, const QString &accessModeStr ) :
	DeviceStateHistoryVariable( varHwInterface, varName, varType, accessModeStr ),
	QwtSeriesData<QPointF>()
{
}

QRectF DeviceStatePlotDataVariable::boundingRect() const
{
	return QRectF( QPointF( toDeviceTime(mTimestampLimits.first), mValueLimits.second ), QPointF( toDeviceTime(mTimestampLimits.second), mValueLimits.first ) );
}

QPointF DeviceStatePlotDataVariable::sample( size_t i ) const
{
	return QPointF( (qreal)toDeviceTime(mHistory.at(i).first), mHistory.at(i).second );
}
