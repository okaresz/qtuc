#ifndef PLOTSETTINGSMANAGER_H
#define PLOTSETTINGSMANAGER_H

#include "SettingsManagerBase.h"

namespace qcPlot
{

class PlotSettingsManager : public QtuC::SettingsManagerBase
{
	Q_OBJECT
public:
	explicit PlotSettingsManager(QObject *parent = 0);

	static PlotSettingsManager *instance(QObject *parent = 0);

private:
	void initCmdParser();
};

}	//QcPlot::
#endif // PLOTSETTINGSMANAGER_H
