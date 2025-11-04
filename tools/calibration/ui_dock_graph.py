# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'dock_graph.ui'
##
## Created by: Qt User Interface Compiler version 5.15.2
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide2.QtCore import *
from PySide2.QtGui import *
from PySide2.QtWidgets import *


class Ui_dock_graph(object):
    def setupUi(self, dock_graph):
        if not dock_graph.objectName():
            dock_graph.setObjectName(u"dock_graph")
        dock_graph.resize(400, 300)
        self.gridLayout = QGridLayout(dock_graph)
        self.gridLayout.setObjectName(u"gridLayout")

        self.retranslateUi(dock_graph)

        QMetaObject.connectSlotsByName(dock_graph)
    # setupUi

    def retranslateUi(self, dock_graph):
        dock_graph.setWindowTitle(QCoreApplication.translate("dock_graph", u"Calibration measurements", None))
    # retranslateUi

