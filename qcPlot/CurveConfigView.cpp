#include "CurveConfigView.h"
#include "ErrorHandlerBase.h"
#include <QLabel>
#include <QListWidget>
#include <QFont>

using namespace qcPlot;

CurveConfigView::CurveConfigView(QWidget *parent) :
	QWidget(parent),
	mModel(0),
	mMainLayout(0),
	mNameLineEdit(0),
	mVariableComboBox(0)
{
	createGui();
}

CurveConfigView::~CurveConfigView()
{}

void CurveConfigView::setModel(CurveConfig *model)
{
	if( model )
	{
		if( mModel )
			{ mModel->disconnect(this); }
		setEnabled(false);
		mModel = model;
		syncToModel();
		connectViewModel();
		setEnabled(true);
	}
	else
	{
		setEnabled(false);
	}
}

void CurveConfigView::fillStateVariableList(QHash<QString, QStringList> list)
{
	if( list.isEmpty() )
		{ return; }

	// helper list widget for the combobox
	QListWidget *listWidget = new QListWidget(this);

	QHash<QString, QStringList>::const_iterator hwiItem = list.constBegin();
	while( hwiItem != list.constEnd() )
	{
		if( hwiItem.value().isEmpty() )
		{
			++hwiItem;
			continue;
		}

		QListWidgetItem *listItem = new QListWidgetItem( listWidget );
		listItem->setText( hwiItem.key() );
		listItem->setFlags( Qt::NoItemFlags );
		QFont boldFont;
		boldFont.setBold(true);
		listItem->setFont( boldFont );
		QStringList itemData = QStringList() << hwiItem.key();
		listItem->setData( Qt::UserRole, itemData );

		for( int i=0; i<hwiItem.value().size(); i++ )
		{
			QListWidgetItem *listItem = new QListWidgetItem( listWidget );
			listItem->setText( "   "+hwiItem.value().at(i) );
			itemData = QStringList() << hwiItem.key() << hwiItem.value().at(i);
			listItem->setData( Qt::UserRole, itemData );
		}
		++hwiItem;
	}

	mVariableComboBox->setModel( listWidget->model() );
	mVariableComboBox->setView( listWidget );
}

void CurveConfigView::syncToModel()
{
	if( !mModel )
	{
		QtuC::ErrorHandlerBase::error( QtWarningMsg, "Model is null.", "syncToModel()", "CurveConfigView" );
		return;
	}

	mNameLineEdit->setText( mModel->name() );

	int index = mVariableComboBox->model()->rowCount();
	while( index-- )
	{
		QStringList itemData = mVariableComboBox->model()->data( mVariableComboBox->model()->index(index,0), Qt::UserRole ).toStringList();
		if( itemData.size() == 2 &&
			itemData.at(0) == mModel->stateVariable().first &&
			itemData.at(1) == mModel->stateVariable().second )
		{ break; }
	}
	mVariableComboBox->setCurrentIndex(index);
}

void CurveConfigView::createGui()
{
	// Create main layout
	mMainLayout = new QFormLayout();
	setLayout( mMainLayout );
	setEnabled(false);

	// Create widgets
	mNameLineEdit = new QLineEdit(this);
	mNameLineEdit->setObjectName( "nameLineEdit" );
	mMainLayout->addRow( "name", mNameLineEdit );

	mVariableComboBox = new QComboBox(this);
	mVariableComboBox->setObjectName("variableComboBox");
	mMainLayout->addRow( "variable", mVariableComboBox );

	QMetaObject::connectSlotsByName(this);
}

void CurveConfigView::connectViewModel()
{
}

void CurveConfigView::on_nameLineEdit_textEdited(const QString &text)
{
	mModel->setName(text);
}

void CurveConfigView::on_variableComboBox_activated(int index)
{
	QStringList itemData = mVariableComboBox->itemData( index ).toStringList();
	mModel->setStateVariable( itemData.at(0), itemData.at(1) );
}
