#include "QcGuiMainWindow.h"
#include "ui_QcGuiMainWindow.h"

QcGuiMainWindow::QcGuiMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::QcGuiMainWindow)
{
	ui->setupUi(this);
}

QcGuiMainWindow::~QcGuiMainWindow()
{
	delete ui;
}
