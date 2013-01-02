#ifndef STATEVARIABLEULONGBINVIEW_H
#define STATEVARIABLEULONGBINVIEW_H

#include "StateVariableCustomViewBase.h"
#include <QLineEdit>

namespace qcGui
{

class StateVariableUlongBinView : public StateVariableCustomViewBase
{
	Q_OBJECT
public:
	explicit StateVariableUlongBinView(QWidget *parent = 0);

	QWidget *getMainWidget();

public slots:

	void setBValue( uint newVal );

private slots:

	//void handleValueEdited( QString newStr );

signals:

	void valueChanged( uint val );

private:
	QLineEdit *mLineEditWidget;
};

}	//QcGui::

#endif // STATEVARIABLEULONGBINVIEW_H
