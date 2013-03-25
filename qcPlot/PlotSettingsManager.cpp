#include "PlotSettingsManager.h"

using namespace qcPlot;

PlotSettingsManager::PlotSettingsManager(QObject *parent) :
	SettingsManagerBase(parent)
{
	// selfInfo
	if( !contains("selfInfo/id") )
		{ setValue( "selfInfo/id", "QcPlot" ); }
	if( !contains("selfInfo/name") )
		{ setValue( "selfInfo/name", "QcPlot" ); }
	if( !contains("selfInfo/desc") )
		{ setValue( "selfInfo/desc", "Plot client for the QtuC framework." ); }
	if( !contains("selfInfo/author") )
		{ setValue( "selfInfo/author", "okaresz" ); }

	// proxyAddress
	if( !contains("proxyAddress/host") )
		{ setValue( "proxyAddress/host", "localhost" ); }
	if( !contains("proxyAddress/port") )
		{ setValue( "proxyAddress/port", 24563 ); }

}

PlotSettingsManager *PlotSettingsManager::instance(QObject *parent)
{
	if( !instancePtr )
	{
		instancePtr = new PlotSettingsManager(parent);
	}
	return (PlotSettingsManager*)instancePtr;
}

void PlotSettingsManager::initCmdParser()
{
}
