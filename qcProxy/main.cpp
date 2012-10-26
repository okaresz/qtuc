#include <QtCore/QCoreApplication>
#include "ProxySettingsManager.h"
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

	// Create the settings object, the app as parent.
	ProxySettingsManager::instance(qcProxyApp);

	// ...and let the show begin!
	QcProxy *proxy = new QcProxy(qcProxyApp);
	if( !proxy )
	{
		//  oops..
		qFatal( "Fatal error, exit..." );
		return 1;
	}
	else
	{
		// Okay! Start the main thread's event loop.
		return qcProxyApp.exec();
	}
}
