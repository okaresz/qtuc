#ifndef DEVICECOMMANDBASE_H
#define DEVICECOMMANDBASE_H

#include <QObject>
#include <QStringList>

namespace QtuC
{

enum deviceCommandType_t
{
	undefined_cmd,
	get_cmd,
	set_cmd,
	call_cmd
};

class DeviceCommandBase : public QObject
{
public:

	DeviceCommandBase();

	DeviceCommandBase( DeviceCommandBase deviceCommand );

	DeviceCommandBase( DeviceCommandBase* deviceCommand );

	DeviceCommandBase( deviceCommandType_t cmdType, DeviceStateVariable* stateVar, bool convert );

	bool isValid();

	void getType() const;


	QString getInterface();

	QString getVar();

	QString getArg();

	void setType( deviceCommandType_t t );


	void setInterface( const QString & hwi );


	void setVar( const QString& cv );


	void setArg( const QString& ca );

private:

	QString cmdHwInterface;
	deviceCommandType_t cmdType;
	QString cmdVar;
	QStringList cmdArgs;
	bool valid;
	bool checkArgCount();
};

}	//QtuC::
#endif //DEVICECOMMANDBASE_H

