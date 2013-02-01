#ifndef ERRORHANDLERBASE_H
#define ERRORHANDLERBASE_H

#include <QObject>
#include <QHash>

namespace QtuC
{

enum debugLevel_t
{
	debugLevelInfo,
	debugLevelVerbose,
	debugLevelVeryVerbose
};

/** Error and debug print functions.
*	@todo console colors, formatting; simple "message" type print, where INFO is the beginning, and no location is printed, and could use Q_FUNC_INFO.*/
class ErrorHandlerBase : public QObject
{
	Q_OBJECT

public:
	typedef QHash<const char*,QString> errorDetails_t;

	explicit ErrorHandlerBase(QObject *parent = 0);

	/** A custom Qt message handler.
	  *	See QtGlobal documentation for details.*/
	static void customMessageHandler( QtMsgType msgType, const char *msg );

	/** Get current application debug level.
	  *	@return Current debug level.*/
	static debugLevel_t getDebugLevel()
		{ return mDebugLevel; }

	/** Set the application debug level.
	  *	@param level The new debug level to set.*/
	static void setDebugLevel( debugLevel_t level )
		{ mDebugLevel = level; }

	// static versions....

	static void error( QtMsgType severity, const QString &msg, char const *functionName, char const*className, const errorDetails_t &details = QHash<const char*,QString>() );
	static void error( QtMsgType severity, char const *msg, char const *functionName, char const *className, const errorDetails_t &details = QHash<const char*,QString>() );
	static void debug( debugLevel_t debugLevel, const QString &msg, const char *functionName, const char *className, const errorDetails_t &details = QHash<const char*, QString>() );
	static void debug( debugLevel_t debugLevel, const char *msg, const char *functionName, const char *className, const errorDetails_t &details = QHash<const char*, QString>() );
	
signals:
	/** Emitted whenever an error occurs.
	  *	@param severity Severity of the error.
	  *	@param location Location of the error.
	  * @param details Details. The type must be written fully resolved, because the signal/slot type comparison of Qt (requires full char-by-char match).*/
	void signalError( QtMsgType severity, QString msg, QString location, const QtuC::ErrorHandlerBase::errorDetails_t details ) const;

protected slots:
	/** Error handler (slot).
	  * Call this when an error occurs.
	  * This function emits signalError() signal with these exact arguments,
	  * then calls qDebug/qWarning/qCritical/qFatal depending on the severity. The printed output contains the class name, location and message.
	  *	The execution is not halted explicitly, however qFatal() probably will terminate your program. Refer to Qt docs for more info.
	  *	@param type severity of error.
	  * @param msg A message string.
	  *	@param location Location (usually the function name) of the error.
	  *	@param details An associative array of detail name and value.*/
	void error( QtMsgType severity, const QString &msg, char const *location, const errorDetails_t &details = QHash<const char*,QString>() ) const;

	/// Overloaded function.
	void error( QtMsgType severity, char const *msg, char const *location, const errorDetails_t &details = QHash<const char*,QString>() ) const;

	/// Alias for error().
	void debug( debugLevel_t debugLevel, const QString &msg, char const *location, const errorDetails_t &details = QHash<const char*, QString>() ) const;
	void debug( debugLevel_t debugLevel, char const *msg, char const *location, const errorDetails_t &details = QHash<const char*, QString>() ) const;

private:

	static void printError( QtMsgType severity, const QString &msg, char const *functionName, char const*className, const errorDetails_t &details = QHash<const char*,QString>() );

	static debugLevel_t mDebugLevel;	/// Application debug level, set as a command line switch
};

}	//QtuC::
#endif // ERRORHANDLERBASE_H
