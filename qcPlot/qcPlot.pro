#-------------------------------------------------
#
# Project created by QtCreator 2012-11-02T01:13:06
#
#-------------------------------------------------

QT       += core gui network xml

TARGET = qcPlot
TEMPLATE = app

SOURCES += main.cpp\
    QcPlot.cpp \
    ProxyStateManager.cpp \
    ProxyConnectionManager.cpp \
    PlotSettingsManager.cpp \
    QcPlotMainView.cpp \
    DeviceStateHistoryVariable.cpp \
    DeviceStatePlotDataVariable.cpp \
    PlotConfig.cpp \
    CurveConfig.cpp \
    CurveConfigView.cpp \
    PlotConfigView.cpp \
    PlotManager.cpp \
    Plotter.cpp \
    PlotView.cpp \
    CurveView.cpp \
    Qwt2AxisMagnifier.cpp \
    PlotterView.cpp \
    QcPlotCurve.cpp

HEADERS  += \
    QcPlot.h \
    ProxyStateManager.h \
    ProxyConnectionManager.h \
    PlotSettingsManager.h \
    QcPlotMainView.h \
    DeviceStateHistoryVariable.h \
    DeviceStatePlotDataVariable.h \
    PlotConfig.h \
    CurveConfig.h \
    CurveConfigView.h \
    PlotConfigView.h \
    PlotManager.h \
    Plotter.h \
    PlotView.h \
    CurveView.h \
    Qwt2AxisMagnifier.h \
    PlotterView.h \
    QcPlotCurve.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qcCommon/release/ -lqcCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qcCommon/debug/ -lqcCommon
else:unix: LIBS += -L$$OUT_PWD/../qcCommon/ -lqcCommon

INCLUDEPATH += $$PWD/../qcCommon
INCLUDEPATH += $$PWD/../qcCommon/clientCommands
DEPENDPATH += $$PWD/../qcCommon

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qcCommon/release/qcCommon.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qcCommon/debug/qcCommon.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../qcCommon/libqcCommon.a

# Qwt (system)
INCLUDEPATH += /usr/include/qwt
LIBS      += -lqwt
