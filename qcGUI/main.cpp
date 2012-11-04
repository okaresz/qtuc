#include <QtGui/QApplication>
#include "QcGuiMainWindow.h"
#include "ErrorHandlerBase.h"
#include "QcGui.h"

using namespace QtuC;

void parseAppArg( const QString &, int & );

int main(int argc, char *argv[])
{
	int versionMajor = 0;
	int versionMinor = 5;
	int versionFix = 0;

	QApplication qcGuiApp(argc, argv);

	qcGuiApp.setApplicationName( "qcGUI" );
	qcGuiApp.setOrganizationName( "QtuC" );
	qcGuiApp.setOrganizationDomain( "QtuC" );
	qcGuiApp.setApplicationVersion( QString::number(versionMajor) + "." + QString::number(versionMinor) + "." + QString::number(versionFix) );

	// Install a custom mesage handler
	qInstallMsgHandler( ErrorHandlerBase::customMessageHandler );

	QcGuiMainWindow mainWindow;
	mainWindow.setWindowTitle( qcGuiApp.applicationName() );

	// Parse arguments
	QStringList appArgs = qcGuiApp.arguments();
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
	QcGui *gui = new QcGui();
	if( !gui )
	{
		//  oops..
		qCritical( "Fatal error, exit..." );
		delete gui;
		return 1;
	}
	else
	{
		gui->connect( &qcGuiApp, SIGNAL(aboutToQuit()), gui, SLOT(deleteLater()) );
		mainWindow.show();
		gui->connectProxy();

		// Okay! Start the main thread's event loop.
		return qcGuiApp.exec();
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
