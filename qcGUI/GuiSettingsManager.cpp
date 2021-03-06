#include "GuiSettingsManager.h"

using namespace QtuC;

GuiSettingsManager::GuiSettingsManager(QObject *parent) :
	SettingsManagerBase(parent)
{
	// selfInfo
	if( !contains("selfInfo/id") )
		{ setValue( "selfInfo/id", "QcGui" ); }
	if( !contains("selfInfo/name") )
		{ setValue( "selfInfo/name", "QcGui" ); }
	if( !contains("selfInfo/desc") )
		{ setValue( "selfInfo/desc", "GUI client for the QtuC framework." ); }
	if( !contains("selfInfo/author") )
		{ setValue( "selfInfo/author", "okaresz" ); }

	// proxyAddress
	if( !contains("proxyAddress/host") )
		{ setValue( "proxyAddress/host", "localhost" ); }
	if( !contains("proxyAddress/port") )
		{ setValue( "proxyAddress/port", 24563 ); }

}

GuiSettingsManager *GuiSettingsManager::instance(QObject *parent)
{
	if( !instancePtr )
	{
		instancePtr = new GuiSettingsManager(parent);
	}
	return (GuiSettingsManager*)instancePtr;
}

void GuiSettingsManager::initCmdParser()
{

}
