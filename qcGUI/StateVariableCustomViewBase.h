#ifndef STATEVARIABLECUSTOMVIEWBASE_H
#define STATEVARIABLECUSTOMVIEWBASE_H

#include <QWidget>

namespace qcGui
{

class StateVariableCustomViewBase : public QWidget
{
	Q_OBJECT
public:
	explicit StateVariableCustomViewBase(QWidget *parent = 0);

public:

	virtual QWidget *getMainWidget() = 0;
	
};

}	//QcGui::

#endif // STATEVARIABLECUSTOMVIEWBASE_H
