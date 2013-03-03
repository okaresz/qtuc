#include "PlotterView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>

using namespace qcPlot;

PlotterView::PlotterView( Plotter *model, QWidget *parent) :
	QWidget(parent),
	mModel(model)
{
	createGui();
	initModelView();
}

void PlotterView::on_plotterView_toolBar_clearButton_clicked()
{
	if( QMessageBox::Yes == QMessageBox::question(this, "Are you sure?", "Are you sure to clear the plot?", QMessageBox::Yes, QMessageBox::No ) )
	{
		mModel->clear();
		qDebug("cleared");
	}
}

void PlotterView::on_plotterView_toolBar_saveButton_clicked()
{
}

void PlotterView::on_plotterView_toolBar_startPauseButton_toggled(bool checked)
{
	QPushButton *btn = (QPushButton*)sender();
	if( checked )
	{
		btn->setText( "running" );
		mModel->start();
	}
	else
	{
		btn->setText( "paused" );
		mModel->stop();
	}
}

void PlotterView::on_plotterView_toolBar_autoZoomButton_toggled( bool checked )
{
	mPlot->setAutoZoom( checked );
}

void PlotterView::on_plotterView_toolBar_autoFollowButton_toggled( bool checked )
{
	mPlot->setAutoFollow( checked );
}

void PlotterView::onModelReset()
{
}

void PlotterView::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout();
	setLayout(layout);

	mPlot = new PlotView(mModel, this);
	layout->addWidget(mPlot);

	QHBoxLayout *plotToolBar = new QHBoxLayout();
	layout->addLayout(plotToolBar);

	QPushButton *saveButton = new QPushButton(this);
	saveButton->setText("save");
	saveButton->setObjectName( "plotterView_toolBar_saveButton" );
	plotToolBar->addWidget(saveButton);

	plotToolBar->addStretch();

	QPushButton *autoZoomButton = new QPushButton(this);
	autoZoomButton->setText("aZoom");
	autoZoomButton->setObjectName( "plotterView_toolBar_autoZoomButton" );
	autoZoomButton->setCheckable(true);
	autoZoomButton->setChecked( mPlot->getAutoZoom() );
	plotToolBar->addWidget(autoZoomButton);

	QPushButton *autoFollowButton = new QPushButton(this);
	autoFollowButton->setText("aFollow");
	autoFollowButton->setObjectName( "plotterView_toolBar_autoFollowButton" );
	autoFollowButton->setCheckable(true);
	autoFollowButton->setChecked( mPlot->getAutoFollow() );
	plotToolBar->addWidget(autoFollowButton);

	plotToolBar->addStretch();

	QPushButton *clearButton = new QPushButton(this);
	clearButton->setText("clear");
	clearButton->setObjectName( "plotterView_toolBar_clearButton" );
	plotToolBar->addWidget(clearButton);

	QPushButton *startPauseButton = new QPushButton(this);
	startPauseButton->setText("running");
	startPauseButton->setObjectName( "plotterView_toolBar_startPauseButton" );
	startPauseButton->setCheckable(true);
	startPauseButton->setChecked(true);
	plotToolBar->addWidget(startPauseButton);

	QMetaObject::connectSlotsByName(this);
}

void PlotterView::initModelView()
{
	connect( mModel, SIGNAL(hadReset()), this, SLOT(onModelReset()) );
	mPlot->setConfig(mModel->cfg());
}
