#include <QCoreApplication>
#include "QcProxy.h"
#include <QDebug>
#include "ErrorHandlerBase.h"
#include <signal.h>

using namespace QtuC;

void parseAppArg( const QString &, int & );

/** Signal handling...
  *	For more info, see http://www.cplusplus.com/forum/unices/16430/, http://linux.die.net/man/2/sigaction*/
struct sigaction sigHandlerConf;
void onSigInt( int signum );

int main(int argc, char *argv[])
{
	int versionMajor = 0;
	int versionMinor = 5;
	int versionFix = 0;

	QCoreApplication qcProxyApp(argc, argv);

	qcProxyApp.setApplicationName( "qcProxy" );
	qcProxyApp.setOrganizationName( "QtuC" );
	qcProxyApp.setOrganizationDomain( "QtuC" );
	qcProxyApp.setApplicationVersion( QString::number(versionMajor) + "." + QString::number(versionMinor) + "." + QString::number(versionFix) );

	// Install a custom mesage handler
	qInstallMsgHandler( ErrorHandlerBase::customMessageHandler );

	// Parse arguments
	QStringList appArgs = qcProxyApp.arguments();
	for( int i=0; i<appArgs.size(); )
	{
		if( i == 0 )	// the command that started this application
		{
			++i;
			continue;
		}

		parseAppArg( appArgs.at(i), i );
	}

	// ...and let the show begin!
	QcProxy *proxy = new QcProxy();
	if( !proxy->start() )
	{
		//  oops..
		qCritical( "Fatal error, exit..." );
		delete proxy;
		return 1;
	}
	else
	{
		// add *nix signal handlers
		sigHandlerConf.sa_handler = onSigInt;
		sigemptyset(&sigHandlerConf.sa_mask);
		sigHandlerConf.sa_flags = 0;
		sigaction( SIGINT, &sigHandlerConf, 0 );

		// Okay! Start the main thread's event loop.
		return qcProxyApp.exec();
	}
}

void parseAppArg( const QString &appArg, int &argIndex )
{
	QString arg(appArg);
	// switches
	if( arg.startsWith('-') )
	{
		arg = arg.mid(1);
		if( arg == "v" )
		{
			ErrorHandlerBase::setDebugLevel( debugLevelVerbose );
			++argIndex;
		}
		else if( arg == "vv" )
		{
			ErrorHandlerBase::setDebugLevel( debugLevelVeryVerbose );
			++argIndex;
		}
	}
	// args
	else if( arg.startsWith("--") )
	{
		arg = arg.mid(2);
	}
}

void onSigInt(int signum)
{
	qDebug( "Cought SIGINT, quit..." );
	QCoreApplication::instance()->quit();
}
