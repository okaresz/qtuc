#ifndef ERRORHANDLERBASE_H
#define ERRORHANDLERBASE_H

#include <QObject>
#include <QHash>

namespace QtuC
{

class ErrorHandlerBase : public QObject
{
	Q_OBJECT
public:
	explicit ErrorHandlerBase(QObject *parent = 0);
	
signals:
	void signalError( QtMsgType severity, QString msg, QString location );
	
private slots:
	/** Error handler (slot).
	  * Call this when an error occurs.
	  * This function emits signalError() signal with these exact arguments,
	  * then calls qDebug/qWarning/qCritical/qFatal depending on the severity. The printed output contains the class name, location and message.
	  *	The execution is not halted explicitly, however qFatal() probably will terminate your program. Refer to Qt docs for more info.
	  *	@param type severity of error.
	  * @param msg A message string.
	  *	@param location Location (usually the function name) of the error.
	  *	@param details An associative array of detail name and value.*/
	virtual static void error( QtMsgType severity, const QString &msg, const QString &location = "", const QHash<QString,QString> &details = QHash<QString,QString>() );

	/// Alias for error().
	virtual static void debug( const QString &msg, const QString &location, const QHash<QString, QString> &details = "" );
};

}	//QtuC::
#endif // ERRORHANDLERBASE_H
