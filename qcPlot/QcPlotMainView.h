#ifndef QCGUIMAINWINDOW_H
#define QCGUIMAINWINDOW_H

#include "QcPlot.h"
#include <QMainWindow>
#include <QToolBar>
#include <QHash>
#include <QAction>

namespace qcPlot {

/** Handle the view of th device state variables.*/
class QcPlotMainView : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit QcPlotMainView(QcPlot *model);
	~QcPlotMainView();

	/** Set model pointer,
	  *	Set the model and do the necessary initializations (eg. connect())
	  *	@param model The model pointer to set.
	  *	@return True on success, false otherwise.*/
	//bool setModel( QcGui *model );

	void createGui();

public slots:
	void showError( QtMsgType severity, QString msg, QString location, const QtuC::ErrorHandlerBase::errorDetails_t details );

private slots:

	// Actions
	void onConnectActionTriggered();
	void onNewPlotActionTriggered();
	void onSaveLayoutActionTriggered();
	void onLoadLayoutActionTriggered();

	//dialogs, user interactions,...
	void onNewPlotDialogSubmit();

	// else
	void onProxyConnected();
	void onDeviceApiSet();
	void onNewPlotterAdded( Plotter* plotter );

signals:

	void connectProxy();
	void disconnectProxy();
	
private:
	/// Initialize the view (after mModel has been set).
	void initModelView();

	QcPlot *mModel;

	QHash<QString, QAction*> mActions;
	QToolBar *mMainToolBar;
};

}	//QcPlot::
#endif // QCGUIMAINWINDOW_H
