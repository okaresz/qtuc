#include "ErrorHandlerBase.h"
#include <QDebug>

using namespace QtuC;

ErrorHandlerBase::ErrorHandlerBase(QObject *parent) : QObject(parent)
{
}

void ErrorHandlerBase::customMessageHandler(QtMsgType msgType, const char *msg)
{
	switch( msgType )
	{
		case QtDebugMsg: cout << "[DEBUG] " << msg; break;
		case QtWarningMsg: cout << "[WARN] " << msg; break;
		case QtCriticalMsg: cout << "[CRIT] " << msg; break;
		case QtFatalMsg: cout << "[FATAL] " << msg;
	}
}

void ErrorHandlerBase::error( QtMsgType severity, const QString &msg, const QString &location, const QHash<QString, QString> &details )
{
	if(this)
		{ emit signalError(severity, msg, location); }

	QString className;
	if(this)
		{ className = this->metaObject()->className(); }

	if( !className.isEmpty() )
		{ QString line = "(@" + className + "::" + location + "): " + msg; }
	else
		{ QString line = "(@" + location + "): " + msg; }
	switch( severity )
	{
		case QtDebugMsg: qDebug() << line; break;
		case QtWarningMsg: qWarning() << line; break;
		case QtCriticalMsg: qCritical(); << line; break;
		case QtFatalMsg: qFatal(line);
	}
}

void ErrorHandlerBase::debug( const QString &msg, const QString &location, const QHash<QString, QString> &details)
{
	error( QtDebugMsg, msg, location, details );
}
