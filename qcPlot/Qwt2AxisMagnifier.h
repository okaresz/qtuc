#ifndef QWT2AXISMAGNIFIER_H
#define QWT2AXISMAGNIFIER_H

#include <qwt_plot_magnifier.h>
#include <QPoint>

class Qwt2AxisMagnifier : public QwtPlotMagnifier
{
public:
	Qwt2AxisMagnifier( QwtPlotCanvas *canvas );

private:
	void widgetMouseMoveEvent( QMouseEvent * );
	void widgetMousePressEvent( QMouseEvent *mouseEvent );
	void widgetMouseReleaseEvent( QMouseEvent *mouseEvent );

	void rescaleAxis( int axis, double factor );

	QPoint mPrevMousePos;
	bool mMousePressed;
	bool mParentHasMouseTracking;

};

#endif // QWT2AXISMAGNIFIER_H
