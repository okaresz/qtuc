#ifndef QCGUIMAINWINDOW_H
#define QCGUIMAINWINDOW_H

#include "QcGui.h"
#include "StateVariablesView.h"
#include <QMainWindow>
#include <QToolBar>
#include <QHash>
#include <QAction>

// Forward declare the auto-generated UI class
namespace Ui
	{ class QcGuiMainView; }

namespace qcGui {

class QcGuiMainView : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit QcGuiMainView(QcGui *model);
	~QcGuiMainView();

	/** Set model pointer,
	  *	Set the model and do the necessary initializations (eg. connect())
	  *	@param model The model pointer to set.
	  *	@return True on success, false otherwise.*/
	//bool setModel( QcGui *model );

	void createGui();

public slots:
	void showError( QtMsgType severity, QString msg, QString location, const QtuC::ErrorHandlerBase::errorDetails_t details );

private slots:

	void onConnectActionTriggered();
	void onDeviceApiSet();

signals:

	void connectProxy();
	void disconnectProxy();
	
private:
	/// Initialize the view (after mModel has been set).
	void initModelView();

	Ui::QcGuiMainView *mUi;
	QcGui *mModel;
	StateVariablesView *mVariableView;

	QHash<QString, QAction*> mActions;
	QToolBar *mMainToolBar;
};

}	//qcGui::
#endif // QCGUIMAINWINDOW_H
