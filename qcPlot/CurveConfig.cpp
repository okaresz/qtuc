#include "CurveConfig.h"

using namespace qcPlot;

uint CurveConfig::mCurveIdCounter = 0;

CurveConfig::CurveConfig(QObject *parent) :
	QObject(parent),
	mId(mCurveIdCounter++)
{
}

CurveConfig::CurveConfig(const CurveConfig &other, QObject *parent) :
	QObject(parent)
{
	copy(other);
}

bool CurveConfig::copy(const CurveConfig &other)
{
	mId = other.mId;
	mName = other.mName;
	mStateVariable = other.mStateVariable;
	return true;
}

void CurveConfig::setStateVariable(const QString &hwi, const QString &name)
{
	mStateVariable.first = hwi;
	mStateVariable.second = name;
}

QDomElement CurveConfig::getXml() const
{
	QDomDocument dom;
	QDomElement curveEl = dom.createElement( "curve" );
	curveEl.setAttribute( "name", mName );
	QDomElement varEl = dom.createElement( "stateVar" );
	varEl.setAttribute( "hwi", mStateVariable.first );
	varEl.setAttribute( "name", mStateVariable.second );
	curveEl.appendChild( varEl );
	return curveEl;
}

bool CurveConfig::loadXml(const QDomElement &curveCfgElement)
{
	if( curveCfgElement.attribute( "name" ).isEmpty() )
		{ return false; }
	mName = curveCfgElement.attribute( "name" );
	if( !curveCfgElement.firstChildElement("stateVar").isNull() )
	{
		QDomElement stateVarEl = curveCfgElement.firstChildElement("stateVar");
		if( stateVarEl.attribute("hwi").isEmpty() || stateVarEl.attribute("name").isEmpty() )
			{ return false; }
		mStateVariable.first = stateVarEl.attribute("hwi");
		mStateVariable.second = stateVarEl.attribute("name");
	}
	else
		{ return false; }
	return true;
}
