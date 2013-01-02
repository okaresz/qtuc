#include "StateVariablesView.h"
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include "ErrorHandlerBase.h"
#include <QLabel>
#include <QPushButton>
#include "StateVariableUlongBinView.h"
#include <QSlider>
#include "StateVarIntView.h"

using namespace qcGui;

StateVariablesView::StateVariablesView(QWidget *parent) :
	QWidget(parent),
	mModel(0)
{
	mLayout = new QGridLayout();
	setLayout(mLayout);
}

bool StateVariablesView::setModel(QcGui *model)
{
	if( !mModel )
	{
		mModel = model;
		return true;
	}
	else
	{
		QtuC::ErrorHandlerBase::error( QtWarningMsg, "Model is already set", "setModel()", "StateVariablesView" );
		return false;
	}
}

void StateVariablesView::showVariable(const QtuC::DeviceStateVariable *newVar, const QString &guiHint)
{
	QGroupBox *hwiGroup = getHwiGroup(newVar->getHwInterface());
	if( !hwiGroup )
		{ hwiGroup = createHwiGroup( newVar->getHwInterface() ); }

	QWidget *varWidget = createVariableWidget( newVar, guiHint );
	if( !varWidget )
		{ QtuC::ErrorHandlerBase::error( QtWarningMsg, "Variable skipped", "showVariable()", "StateVariablesView" ); }
	else
	{
		//QLayout::addWidget() does this for me
		//varWidget->setParent(hwiGroup);

		QLabel *label = new QLabel(hwiGroup);
		label->setText( newVar->getName() );

		QGridLayout *groupLayout = (QGridLayout*)hwiGroup->layout();
		int widgetRow = groupLayout->rowCount();
		groupLayout->addWidget( label, widgetRow, 0 );
		groupLayout->addWidget( varWidget, widgetRow, 1 );

		label->show();
		varWidget->show();
	}
}

/// @todo Seriously improve function handling (call width editable args)
void StateVariablesView::showFunction(const QString &hwInterface, const QString &name)
{
	QGroupBox *hwiGroup = getHwiGroup(hwInterface);
	if( !hwiGroup )
		{ hwiGroup = createHwiGroup( hwInterface ); }

	QPushButton *varWidget = new QPushButton(hwiGroup);
	varWidget->setObjectName(name+"Widget");
	varWidget->setText( "call" );
	varWidget->setProperty( "hwi", hwInterface );
	varWidget->setProperty( "name", name );
	connect( varWidget, SIGNAL(clicked()), this, SLOT(functionButtonClicked()) );

	QLabel *label = new QLabel(hwiGroup);
	label->setText( name );

	QGridLayout *groupLayout = (QGridLayout*)hwiGroup->layout();
	int widgetRow = groupLayout->rowCount();
	groupLayout->addWidget( label, widgetRow, 0 );
	groupLayout->addWidget( varWidget, widgetRow, 1 );

	label->show();
	varWidget->show();
}

QGroupBox *StateVariablesView::getHwiGroup(const QString &hwiName)
{
	for( int i=0; i<mHwInterfaceGroups.size(); ++i )
	{
		if( mHwInterfaceGroups.at(i)->objectName() == hwiName )
			{ return mHwInterfaceGroups.at(i); }
	}
	return 0;
}

QGroupBox *StateVariablesView::createHwiGroup(const QString &hwiName)
{
	int maxRowCount = 3;
	QGroupBox * group = new QGroupBox();
	group->setObjectName(hwiName);
	group->setLayout(new QGridLayout);
	group->setTitle(hwiName);
	mHwInterfaceGroups.append(group);
	mLayout->addWidget(group, mLayout->rowCount(), 0);
	group->show();
	return group;
}

QWidget *StateVariablesView::createVariableWidget(const QtuC::DeviceStateVariable *var, const QString &guiHint)
{
	QWidget *customWidget = createCustomVariableWidget(var);
	if( customWidget )
		{ return customWidget; }

	QString widgetName( var->getName()+"Widget" );
	switch( var->getType() )
	{
		case QVariant::Double:
		case QVariant::String:
		{
			QLineEdit *lineEdit = new QLineEdit();
			lineEdit->setObjectName( widgetName );
			lineEdit->setText( var->getValue().toString() );
			if( var->getAccessMode() == DeviceStateVariable::readAccess )
			{
				lineEdit->setDisabled(true);
				connect( var, SIGNAL(valueChanged(QString)), lineEdit, SLOT(setText(QString)) );
			}
			else if( var->getAccessMode() == DeviceStateVariable::writeAccess )
				{ connect( lineEdit, SIGNAL(textEdited(QString)), var, SLOT(setValue(QString)) ); }
			return lineEdit;
		} break;
		case QVariant::Bool:
		{
			QCheckBox *checkBox = new QCheckBox();
			checkBox->setObjectName( widgetName );
			checkBox->setChecked( var->getValue().toBool() );
			if( var->getAccessMode() == DeviceStateVariable::readAccess )
			{
				checkBox->setDisabled(true);
				connect( var, SIGNAL(valueChanged(bool)), checkBox, SLOT(setChecked(bool)) );
			}
			else if( var->getAccessMode() == DeviceStateVariable::writeAccess )
				{ connect( checkBox, SIGNAL(clicked(bool)), var, SLOT(setValue(bool)) ); }
			return checkBox;
		} break;
		case QVariant::Int:
		case QVariant::UInt:
		{
			if( guiHint == "slider" )
			{
				QSlider *slider = new QSlider();
				slider->setObjectName( widgetName );
				slider->setMinimum(0);
				slider->setMaximum(65535);
				slider->setSingleStep(10);
				slider->setOrientation(Qt::Horizontal);
				slider->setTracking(true);
				slider->setValue( var->getValue().toInt() );
				if( var->getAccessMode() == DeviceStateVariable::readAccess )
				{
					slider->setDisabled(true);
					connect( var, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)) );
				}
				else if( var->getAccessMode() == DeviceStateVariable::writeAccess )
					{ connect( slider, SIGNAL(valueChanged(int)), var, SLOT(setValue(int)) ); }
				return slider;
			}
			else
			{
				StateVarIntView *spinBox = new StateVarIntView();
				spinBox->setObjectName( widgetName );
				spinBox->setMaximum(65535);
				spinBox->setSingleStep(1);
				spinBox->setValue( var->getValue().toInt() );
				if( var->getAccessMode() == DeviceStateVariable::readAccess )
				{
					spinBox->setDisabled(true);
					connect( var, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)) );
				}
				else if( var->getAccessMode() == DeviceStateVariable::writeAccess )
					{ connect( spinBox, SIGNAL(valueEdited(int)), var, SLOT(setValue(int)) ); }
				return spinBox;
			}
		} break;
		default:
			QtuC::ErrorHandlerBase::error( QtWarningMsg, QString("No matching widget for variable %1 (%2)").arg(var->getName(),QVariant::typeToName(var->getType())), "createVariableWidget()", "StateVariablesView()" );
	}
	return 0;
}

QWidget *StateVariablesView::createCustomVariableWidget(const DeviceStateVariable *var)
{
	if( var->getHwInterface() == "hwlf" && ( var->getName() == "lineBitsL" || var->getName() == "lineBitsH" || var->getName() == "chBits") )
	{
		StateVariableUlongBinView *lineBitsView = new StateVariableUlongBinView();

		lineBitsView->setBValue( var->getValue().toUInt() );

		//lineBitsView->setDisabled(true);
		connect( var, SIGNAL(valueChanged(uint)), lineBitsView, SLOT(setBValue(uint)) );

		return lineBitsView;
	}
	else
		return 0;
}

void StateVariablesView::clearApiGui()
{
	for( int i=0; i<mHwInterfaceGroups.size(); ++i )
	{
		mLayout->removeWidget( mHwInterfaceGroups.at(i) );
		mHwInterfaceGroups.at(i)->deleteLater();
	}
	mHwInterfaceGroups.clear();
}

void StateVariablesView::functionButtonClicked()
{
	if( mModel )
	{
		QObject *button = sender();
		if( !mModel->callDeviceFunction( button->property("hwi").toString(), button->property("name").toString() ) )
		{
			QtuC::ErrorHandlerBase::errorDetails_t errDet;
			errDet.insert( "hwInterface", button->property("hwi").toString() );
			errDet.insert( "name", button->property("name").toString() );
			QtuC::ErrorHandlerBase::error( QtWarningMsg, "Function call failed", "functionButtonClicked()", "StateVariablesView()", errDet );
		}
	}
	else
		{ QtuC::ErrorHandlerBase::error( QtWarningMsg, "No model set, cannot call function", "functionButtonClicked()", "StateVariablesView()" ); }
}
