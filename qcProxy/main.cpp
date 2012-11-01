#include <QCoreApplication>
#include "QcProxy.h"
#include <QDebug>
#include "ErrorHandlerBase.h"

using namespace QtuC;

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

	// ...and let the show begin!
	QcProxy *proxy = new QcProxy();
	if( !proxy->start() )
	{
		//  oops..
		delete proxy;
		qFatal( "Fatal error, exit..." );
		return 1;
	}
	else
	{
		proxy->connect( &qcProxyApp, SIGNAL(aboutToQuit()), proxy, SLOT(deleteLater()) );
		// Okay! Start the main thread's event loop.
		return qcProxyApp.exec();
	}
}
