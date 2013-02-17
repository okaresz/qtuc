#ifndef CURVECONFIGVIEW_H
#define CURVECONFIGVIEW_H

#include <QWidget>
#include "CurveConfig.h"
#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHash>
#include <QComboBox>

namespace qcPlot
{

class CurveConfigView : public QWidget
{
	Q_OBJECT
public:
	explicit CurveConfigView(QWidget *parent = 0);
	~CurveConfigView();

	void setModel( CurveConfig *model );

	inline CurveConfig *model() const
		{ return mModel; }

	void fillStateVariableList( QHash<QString, QStringList> list );

public slots:
	void syncToModel();

private slots:
	void on_nameLineEdit_textEdited( const QString &text );
	void on_variableComboBox_activated( int index );

private:
	void createGui();
	void connectViewModel();

	CurveConfig *mModel;

	QFormLayout * mMainLayout;
	QLineEdit *mNameLineEdit;
	QComboBox *mVariableComboBox;
};

}	//qcPlot::
#endif // CURVECONFIGVIEW_H
