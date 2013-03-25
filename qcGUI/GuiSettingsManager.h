#ifndef GUISETTINGSMANAGER_H
#define GUISETTINGSMANAGER_H

#include "SettingsManagerBase.h"

namespace QtuC
{

class GuiSettingsManager : public SettingsManagerBase
{
	Q_OBJECT
public:
	explicit GuiSettingsManager(QObject *parent = 0);

	static GuiSettingsManager *instance(QObject *parent = 0);

private:
	void initCmdParser();
};

}	//QtuC::
#endif // GUISETTINGSMANAGER_H
