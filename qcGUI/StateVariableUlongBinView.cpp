#include "StateVariableUlongBinView.h"
#include <QBoxLayout>

using namespace qcGui;

StateVariableUlongBinView::StateVariableUlongBinView(QWidget *parent) :
	StateVariableCustomViewBase(parent)
{
	mLineEditWidget = new QLineEdit();
	setLayout( new QBoxLayout( QBoxLayout::TopToBottom, this ) );
	layout()->addWidget( mLineEditWidget );
	mLineEditWidget->setObjectName( "hwlflinebitswidget" );

	QFont font("monospace");
	font.setFixedPitch(true);
	mLineEditWidget->setFont( font );
	mLineEditWidget->show();
}

QWidget *StateVariableUlongBinView::getMainWidget()
{
	return mLineEditWidget;
}

void StateVariableUlongBinView::setBValue( uint newVal )
{
	mLineEditWidget->setText( QString::number(newVal, 2).rightJustified(32,'0') );
}
/*
void StateVariableUlongBinView::handleValueEdited(QString newStr)
{

}
*/
