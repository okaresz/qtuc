#ifndef PLOTCONFIGVIEW_H
#define PLOTCONFIGVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QListWidget>
#include <QLineEdit>
#include "PlotConfig.h"
#include "CurveConfigView.h"

namespace qcPlot
{

class PlotConfigView : public QWidget
{
	Q_OBJECT
public:
	enum curveCustomRoles_t {
		curveIdRole = Qt::UserRole
	};

	explicit PlotConfigView(QWidget *parent = 0);

	void setModel( PlotConfig *model );

	inline PlotConfig *model() const
		{ return mModel; }

	inline CurveConfigView *curveCfgView()
		{ return mCurveConfig; }

public slots:
	void syncToModel();
	void createNewCurve( QString const &name = QString() );

signals:
	void submit();

private slots:
	void onCurveSelectionChanged();
	void onCurveDeleteButtonClicked();
	void on_plotNameLineEdit_textEdited( const QString &text );

private:
	void createGui();
	void createGui_plotCfg( QGroupBox *groupBox );
	void createGui_curveCfg( QGroupBox *groupBox );
	void connectViewModel();

	PlotConfig *mModel;

	QLineEdit *mNameLineEdit;

	QListWidget *mCurveListWidget;
	class ListWidgetCurveItem;
	CurveConfigView *mCurveConfig;
};

class PlotConfigView::ListWidgetCurveItem : public QObject, public QListWidgetItem
{
	Q_OBJECT
public:
	ListWidgetCurveItem( QString const &text ) : QObject(0), QListWidgetItem(text)
	{}

public slots:
	void updateText(const QString &text)
		{ setText(text); }
};

}	//qcPlot::
#endif // PLOTCONFIGVIEW_H
