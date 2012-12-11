#ifndef STATEVARIABLESVIEW_H
#define STATEVARIABLESVIEW_H

#include <QWidget>
#include <QGridLayout>
#include <QGroupBox>
#include <QList>
#include "DeviceStateVariable.h"
#include "QcGui.h"

namespace qcGui
{

/** View class for the device variables.
  *	StateVariablesView is responsible for displaying the device state variables, update the view and signel if they are changed.*/
class StateVariablesView : public QWidget
{
	Q_OBJECT

public:
	explicit StateVariablesView(QWidget *parent = 0);

public slots:

	/** Set model pointer,
	  *	Set the model and do the necessary initializations (eg. connect())
	  *	@param model The model pointer to set.
	  *	@return True on success, false otherwise.*/
	bool setModel( QcGui *model );

	/** Show variable on the GUI.
	  *	Create and initialize the necessary GUI components to display the variable.
	  *	@param newVar The variable to show.*/
	void showVariable( const QtuC::DeviceStateVariable *newVar );

	/** Show function on the GUI.
	  *	Create and initialize the necessary GUI components to display the function.
	  *	@param hwInterface The hardware interface of the function.
	  *	@param hwInterface The name of the function.*/
	void showFunction( const QString &hwInterface, const QString &name );

	/** Clear GUI.
	  *	Destroy entire GUI.*/
	void clearApiGui();

private slots:
	void functionButtonClicked();

private:
	/** Get the groupBox of the passed hardware interface.
	  *	@param hwiName Name of the hardware interface.
	  *	@return The QGroupBox of the requested hardware interface.*/
	QGroupBox* getHwiGroup( const QString &hwiName );

	/** Create a QGroupBox for a hardware interface.
	  *	@param hwiName Name of the hardware interface.
	  *	@return Pointer to the created QGroupBox. The returned box is already displayed, and appended to the layout.*/
	QGroupBox* createHwiGroup( const QString &hwiName );

	/** Create a widget for the device variable.
	*	Create the appropriate widget for the passed device state variable, and connect the necessary signals/slots.
	*	@param var The device state variable for the widget.
	*	@return The new widget for the device state variable.*/
	QWidget *createVariableWidget( const QtuC::DeviceStateVariable *var );

	QcGui *mModel;		///< Model for this view.
	QGridLayout *mLayout;	///< Layout for StateVariablesView
	QList<QGroupBox*> mHwInterfaceGroups;	///< List of the hardware interface groups.
	
};

}	//qcGui::
#endif // STATEVARIABLESVIEW_H
