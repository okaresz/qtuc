TEMPLATE = subdirs
SUBDIRS += qcProxy qcCommon qcGUI

qcGUI.depends = qcCommon
qcProxy.depends = qcCommon
