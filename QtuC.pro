TEMPLATE = subdirs
SUBDIRS += qcProxy qcCommon qcGUI qcPlot

qcProxy.depends = qcCommon
qcGUI.depends = qcCommon
qcPlot.depends = qcCommon
