#ifndef STATEVARINTVIEW_H
#define STATEVARINTVIEW_H

#include "DeviceStateVariable.h"
#include <QSpinBox>

namespace qcGui
{

class StateVarIntView : public QSpinBox
{
	Q_OBJECT

enum viewType_t
{
	spinBoxWidget,
	lineEditWidget
};

public:
	explicit StateVarIntView( QWidget *parent = 0);
	
signals:

	void valueEdited(int newVal);
	
private slots:
	void onEditingFinished();

private:
};

}	//qcGui::
#endif // STATEVARINTVIEW_H
