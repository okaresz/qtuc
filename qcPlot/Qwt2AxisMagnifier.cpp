#include "Qwt2AxisMagnifier.h"
#include <qwt_plot.h>
#include <qwt_scale_div.h>
#include <QMouseEvent>

Qwt2AxisMagnifier::Qwt2AxisMagnifier( QwtPlotCanvas *canvas ):
	QwtPlotMagnifier( canvas ),
	mPrevMousePos(0,0),
	mMousePressed(false),
	mParentHasMouseTracking(false)
{}

void Qwt2AxisMagnifier::widgetMousePressEvent( QMouseEvent *mouseEvent )
{
	int mouseButton, mouseButtonState;
	getMouseButton( mouseButton, mouseButtonState );

	if ( ( mouseEvent->button() != mouseButton)
		|| parentWidget() == NULL )
	{
		return;
	}

	if ( ( mouseEvent->modifiers() & Qt::KeyboardModifierMask ) !=
		( int )( mouseButtonState & Qt::KeyboardModifierMask ) )
	{
		return;
	}

	mParentHasMouseTracking = parentWidget()->hasMouseTracking();
	parentWidget()->setMouseTracking( true );
	mPrevMousePos = mouseEvent->pos();
	mMousePressed = true;
}

void Qwt2AxisMagnifier::widgetMouseReleaseEvent( QMouseEvent *mouseEvent )
{
	Q_UNUSED( mouseEvent );

	if ( mMousePressed && parentWidget() )
	{
		mMousePressed = false;
		parentWidget()->setMouseTracking( mParentHasMouseTracking );
	}
}

void Qwt2AxisMagnifier::widgetMouseMoveEvent( QMouseEvent *mouseEvent )
{
	if ( !mMousePressed )
		return;

	// initialize pos
//	if( mPrevMousePos.isNull() )
//	{
//		mPrevMousePos = mouseEvent->pos();
//		return;
//	}

	const int dy = mouseEvent->pos().y() - mPrevMousePos.y();
	const int dx = mouseEvent->pos().x() - mPrevMousePos.x();
	double fx = mouseFactor();
	double fy = mouseFactor();

	// scale y
	if ( dy != 0 )
	{
		if ( dy > 0 )
			fy = 1 / fy;

		rescaleAxis( QwtPlot::yLeft, fy );
	}

	//scale x
	if ( dx != 0 )
	{
		if ( dx < 0 )
			fx = 1 / fx;

		rescaleAxis( QwtPlot::xBottom, fx );
	}

	mPrevMousePos = mouseEvent->pos();
}

void Qwt2AxisMagnifier::rescaleAxis(int axis, double factor)
{
	if( !isAxisEnabled(axis) )
		{ return; }

	factor = qAbs( factor );
	if( factor == 1.0 || factor == 0.0 )
		{ return; }

	bool doReplot = false;
	QwtPlot* plt = plot();

	const bool autoReplot = plt->autoReplot();
	plt->setAutoReplot( false );

	// do the rescale
	const QwtScaleDiv *scaleDiv = plt->axisScaleDiv( axis );
	if ( scaleDiv->isValid() )
	{
		const double center = scaleDiv->lowerBound() + scaleDiv->range() / 2;
		const double width_2 = scaleDiv->range() / 2 * factor;
		plt->setAxisScale( axis, center - width_2, center + width_2 );
		doReplot = true;
	}

	// set original values
	plt->setAutoReplot( autoReplot );
	if ( doReplot )
		{ plt->replot(); }
}
