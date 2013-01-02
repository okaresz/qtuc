#include "StateVarIntView.h"

using namespace qcGui;

StateVarIntView::StateVarIntView(QWidget *parent) :
	QSpinBox(parent)
{
	connect( this, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()) );
}

void StateVarIntView::onEditingFinished()
{
	emit valueEdited(this->value());
}
