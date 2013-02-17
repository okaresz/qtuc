#include "PlotConfigView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QFrame>
#include <QLabel>
#include <QIcon>

using namespace qcPlot;

PlotConfigView::PlotConfigView(QWidget *parent) :
	QWidget(parent),
	mModel(0),
	mNameLineEdit(0),
	mCurveListWidget(0),
	mCurveConfig(0)
{
	createGui();
}

void PlotConfigView::setModel( PlotConfig *model )
{
	if( mModel )
		{ mModel->disconnect(this); }
	setEnabled(false);
	mModel = model;
	syncToModel();
	connectViewModel();
	setEnabled(true);
}

void PlotConfigView::syncToModel()
{
	mNameLineEdit->setText( mModel->name() );
}

void PlotConfigView::createGui()
{
	QHBoxLayout *hLayout = new QHBoxLayout();

	QGroupBox *plotCfgBox = new QGroupBox();
	createGui_plotCfg( plotCfgBox );
	hLayout->addWidget( plotCfgBox );

	QGroupBox *curveCfgBox = new QGroupBox();
	createGui_curveCfg( curveCfgBox );
	hLayout->addWidget( curveCfgBox );

	QVBoxLayout *vLayout = new QVBoxLayout();
	vLayout->addLayout( hLayout );

	// Add OK and Cancel button
	QPushButton *okButton = new QPushButton( "Okay", this );
	connect( okButton, SIGNAL(clicked()), this, SIGNAL(submit()) );

	QPushButton *cancelButton = new QPushButton( "Cancel", this );
	connect( cancelButton, SIGNAL(clicked()), this, SLOT(deleteLater()) );

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->addWidget( okButton );
	buttonLayout->addWidget( cancelButton );

	vLayout->addLayout( buttonLayout );

	setLayout( vLayout );

	QMetaObject::connectSlotsByName(this);
}

void PlotConfigView::createGui_plotCfg(QGroupBox *groupBox)
{
	groupBox->setTitle( "Plot settings" );

	QFormLayout *formlayout = new QFormLayout();

	mNameLineEdit = new QLineEdit(this);
	mNameLineEdit->setObjectName("plotNameLineEdit");
	formlayout->addRow( "name", mNameLineEdit );

	groupBox->setLayout( formlayout );
}

void PlotConfigView::createGui_curveCfg(QGroupBox *groupBox)
{
	groupBox->setTitle( "Curve settings" );

	QHBoxLayout *layout = new QHBoxLayout();

	// Curve Settings ----------------------------------
	QFrame *curveSettingsFrame = new QFrame();

	mCurveConfig = new CurveConfigView(this);
	curveSettingsFrame->setLayout( new QVBoxLayout() );
	curveSettingsFrame->layout()->addWidget( mCurveConfig );

	layout->addWidget( curveSettingsFrame );

	// Curve List ----------------------------------

	QVBoxLayout *curveListLayout = new QVBoxLayout();
	layout->addLayout( curveListLayout );

	QLabel *curveListTitle = new QLabel("Curve list");
	curveListLayout->addWidget( curveListTitle );

	mCurveListWidget = new QListWidget();
	mCurveListWidget->setSelectionMode( QAbstractItemView::SingleSelection );
	connect( mCurveListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onCurveSelectionChanged()) );
	curveListLayout->addWidget( mCurveListWidget);

	QHBoxLayout *curveListTools = new QHBoxLayout();
	curveListLayout->addLayout( curveListTools );
	curveListTools->addStretch();

	QPushButton *newCurveButton = new QPushButton();
	newCurveButton->setIcon( QIcon::fromTheme("contact-new") );
	connect( newCurveButton, SIGNAL(clicked()), this, SLOT(createNewCurve()) );
	curveListTools->addWidget( newCurveButton );

	QPushButton *deleteCurveButton = new QPushButton();
	deleteCurveButton->setIcon( QIcon::fromTheme("edit-delete") );
	connect( deleteCurveButton, SIGNAL(clicked()), this, SLOT(onCurveDeleteButtonClicked()) );
	curveListTools->addWidget( deleteCurveButton );

	groupBox->setLayout( layout );
}

void PlotConfigView::connectViewModel()
{

}

void PlotConfigView::createNewCurve( const QString &name )
{
	CurveConfig *curveModel = mModel->createCurve( name );
	ListWidgetCurveItem *curveItem = new ListWidgetCurveItem( curveModel->name() );
	curveItem->setData( curveIdRole, QVariant( curveModel->id() ) );
	connect( curveModel, SIGNAL(nameChanged(QString)), curveItem, SLOT(updateText(QString)) );
	mCurveListWidget->addItem( curveItem );
}

void PlotConfigView::onCurveSelectionChanged()
{
	CurveConfig *curveCfg = mModel->curve( mCurveListWidget->currentItem()->data( curveIdRole ).toUInt() );
	if( curveCfg )
		{ mCurveConfig->setModel( curveCfg ); }
}

void PlotConfigView::onCurveDeleteButtonClicked()
{
	QListWidgetItem *currentCurveItem = mCurveListWidget->currentItem();
	CurveConfig *curveModel = mModel->curve( currentCurveItem->data( curveIdRole ).toUInt() );
	QMessageBox::StandardButton buttonClicked = QMessageBox::question( this, "Delete curve", QString("Delete %1?").arg(curveModel->name()), QMessageBox::Yes | QMessageBox::No );

	if( buttonClicked == QMessageBox::Yes )
	{
		mCurveListWidget->takeItem( mCurveListWidget->row(currentCurveItem) );
		mModel->deleteCurve( curveModel->id() );
	}
}

void PlotConfigView::on_plotNameLineEdit_textEdited(const QString &text)
{
	mModel->setName( text );
}
