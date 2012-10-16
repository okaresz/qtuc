TEMPLATE = subdirs
SUBDIRS += qcProxy qcCommon

#qcGUI.depends = qcCommon
qcProxy.depends = qcCommon
