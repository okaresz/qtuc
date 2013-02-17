#ifndef CURVECONFIG_H
#define CURVECONFIG_H

#include <QObject>
#include <QString>
#include <QPair>

namespace qcPlot
{

class CurveConfig : public QObject
{
	Q_OBJECT
public:
	explicit CurveConfig(QObject *parent = 0);

	/// Parent is not copied! Id is copied!
	CurveConfig( CurveConfig const &other, QObject *parent = 0 );

	/// Id is copied!
	bool copy( CurveConfig const &other );

	inline uint id() const
		{ return mId; }

	inline QString const name() const
		{ return mName; }

	void setName( QString const &name )
	{
		mName = name;
		emit nameChanged(mName);
	}

	inline QPair<QString,QString> const stateVariable() const
		{ return mStateVariable; }

	void setStateVariable( QString const &hwi, QString const &name );

signals:
	void nameChanged( QString newName );

private:
	QString mName;
	uint mId;
	QPair<QString,QString> mStateVariable;

	static uint mCurveIdCounter;
};

}	//qcPlot::
#endif // CURVECONFIG_H
