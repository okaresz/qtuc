#ifndef QCGUIMAINWINDOW_H
#define QCGUIMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class QcGuiMainWindow;
}

class QcGuiMainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit QcGuiMainWindow(QWidget *parent = 0);
	~QcGuiMainWindow();
	
private:
	Ui::QcGuiMainWindow *ui;
};

#endif // QCGUIMAINWINDOW_H
