#ifndef PLOTCONFIG_H
#define PLOTCONFIG_H

#include <QObject>
#include "CurveConfig.h"
#include <QList>
#include <QString>

namespace qcPlot
{

class PlotConfig : public QObject
{
	Q_OBJECT
public:
	explicit PlotConfig(QObject *parent = 0);

	/// Parent is not copied!
	PlotConfig( PlotConfig const &other, QObject *parent = 0 );

	bool copy( PlotConfig const &other );

	inline QString const name() const
		{ return mName; }

	void setName( QString const &name )
	{
		mName = name;
	}

	CurveConfig *createCurve( QString const &name = QString() );

	void deleteCurve( uint curveId );

	CurveConfig *getCurve( uint curveId ) const;

	QList<CurveConfig*> getCurves() const
		{ return mCurves; }

	QDomElement getXml() const;
	bool loadXml( QDomElement const &plotCfgElement );

private:

	QString mName;
	QList<CurveConfig*> mCurves;

};

}	//qcPlot::
#endif // PLOTCONFIG_H
