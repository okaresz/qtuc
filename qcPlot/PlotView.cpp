#include "PlotView.h"
#include <qwt_plot_panner.h>
#include <qwt_plot_canvas.h>
#include <qwt_legend.h>
#include <qwt_plot_picker.h>
#include "Qwt2AxisMagnifier.h"
#include <QDebug>
#include <qwt_symbol.h>

using namespace qcPlot;

PlotView::PlotView( Plotter *model, QWidget *parent ):
	QwtPlot( parent ),
	mModel(model),
	mAutoZoom(true),
	mAutoFollow(true)
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
	if( mAutoZoom )
	{
		QPair<double, double> valLimits = ((DeviceStateHistoryVariable*)sender())->getValueLimits();
		setAxisScale(yLeft, valLimits.first, valLimits.second, axisStepSize(yLeft) );
	}

	if( mAutoFollow )
	{
		QPair<qint64, qint64> timeLimits = ((DeviceStateHistoryVariable*)sender())->getTimestampLimits();
		double lowerTimeLim = timeLimits.second-axisScaleDiv(xBottom)->range();
		setAxisScale(xBottom, lowerTimeLim, timeLimits.second, axisStepSize(xBottom) );
	}

	/// @todo why is this necessary? QwtPlot should replot itself on new data. Maybe a special signal must be emitted by data object?...
	replot();
}

void PlotView::setConfig(PlotConfig const *config)
{
	/// @todo I can't disconnect the config slots from me, because it's const. It's his responsibility. Is this an issue?...
	//disconnect(mConfig);

	setTitle( config->name() );

	//create/set curves
	QList<CurveConfig*> curveCfgList = config->getCurves();
	for( int i=0; i<curveCfgList.size(); ++i )
	{
		QwtPlotCurve *plotCurve = curve(curveCfgList.at(i)->id());
		if( !plotCurve )
		{
			plotCurve = new QwtPlotCurve();
			DeviceStatePlotDataVariable *plotVar = mModel->resolveStateVar( curveCfgList.at(i)->stateVariable() );
			if( plotVar )
			{
				plotCurve->setData( plotVar );
				mCurves.append( QPair<uint,QwtPlotCurve*>( curveCfgList.at(i)->id(), plotCurve ) );
				plotCurve->attach(this);
				connect( plotVar, SIGNAL(historyUpdated()), this, SLOT(onDataChanged()) );
			}
			else
				{ qWarning()<<QString("no variable vound: hwi=%1, name=%2").arg(curveCfgList.at(i)->stateVariable().first, curveCfgList.at(i)->stateVariable().second); }

			plotCurve->setRenderHint( QwtPlotItem::RenderAntialiased );
			plotCurve->setLegendAttribute( QwtPlotCurve::LegendShowLine, true );

			QwtSymbol *curveSymbol = new QwtSymbol(QwtSymbol::Ellipse);
			curveSymbol->setColor( QColor(160,160,160));
			curveSymbol->setSize( 3, 3 );
			plotCurve->setSymbol( curveSymbol );
		}

		plotCurve->setTitle( curveCfgList.at(i)->name() );
	}

}

QwtPlotCurve *PlotView::curve(uint curveId)
{
	for( unsigned short int i=0; i<mCurves.size(); ++i )
	{
		if( mCurves.at(i).first == curveId )
			{ return mCurves.at(i).second; }
	}
	return 0;
}
