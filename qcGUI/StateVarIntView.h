#ifndef STATEVARINTVIEW_H
#define STATEVARINTVIEW_H

#include <QObject>

namespace qcGui
{

class StateVarIntView : public QObject
{
	Q_OBJECT

enum viewType_t
{
	spinBoxWidget,
	lineEditWidget
};

public:
	explicit StateVarIntView(QObject *parent = 0);
	
signals:
	
public slots:
	
};

}	//qcGui::
#endif // STATEVARINTVIEW_H
