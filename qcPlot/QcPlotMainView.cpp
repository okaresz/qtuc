#include "QcPlotMainView.h"
#include "ErrorHandlerBase.h"
#include <QStatusBar>
#include <QMessageBox>
#include <QStringList>
#include "PlotSettingsManager.h"
#include "PlotConfigView.h"
#include "PlotView.h"
#include <QDockWidget>

using namespace qcPlot;

QcPlotMainView::QcPlotMainView(QcPlot *model) :
	QMainWindow(0),
	mModel(model)
{
	createGui();
	initModelView();
}

QcPlotMainView::~QcPlotMainView()
{
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

void QcPlotMainView::createGui()
{
	mMainToolBar = new QToolBar(this);

	QAction *connectAction = new QAction( QIcon(""), tr("Connect"), this );
	connectAction->setObjectName( QString::fromUtf8( "connectAction" ) );
	connectAction->setToolTip( tr("connect to proxy") );
	mActions.insert( "connect", connectAction );
	connect( connectAction, SIGNAL(triggered()), this, SLOT(onConnectActionTriggered()) );
	mMainToolBar->addAction(connectAction);

	QAction *newPlotAction = new QAction( QIcon(""), tr("New plot"), this );
	newPlotAction->setObjectName( QString::fromUtf8( "newPlotAction" ) );
	newPlotAction->setToolTip( tr("create new plot") );
	mActions.insert( "newPlot", newPlotAction );
	connect( newPlotAction, SIGNAL(triggered()), this, SLOT(onNewPlotActionTriggered()) );
	newPlotAction->setEnabled(false);
	mMainToolBar->addAction(newPlotAction);

	setDockOptions( QMainWindow::AnimatedDocks );

	addToolBar( Qt::TopToolBarArea, mMainToolBar );
	setStatusBar( new QStatusBar(this) );

	QMetaObject::connectSlotsByName(this);
}

void QcPlotMainView::showError(QtMsgType severity, QString msg, QString location, const QtuC::ErrorHandlerBase::errorDetails_t details)
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

void QcPlotMainView::onConnectActionTriggered()
{
	if( mModel != 0 )
		{ mModel->connectProxy(); }
}

void QcPlotMainView::onNewPlotActionTriggered()
{
	PlotConfig *plotCfg = new PlotConfig();
	PlotConfigView *cfgView = new PlotConfigView();
	cfgView->setModel(plotCfg);
	cfgView->setWindowTitle( "Create new plot" );
	cfgView->curveCfgView()->fillStateVariableList( mModel->getVariableList() );
	connect( cfgView, SIGNAL(submit()), this, SLOT(onNewPlotDialogSubmit()) );
	cfgView->show();
}

void QcPlotMainView::onNewPlotDialogSubmit()
{
	PlotConfigView *configView = (PlotConfigView*)sender();
	PlotConfig *plotCfg = configView->model();
	configView->setAttribute( Qt::WA_DeleteOnClose );
	configView->close();
	mModel->addNewPlot(plotCfg);
}

void QcPlotMainView::onProxyConnected()
{
	mActions.value("connect")->setEnabled(false);
}

void QcPlotMainView::onDeviceApiSet()
{
	mActions.value("newPlot")->setEnabled(true);
	statusBar()->showMessage( "New device API has been set.", 4000 );
}

void QcPlotMainView::onNewPlotterAdded(Plotter *plotter)
{
	PlotView *plot = new PlotView( plotter, this );
	QDockWidget *plotDock = new QDockWidget( plotter->cfg()->name(), this );
	plotDock->setAllowedAreas( Qt::LeftDockWidgetArea );
	plotDock->setWidget( plot );

	addDockWidget( Qt::LeftDockWidgetArea, plotDock );
}

void QcPlotMainView::initModelView()
{
	connect( mModel, SIGNAL(signalError(QtMsgType,QString,QString,QtuC::ErrorHandlerBase::errorDetails_t)), this, SLOT(showError(QtMsgType,QString,QString,QtuC::ErrorHandlerBase::errorDetails_t)) );
	connect( mModel->plotManager(), SIGNAL(newPlotterAdded(Plotter*)), this, SLOT(onNewPlotterAdded(Plotter*)) );
	connect( mModel, SIGNAL(proxyHasConnected()), this, SLOT(onProxyConnected()) );
	connect( mModel, SIGNAL(deviceApiSet()), this, SLOT(onDeviceApiSet()) );
}
