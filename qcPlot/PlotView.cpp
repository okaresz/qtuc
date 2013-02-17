#include "PlotView.h"
#include <qwt_plot_panner.h>
#include <qwt_plot_canvas.h>
#include <qwt_legend.h>
#include <qwt_plot_picker.h>
#include "Qwt2AxisMagnifier.h"

using namespace qcPlot;

PlotView::PlotView( Plotter *model, QWidget *parent ):
	QwtPlot( parent ),
	mModel(model)
{
	// panning with the left mouse button
	( void ) new QwtPlotPanner( canvas() );

	// zoom in/out with the wheel
	Qwt2AxisMagnifier *magnifier = new Qwt2AxisMagnifier( canvas() );
	magnifier->setAxisEnabled( yLeft, true );
	magnifier->setAxisEnabled( xBottom, true );

	//Picker
	QwtPlotPicker *picker = new QwtPlotPicker( canvas() );
	picker->setTrackerMode(QwtPicker::AlwaysOn);

	setTitle( mModel->cfg()->name() );
	insertLegend( new QwtLegend(), QwtPlot::RightLegend );

	setAutoReplot(false);

	// canvas
	canvas()->setLineWidth( 1 );
	canvas()->setFrameStyle( QFrame::Box | QFrame::Plain );

	QPalette canvasPalette( Qt::white );
	canvasPalette.setColor( QPalette::Foreground, QColor( 133, 190, 232 ) );
	canvas()->setPalette( canvasPalette );

#if defined(Q_WS_X11)
	// Even if not recommended by TrollTech, Qt::WA_PaintOutsidePaintEvent
	// works on X11. This has a nice effect on the performance.

	canvas()->setAttribute( Qt::WA_PaintOutsidePaintEvent, true );

	// Disabling the backing store of Qt improves the performance
	// for the direct painter even more, but the canvas becomes
	// a native window of the window system, receiving paint events
	// for resize and expose operations. Those might be expensive
	// when there are many points and the backing store of
	// the canvas is disabled. So in this application
	// we better don't both backing stores.

	if ( canvas()->testPaintAttribute( QwtPlotCanvas::BackingStore ) )
	{
		canvas()->setAttribute( Qt::WA_PaintOnScreen, true );
		canvas()->setAttribute( Qt::WA_NoSystemBackground, true );
	}

#endif

}

void PlotView::onDataChanged()
{
//	double upperLim = ((PlotData*)sender())->getMaxX();
//	double lowerLim = upperLim-axisScaleDiv(xBottom)->range();
//	setAxisScale(xBottom, lowerLim, upperLim, axisStepSize(xBottom) );
//	replot();
}

void PlotView::setConfig(PlotConfig *config)
{
}
