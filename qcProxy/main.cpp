#include <QtCore/QCoreApplication>

int main(int argc, char *argv[])
{
	QCoreApplication qcProxyApp(argc, argv);

	qcProxyApp.setApplicationName( "qcProxy" );
	qcProxyApp.setOrganizationName( "QtuC" );
	qcProxyApp.setOrganizationDomain( "QtuC" );
	
	return qcProxyApp.exec();
}
