#include <QtGui/QApplication>
#include "QcGuiMainView.h"
#include "ErrorHandlerBase.h"
#include "QcGui.h"

using namespace QtuC;
using namespace qcGui;

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

	// create gui model
	QcGui *qcGuiModel = new QcGui();
	if( !qcGuiModel )
	{
		//  oops..
		qCritical( "Fatal error, exit..." );
		delete qcGuiModel;
		return 1;
	}
	else
	{
		qcGuiModel->connect( &qcGuiApp, SIGNAL(aboutToQuit()), qcGuiModel, SLOT(deleteLater()) );


		QcGuiMainView mainView(qcGuiModel);
		mainView.setWindowTitle( qcGuiApp.applicationName() );
		mainView.show();

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
