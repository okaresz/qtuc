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
