#include "PlotConfig.h"

using namespace qcPlot;

PlotConfig::PlotConfig(QObject *parent) :
	QObject(parent)
{
	mName = "New plot";
}

PlotConfig::PlotConfig(const PlotConfig &other, QObject *parent) :
	QObject(parent)
{
	copy(other);
}

bool PlotConfig::copy(const PlotConfig &other)
{
	mName = other.mName;
	int i = other.mCurves.size();
	while( i-- )
		{ mCurves.append( new CurveConfig( *(other.mCurves.at(i)), this ) ); }
	return true;
}

CurveConfig *PlotConfig::createCurve(const QString &name)
{
	CurveConfig *curve = new CurveConfig(this);

	if( name.isEmpty() )
		{ curve->setName( QString("curve%1").arg( QString::number(curve->id()) ) ); }
	else
		{ curve->setName( name ); }

	mCurves.append( curve );
	return curve;
}

void PlotConfig::deleteCurve(uint curveId)
{
	CurveConfig *curveCfg = getCurve( curveId );
	if( curveCfg )
	{
		mCurves.removeOne( curveCfg );
		curveCfg->deleteLater();
	}
}

CurveConfig *PlotConfig::getCurve(uint curveId) const
{
	int i = mCurves.size();
	while( i-- )
	{
		if( mCurves.at(i)->id() == curveId )
			{ return mCurves.at(i); }
	}
	return 0;
}

QDomElement PlotConfig::getXml() const
{
	QDomDocument dom;
	QDomElement plotEl = dom.createElement( "plot" );
	plotEl.setAttribute( "name", mName );
	for( unsigned short int i=0; i < mCurves.size(); ++i )
	{
		plotEl.appendChild( mCurves.at(i)->getXml() );
	}
	return plotEl;
}

bool PlotConfig::loadXml(const QDomElement &plotCfgElement)
{
	mName = plotCfgElement.attribute( "name" );
	QDomElement curveCfgEl = plotCfgElement.firstChildElement( "curve" );
	while( !curveCfgEl.isNull() )
	{
		CurveConfig *curve = new CurveConfig(this);
		if( !curve->loadXml( curveCfgEl ) )
			{ curve->deleteLater(); }
		else
			{ mCurves.append( curve ); }
		curveCfgEl = curveCfgEl.nextSiblingElement( "curve" );
	}
	return true;
}
