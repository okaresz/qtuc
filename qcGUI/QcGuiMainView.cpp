#include "QcGuiMainView.h"
#include "ErrorHandlerBase.h"
#include "ui_QcGuiMainView.h"
#include <QStatusBar>
#include <QMessageBox>
#include <QStringList>
#include "GuiSettingsManager.h"

using namespace qcGui;

QcGuiMainView::QcGuiMainView(QcGui *model) :
	QMainWindow(0),
	mUi(new Ui::QcGuiMainView),
	mModel(model),
	mVariableView(0)
{
	//mUi->setupUi(this);

	createGui();

	initModelView();
}

QcGuiMainView::~QcGuiMainView()
{
	delete mUi;
}

/*
bool QcGuiMainView::setModel(QcGui *model)
{
	if( !mModel )
	{
		mModel = model;
		if( mVariableView )
			{ mVariableView->setModel(model); }
		initModelView();
		return true;
	}
	else
	{
		QtuC::ErrorHandlerBase::error( QtWarningMsg, "Model is already set", "setModel()", "QcGuiMainView" );
		return false;
	}
}
*/

void QcGuiMainView::createGui()
{
	mMainToolBar = new QToolBar(this);

	QAction *connectAction = new QAction( QIcon(""), tr("Connect"), this );
	connectAction->setObjectName( QString::fromUtf8( "connectAction" ) );
	connectAction->setToolTip( tr("connect to proxy") );
	mActions.insert( "connect", connectAction );
	connect( connectAction, SIGNAL(triggered()), this, SLOT(onConnectActionTriggered()) );

	mMainToolBar->addAction(connectAction);

	addToolBar( Qt::TopToolBarArea, mMainToolBar );
	setStatusBar( new QStatusBar(this) );

	mVariableView = new StateVariablesView();

	setCentralWidget(mVariableView);
	mVariableView->show();

	QMetaObject::connectSlotsByName(this);
}

void QcGuiMainView::showError(QtMsgType severity, QString msg, QString location, const QtuC::ErrorHandlerBase::errorDetails_t details)
{
	QMessageBox msgBox;
	switch( severity )
	{
		case QtDebugMsg: return; //msgBox.setIcon( QMessageBox::Information ); break;
		case QtCriticalMsg: msgBox.setIcon( QMessageBox::Critical ); break;
		default: msgBox.setIcon( QMessageBox::Warning );
	}

	QString detailText;
	detailText += "location:\t"+location+"\n";

	QtuC::ErrorHandlerBase::errorDetails_t::const_iterator i = details.constBegin();
	while( i != details.constEnd() )
	{
		detailText += QString(i.key())+":\t"+i.value()+"\n";
		++i;
	}

	msgBox.setWindowTitle(tr("error"));
	msgBox.setText(msg);
	msgBox.setDetailedText(detailText);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setEscapeButton(QMessageBox::Ok);
	msgBox.exec();
}

void QcGuiMainView::onConnectActionTriggered()
{
	if( mModel != 0 )
	{ mModel->connectProxy(); }
}

void QcGuiMainView::onDeviceApiSet()
{
	statusBar()->showMessage( "New device API has been set.", 4000 );
}

void QcGuiMainView::initModelView()
{
	connect( mModel, SIGNAL(signalError(QtMsgType,QString,QString,QtuC::ErrorHandlerBase::errorDetails_t)), this, SLOT(showError(QtMsgType,QString,QString,QtuC::ErrorHandlerBase::errorDetails_t)) );
	connect( mModel, SIGNAL(deviceVariableCreated(const QtuC::DeviceStateVariable*,QString)), mVariableView, SLOT(showVariable(const QtuC::DeviceStateVariable*,QString)) );
	connect( mModel, SIGNAL(deviceFunctionCreated(QString,QString)), mVariableView, SLOT(showFunction(QString,QString)) );
}
