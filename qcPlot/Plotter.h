#ifndef PLOTTER_H
#define PLOTTER_H

#include <QObject>
#include "PlotConfig.h"
#include "DeviceStatePlotDataVariable.h"
#include <QPair>

namespace qcPlot
{

class Plotter : public QObject
{
	Q_OBJECT
public:
	/// Takes ownership of the config
	explicit Plotter( PlotConfig *config, QObject *parent = 0 );

	PlotConfig const * cfg() const
		{ return mConfig; }

	DeviceStatePlotDataVariable *resolveStateVar( const QString &hwi, const QString &name );
	DeviceStatePlotDataVariable *resolveStateVar( const QPair<QString,QString> &varPair )
		{ return resolveStateVar( varPair.first, varPair.second ); }

	inline void start()
		{ setEnabled(true); }

	inline void stop()
		{ setEnabled(false); }

	void setEnabled( bool enabled = true );

	/// Reset plot, clear history.
	void reset();

	/// Clear history of all curves.
	void clear();

signals:
	void hadReset();

private:
	PlotConfig *mConfig;
	
};

}	//qcPlot::
#endif // PLOTTER_H
