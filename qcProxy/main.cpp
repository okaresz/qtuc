#include <QCoreApplication>
#include "QcProxy.h"
#include <QDebug>
#include "ErrorHandlerBase.h"
#include "ProxySettingsManager.h"

#ifdef Q_OS_UNIX
    #include <signal.h>
#endif

using namespace QtuC;

/** Signal handling...
  *	For more info, see http://www.cplusplus.com/forum/unices/16430/, http://linux.die.net/man/2/sigaction*/
#ifdef Q_OS_UNIX
    struct sigaction sigHandlerConf;
	void onSignal( int signum );
#endif

int main(int argc, char *argv[])
{
	int versionMajor = 0;
	int versionMinor = 8;
	int versionFix = 5;

	QCoreApplication qcProxyApp(argc, argv);

	qcProxyApp.setApplicationName( "qcProxy" );
	qcProxyApp.setOrganizationName( "QtuC" );
	qcProxyApp.setOrganizationDomain( "QtuC" );
	qcProxyApp.setApplicationVersion( QString::number(versionMajor) + "." + QString::number(versionMinor) + "." + QString::number(versionFix) );

	// Install a custom mesage handler
	qInstallMsgHandler( ErrorHandlerBase::customMessageHandler );

	// Create the Proxy object
	QcProxy *proxy = new QcProxy();

	// Create the settings object, QcProxy as parent.
	// On windows default format is registry, but we want file.
	#ifdef Q_OS_WIN32
		QSettings::setDefaultFormat( QSettings::IniFormat );
	#endif

	ProxySettingsManager::instance( proxy );

	if( !ProxySettingsManager::instance()->parseCmdArgs( QCoreApplication::arguments() ) )
	{
		ProxySettingsManager::instance()->showCmdHelp();
		delete proxy;
		return -1;
	}

	ErrorHandlerBase::setDebugLevel( (debugLevel_t)ProxySettingsManager::instance()->getCmdArgValue(ProxySettingsManager::cmdArgVerbose).toUInt() );

	if( !proxy->start() )
	{
		//  oops..
		qCritical( "Fatal error, exit..." );
		delete proxy;
		return 1;
	}
	else
    {
    #ifdef Q_OS_UNIX
		// add *nix signal handlers
		sigHandlerConf.sa_handler = onSignal;
		sigemptyset(&sigHandlerConf.sa_mask);
		sigHandlerConf.sa_flags = 0;
		sigaction( SIGINT, &sigHandlerConf, 0 );
    #endif

		// Okay! Start the main thread's event loop.
		return qcProxyApp.exec();
	}
}

#ifdef Q_OS_UNIX
void onSignal(int signum)
{
	switch( signum )
	{
		case SIGINT:
			qDebug( "Cought SIGINT (%d), quit...", signum );
			QCoreApplication::instance()->quit();
			break;
		case SIGTERM:
			qDebug( "Cought SIGTERM (%d), quit...", signum );
			QCoreApplication::instance()->quit();
			break;
	}
}
#endif
