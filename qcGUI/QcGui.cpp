#include "QcGui.h"
#include "GuiSettingsManager.h"
#include <QCoreApplication>

using namespace QtuC;
using namespace qcGui;

QcGui::QcGui(QObject *parent) : ErrorHandlerBase(parent),
	mProxyLink(0),
	mApiParser(0)
{
	// create settings
    QSettings::setDefaultFormat( QSettings::IniFormat );
	GuiSettingsManager::instance(this);

	mProxyState = new ProxyStateManager(this);
	connect( mProxyState, SIGNAL(setVariableOnDeviceRequest(DeviceStateVariable*,QString)), this, SLOT(handleSetVariableOnDeviceRequest(DeviceStateVariable*,QString)) );
}

QcGui::~QcGui()
{
	debug( debugLevelVeryVerbose, "QcGui destroyed", "~QcGui()" );
}

void QcGui::connectProxy()
{
	// If mProxyLink is not null, chances are that it's already connected!
	if( mProxyLink )
		{ return; }

	// give ClientConnectionManagerBase the self-info
	QHash<QString,QString> selfInfo;
	selfInfo.insert( QString("id"), GuiSettingsManager::instance()->value( "selfInfo/id" ).toString() );
	selfInfo.insert( QString("name"), GuiSettingsManager::instance()->value( "selfInfo/name" ).toString() );
	selfInfo.insert( QString("desc"), GuiSettingsManager::instance()->value( "selfInfo/desc" ).toString() );
	selfInfo.insert( QString("author"), GuiSettingsManager::instance()->value( "selfInfo/author" ).toString() );
	selfInfo.insert( QString("version"), QCoreApplication::instance()->applicationVersion() );
	ClientConnectionManagerBase::setSelfInfo(selfInfo);

	QTcpSocket *proxySocket = new QTcpSocket(0);

	connect( proxySocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(proxyConnectError()) );
	connect( proxySocket, SIGNAL(connected()), this, SLOT(proxyConnected()) );
	connect( proxySocket, SIGNAL(disconnected()), this, SLOT(proxyDisconnected()) );
	proxySocket->connectToHost( GuiSettingsManager::instance()->value("proxyAddress/host").toString(), GuiSettingsManager::instance()->value("proxyAddress/port").toInt() );
}

bool QcGui::setDeviceApi(const QString &apiString)
{
	if( !apiString.isEmpty() )
	{
		// Connect nothing on first pass, only if API is successfully parsed
		if( !mApiParser->parseAPI(apiString) )
		{
			error( QtCriticalMsg, "Failed to parse deviceAPI string", "setDeviceApi()" );
			return false;
		}
		else
		{
			//connect( mDeviceAPI, SIGNAL(newDeviceInfo(QString,QString)), mDeviceInstance, SLOT(setInfo(QString,QString)) );
			//connect( mDeviceAPI, SIGNAL(newHardwareInterface(QString,QString)), mDeviceInstance, SLOT(addHardwareInterface(QString,QString)) );
			connect( mApiParser, SIGNAL(newStateVariable(QHash<QString,QString>)), this, SLOT(createDeviceVariable(QHash<QString,QString>)) );
			connect( mApiParser, SIGNAL(newDeviceFunction(QString,QString,QString)), this, SLOT(createDeviceFunction(QString,QString,QString)) );
			if( !mApiParser->parseAPI(apiString) )
			{
				mApiParser->disconnect();
				error( QtCriticalMsg, "Failed to re-parse deviceAPI string", "setDeviceApi()" );
				return false;
			}
		}
	}
	else
	{
		error( QtCriticalMsg, "API string is empty", "setDeviceApi()" );
		return false;
	}

	emit deviceApiSet();
	return true;
}

bool QcGui::resetDeviceApi(const QString &apiString)
{
	clearDeviceApi();
	mApiParser = new DeviceAPIParser(this);
	return setDeviceApi(apiString);
}

void QcGui::clearDeviceApi()
{
	// first signal that this API will be destroyed
	emit deviceApiCleared();
	mApiParser->disconnect();
	delete mApiParser;
	mApiParser = 0;
}

bool QcGui::callDeviceFunction(const QString &hwInterface, const QString &name, const QStringList &argList)
{
	ClientCommandDevice *cmd = new ClientCommandDevice(deviceCmdCall);
	cmd->setInterface(hwInterface);
	cmd->setFunction(name);
	cmd->setArgList(argList);
	if( !mProxyLink->sendCommand( cmd ) )
	{
		error( QtWarningMsg, "Failed to call function", "callDeviceFunction()" );
		return false;
	}
	return true;
}

void QcGui::proxyConnectError()
{
	errorDetails_t errDet;
	errDet.insert( "error", ((QTcpSocket*)sender())->errorString() );
	error( QtCriticalMsg, "Failed to connect to proxy", "proxyConnectError", errDet );
	delete sender();
}

void QcGui::proxyConnected()
{
	mProxyLink = new ProxyConnectionManager( (QTcpSocket*)sender(), this );

	// disconnect socket from me, ProxyConnectionManager is in control now
	disconnect( sender(), 0, this, 0 );

	connect( mProxyLink, SIGNAL(connectionStateReady()), this, SLOT(proxyConnectionReady()) );
	connect( mProxyLink, SIGNAL(commandReceived(ClientCommandBase*)), this, SLOT(handleCommand(ClientCommandBase*)) );

	// initialize handshake process
	mProxyLink->sendHandShake();
}

void QcGui::proxyDisconnected()
{
	mProxyLink->disconnect();
	debug( debugLevelInfo, "Proxy disconnected", "proxyDisconnected()" );
}

void QcGui::proxyConnectionReady()
{
	debug( debugLevelVerbose, "Request device API...", "proxyConnectionReady()" );
	mProxyLink->sendCommand( new ClientCommandReqDeviceApi() );
}

void QcGui::handleSetVariableOnDeviceRequest(DeviceStateVariable *stateVar, QString newRawVal)
{
	ClientCommandDevice *cmd = new ClientCommandDevice( deviceCmdSet, stateVar );
	cmd->setArg( newRawVal );
	mProxyLink->sendCommand( cmd );
}

void QcGui::createDeviceVariable(QHash<QString, QString> varParams)
{
	// Remove autoUpdate-device, so no timers in the stateVars will be created unnecessarily.
	varParams.remove("autoUpdate-device");
	if( mProxyState->registerStateVariable(varParams) )
	{
		DeviceStateVariable *stateVar = mProxyState->getVar( varParams.value("hwInterface"), varParams.value("name") );
		if(  stateVar == 0 )
		{
			error( QtWarningMsg, "StateVar is null", "createDeviceVariable()");
			return;
		}

		emit deviceVariableCreated( stateVar, varParams.value("guiHint") );

		// subscribe if autoUpdate-user is present in the API
		if( varParams.contains("autoUpdate-user") )
		{
			bool ok;
			quint32 interval;
			interval = (quint32)varParams.value("autoUpdate-user").toInt(&ok);
			if( ok )
				{ mProxyLink->sendCommand( new ClientCommandSubscribe( interval, stateVar->getHwInterface(), stateVar->getName() ) ); }
			else
			{
				errorDetails_t errDet;
				errDet.insert( "hwInterface", stateVar->getHwInterface() );
				errDet.insert( "varName", stateVar->getName() );
				errDet.insert( "autoUpdateStr", varParams.value("autoUpdate-user") );
				error( QtWarningMsg, "Invalid value for user-side autoUpdate in API", "createDeviceVariable()" );
			}
		}
	}
	else
		{ error( QtWarningMsg, QString("Unable to register new stateVariable %1:%2").arg( varParams.value("hwInterface"), varParams.value("name") ), "createDeviceVariable()" ); }
}

void QcGui::createDeviceFunction(QString hwInterface, QString name, QString args)
{
	emit deviceFunctionCreated(hwInterface, name);
}

void QcGui::handleDeviceApiCmd(ClientCommandDeviceApi *apiCmd)
{
	if( !apiCmd->isDataValid() )
	{
		error( QtWarningMsg, "The received deviceAPI is corrupted", "handleCommand()" );
	}
	else
	{
		QString decodedApiString ( QString::fromUtf8( QByteArray::fromBase64(apiCmd->getEncodedApi()).data() ) );
		if( !mApiParser )
		{
			mApiParser = new DeviceAPIParser(this);
			if( !setDeviceApi(decodedApiString) )
				{ error( QtWarningMsg, "Failed to set received deviceAPI", "handleCommand()" ); }
		}
		else
		{
			if( !resetDeviceApi(decodedApiString) )
				{ error( QtWarningMsg, "Failed to set received deviceAPI", "handleCommand()" ); }
		}
	}
}

bool QcGui::handleDeviceCmd(ClientCommandDevice *deviceCmd)
{
	if( deviceCmd->getType() == deviceCmdSet )
	{
		DeviceStateVariable *var = mProxyState->getVar( deviceCmd->getHwInterface(), deviceCmd->getVariable() );
		if( var )
			{ var->setFromDevice( deviceCmd->getArg() ); }
		else
		{
			error( QtWarningMsg, QString("Failed to set variable from device, no such variable (hwI: %1, name: %2)").arg(deviceCmd->getHwInterface(),deviceCmd->getVariable()), "handleDeviceCmd()" );
			return false;
		}
		return true;
	}
	else return false;
}

void QcGui::handleCommand(ClientCommandBase *cmd)
{
	if( cmd->getClass() == ClientCommandBase::clientCommandDevice )
	{
		ClientCommandDevice *deviceCmd = (ClientCommandDevice*)cmd;
		handleDeviceCmd( deviceCmd );
		//debug( debugLevelInfo, QString("Device command received: %1 %2 %3 %4").arg(deviceCmd->getName(), deviceCmd->getHwInterface(), deviceCmd->getVariable(), deviceCmd->getArg() ), "handleCommand()" );
	}
	else	//control
	{
		if( cmd->getName() == "deviceAPI" )
		{
			handleDeviceApiCmd( (ClientCommandDeviceApi*)cmd );
		}
		else
			{ debug( debugLevelInfo, QString("Unhandled command: %1").arg(cmd->getName()), "handleCommand()" ); }
	}
	cmd->deleteLater();
}
