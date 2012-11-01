#include "ErrorHandlerBase.h"
#include <QDebug>
#include <iostream>

using namespace QtuC;

ErrorHandlerBase::ErrorHandlerBase(QObject *parent) : QObject(parent)
{
}

void ErrorHandlerBase::customMessageHandler(QtMsgType msgType, const char *msg)
{
	switch( msgType )
	{
		case QtDebugMsg: std::cout << "[DEBUG] " << msg << std::endl; break;
		case QtWarningMsg: std::cerr << "[WARN] " << msg << std::endl; break;
		case QtCriticalMsg: std::cerr << "[CRIT] " << msg << std::endl; break;
		case QtFatalMsg: std::cerr << "[FATAL] " << msg << std::endl;
	}
}

void ErrorHandlerBase::error( QtMsgType severity, const QString &msg, char const *location, const errorDetails_t &details ) const
{
	emit signalError(severity, QString(msg), location);

	QString className = this->metaObject()->className();

	QString line;
	if( !className.isEmpty() )
		{ line = QString("(@%1::%2): %3").arg(className,location,msg); }
	else
		{ line = QString("(@%1): %2").arg(location,msg); }
	switch( severity )
	{
		case QtDebugMsg: qDebug() << line; break;
		case QtWarningMsg: qWarning() << line; break;
		case QtCriticalMsg: qCritical() << line; break;
		case QtFatalMsg: qFatal(line.toStdString().c_str());
	}
}

void ErrorHandlerBase::error(QtMsgType severity, char const *msg, char const *location, const errorDetails_t &details) const
{
	error( severity, QString(msg), location, details );
}

void ErrorHandlerBase::debug( debugLevel_t debugLevel, const QString &msg, char const *location, const errorDetails_t &details) const
{
	error( QtDebugMsg, msg, location, details );
}

void ErrorHandlerBase::debug( debugLevel_t debugLevel, char const *msg, char const *location, const errorDetails_t &details) const
{
	error( QtDebugMsg, msg, location, details );
}


void ErrorHandlerBase::error( QtMsgType severity, const QString &msg, char const *functionName, char const *className, const ErrorHandlerBase::errorDetails_t &details )
{
	QString line;
	if( !QString(className).isEmpty() )
		{ line = QString("(@%1::%2): %3").arg(className,functionName,msg); }
	else
		{ line = QString("(@%1): %2").arg(functionName,msg); }
	switch( severity )
	{
		case QtDebugMsg: qDebug() << line; break;
		case QtWarningMsg: qWarning() << line; break;
		case QtCriticalMsg: qCritical() << line; break;
		case QtFatalMsg: qFatal(line.toStdString().c_str());
	}
}

void ErrorHandlerBase::error( QtMsgType severity, char const *msg, char const *functionName, char const *className, const ErrorHandlerBase::errorDetails_t &details )
{
	error( severity, QString(msg), functionName, className, details );
}

void ErrorHandlerBase::debug( debugLevel_t debugLevel, const QString &msg, const char *functionName, const char *className, const ErrorHandlerBase::errorDetails_t &details )
{
	error( QtDebugMsg, msg, functionName, className, details );
}

void ErrorHandlerBase::debug( debugLevel_t debugLevel, const char *msg, const char *functionName, const char *className, const ErrorHandlerBase::errorDetails_t &details )
{
	error( QtDebugMsg, msg, functionName, className, details );
}
