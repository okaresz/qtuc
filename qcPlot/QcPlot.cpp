#include "QcPlot.h"
#include "PlotSettingsManager.h"
#include <QCoreApplication>
#include "DeviceStateVariableBase.h"
#include "PlotConfigView.h"

using namespace QtuC;
using namespace qcPlot;

QcPlot::QcPlot(QObject *parent) : ErrorHandlerBase(parent),
	mProxyState(0),
	mProxyLink(0),
	mApiParser(0),
	mPlotManager(0)
{
	// create settings
    QSettings::setDefaultFormat( QSettings::IniFormat );
	PlotSettingsManager::instance(this);

	mProxyState = new ProxyStateManager(this);
	connect( mProxyState, SIGNAL(stateVariableSendRequest(DeviceStateVariableBase*)), this, SLOT(handleStateVariableSendRequest(DeviceStateVariableBase*)) );

	mPlotManager = new PlotManager(this);
}

QcPlot::~QcPlot()
{
	debug( debugLevelVeryVerbose, "QcGui destroyed", "~QcGui()" );
}

void QcPlot::connectProxy()
{
	// If mProxyLink is not null, chances are that it's already connected!
	if( mProxyLink )
		{ return; }

	// give ClientConnectionManagerBase the self-info
	QHash<QString,QString> selfInfo;
	selfInfo.insert( QString("id"), PlotSettingsManager::instance()->value( "selfInfo/id" ).toString() );
	selfInfo.insert( QString("name"), PlotSettingsManager::instance()->value( "selfInfo/name" ).toString() );
	selfInfo.insert( QString("desc"), PlotSettingsManager::instance()->value( "selfInfo/desc" ).toString() );
	selfInfo.insert( QString("author"), PlotSettingsManager::instance()->value( "selfInfo/author" ).toString() );
	selfInfo.insert( QString("version"), QCoreApplication::instance()->applicationVersion() );
	ClientConnectionManagerBase::setSelfInfo(selfInfo);

	QTcpSocket *proxySocket = new QTcpSocket(0);

	connect( proxySocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(proxyConnectError()) );
	connect( proxySocket, SIGNAL(connected()), this, SLOT(proxyConnected()) );
	connect( proxySocket, SIGNAL(disconnected()), this, SLOT(proxyDisconnected()) );
	proxySocket->connectToHost( PlotSettingsManager::instance()->value("proxyAddress/host").toString(), PlotSettingsManager::instance()->value("proxyAddress/port").toInt() );
}

bool QcPlot::setDeviceApi(const QString &apiString)
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
			mApiParser->clear();
			connect( mApiParser, SIGNAL(signalError(QtMsgType,QString,QString,QtuC::ErrorHandlerBase::errorDetails_t)), this, SIGNAL(signalError(QtMsgType,QString,QString,QtuC::ErrorHandlerBase::errorDetails_t)) );
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

bool QcPlot::resetDeviceApi(const QString &apiString)
{
	clearDeviceApi();
	mApiParser = new DeviceAPIParser(this);
	return setDeviceApi(apiString);
}

void QcPlot::clearDeviceApi()
{
	// first signal that this API will be destroyed
	emit deviceApiCleared();
	mApiParser->disconnect();
	delete mApiParser;
	mApiParser = 0;
}

bool QcPlot::callDeviceFunction(const QString &hwInterface, const QString &name, const QStringList &argList)
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

QHash<QString, QStringList> QcPlot::getVariableList() const
{
	QHash<QString,QStringList> varNlist;
	QList<DeviceStateVariableBase*> varPList = mProxyState->getVarList();
	int i = varPList.size();
	while( i-- )
	{
		if( !varNlist.contains(varPList.at(i)->getHwInterface()) )
		{
			QStringList newVarList;
			newVarList.append( varPList.at(i)->getName() );
			varNlist.insert( varPList.at(i)->getHwInterface(), newVarList );
		}
		else
		{
			varNlist[varPList.at(i)->getHwInterface()].append( varPList.at(i)->getName() );
		}
	}
	return varNlist;
}

void QcPlot::proxyConnectError()
{
	errorDetails_t errDet;
	errDet.insert( "error", ((QTcpSocket*)sender())->errorString() );
	error( QtCriticalMsg, "Failed to connect to proxy", "proxyConnectError", errDet );
	delete sender();
}

void QcPlot::proxyConnected()
{
	mProxyLink = new ProxyConnectionManager( (QTcpSocket*)sender(), this );

	// disconnect socket from me, ProxyConnectionManager is in control now
	disconnect( sender(), 0, this, 0 );

	connect( mProxyLink, SIGNAL(connectionStateReady()), this, SLOT(proxyConnectionReady()) );
	connect( mProxyLink, SIGNAL(commandReceived(ClientCommandBase*)), this, SLOT(handleCommand(ClientCommandBase*)) );

	// initialize handshake process
	mProxyLink->sendHandShake();
}

void QcPlot::proxyDisconnected()
{
	mProxyLink->disconnect();
	debug( debugLevelInfo, "Proxy disconnected", "proxyDisconnected()" );
}

void QcPlot::proxyConnectionReady()
{
	emit proxyHasConnected();
	debug( debugLevelVerbose, "Request device API...", "proxyConnectionReady()" );
	mProxyLink->sendCommand( new ClientCommandReqDeviceApi() );
}

void QcPlot::handleStateVariableSendRequest(DeviceStateVariableBase *stateVar )
{
	ClientCommandDevice *cmd = new ClientCommandDevice( deviceCmdSet, stateVar );
	mProxyLink->sendCommand( cmd );
}

void QcPlot::createDeviceVariable(QHash<QString, QString> varParams)
{
	// Remove autoUpdate-device, so no timers in the stateVars will be created unnecessarily.
	varParams.remove("autoUpdate-device");
	if( mProxyState->registerNewStateVariable(varParams) )
	{
		DeviceStateVariableBase *stateVar = mProxyState->getVar( varParams.value("hwInterface"), varParams.value("name") );
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

void QcPlot::createDeviceFunction(QString hwInterface, QString name, QString args)
{
	Q_UNUSED(args);
	emit deviceFunctionCreated(hwInterface, name);
}

void QcPlot::handleDeviceApiCmd(ClientCommandDeviceApi *apiCmd)
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

bool QcPlot::handleDeviceCmd(ClientCommandDevice *deviceCmd)
{
	if( deviceCmd->getType() == deviceCmdSet )
	{
		DeviceStateVariableBase *var = mProxyState->getVar( deviceCmd->getHwInterface(), deviceCmd->getVariable() );
		if( var )
			{ var->updateFromSource( deviceCmd->getArg() ); }
		else
		{
			error( QtWarningMsg, QString("Failed to set variable from device, no such variable (hwI: %1, name: %2)").arg(deviceCmd->getHwInterface(),deviceCmd->getVariable()), "handleDeviceCmd()" );
			return false;
		}
		return true;
	}
	else return false;
}

void QcPlot::handleCommand(ClientCommandBase *cmd)
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
