#include <QtGui/QApplication>
#include "ErrorHandlerBase.h"
#include "QcPlot.h"
#include "QcPlotMainView.h"

using namespace QtuC;
using namespace qcPlot;

void parseAppArg( const QString &, int & );

int main(int argc, char *argv[])
{
	int versionMajor = 0;
	int versionMinor = 1;
	int versionFix = 0;

	QApplication qcPlotApp(argc, argv);

	qcPlotApp.setApplicationName( "QcPlot" );
	qcPlotApp.setOrganizationName( "QtuC" );
	qcPlotApp.setOrganizationDomain( "QtuC" );
	qcPlotApp.setApplicationVersion( QString::number(versionMajor) + "." + QString::number(versionMinor) + "." + QString::number(versionFix) );

	// Install a custom mesage handler
	qInstallMsgHandler( ErrorHandlerBase::customMessageHandler );

	// Parse arguments
	QStringList appArgs = qcPlotApp.arguments();
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
	QcPlot *qcPlotModel = new QcPlot();
	if( !qcPlotModel )
	{
		//  oops..
		qCritical( "Fatal error, exit..." );
		delete qcPlotModel;
		return 1;
	}
	else
	{
		qcPlotModel->connect( &qcPlotApp, SIGNAL(aboutToQuit()), qcPlotModel, SLOT(deleteLater()) );


		QcPlotMainView mainView(qcPlotModel);
		mainView.setWindowTitle( qcPlotApp.applicationName() );
		mainView.show();

		// Okay! Start the main thread's event loop.
		return qcPlotApp.exec();
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
