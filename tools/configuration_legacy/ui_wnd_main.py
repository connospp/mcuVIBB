# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'wnd_main.ui'
##
## Created by: Qt User Interface Compiler version 5.15.2
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide2.QtCore import *
from PySide2.QtGui import *
from PySide2.QtWidgets import *


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(412, 302)
        self.actionDebug = QAction(MainWindow)
        self.actionDebug.setObjectName(u"actionDebug")
        self.actionDebug.setCheckable(True)
        self.actionInfo = QAction(MainWindow)
        self.actionInfo.setObjectName(u"actionInfo")
        self.actionInfo.setCheckable(True)
        self.actionWarning = QAction(MainWindow)
        self.actionWarning.setObjectName(u"actionWarning")
        self.actionWarning.setCheckable(True)
        self.actionError = QAction(MainWindow)
        self.actionError.setObjectName(u"actionError")
        self.actionError.setCheckable(True)
        self.actionCritical = QAction(MainWindow)
        self.actionCritical.setObjectName(u"actionCritical")
        self.actionCritical.setCheckable(True)
        self.actionHelp = QAction(MainWindow)
        self.actionHelp.setObjectName(u"actionHelp")
        self.wgtcentral = QWidget(MainWindow)
        self.wgtcentral.setObjectName(u"wgtcentral")
        self.gridLayout_2 = QGridLayout(self.wgtcentral)
        self.gridLayout_2.setObjectName(u"gridLayout_2")
        self.line_host = QLineEdit(self.wgtcentral)
        self.line_host.setObjectName(u"line_host")
        sizePolicy = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.line_host.sizePolicy().hasHeightForWidth())
        self.line_host.setSizePolicy(sizePolicy)
        self.line_host.setMaxLength(15)

        self.gridLayout_2.addWidget(self.line_host, 0, 1, 1, 2)

        self.lblhostport = QLabel(self.wgtcentral)
        self.lblhostport.setObjectName(u"lblhostport")
        sizePolicy1 = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Preferred)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.lblhostport.sizePolicy().hasHeightForWidth())
        self.lblhostport.setSizePolicy(sizePolicy1)

        self.gridLayout_2.addWidget(self.lblhostport, 0, 3, 1, 1)

        self.btn_connect = QPushButton(self.wgtcentral)
        self.btn_connect.setObjectName(u"btn_connect")
        sizePolicy2 = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.btn_connect.sizePolicy().hasHeightForWidth())
        self.btn_connect.setSizePolicy(sizePolicy2)

        self.gridLayout_2.addWidget(self.btn_connect, 0, 5, 1, 1)

        self.lblhost = QLabel(self.wgtcentral)
        self.lblhost.setObjectName(u"lblhost")
        sizePolicy1.setHeightForWidth(self.lblhost.sizePolicy().hasHeightForWidth())
        self.lblhost.setSizePolicy(sizePolicy1)

        self.gridLayout_2.addWidget(self.lblhost, 0, 0, 1, 1)

        self.grp_ip = QGroupBox(self.wgtcentral)
        self.grp_ip.setObjectName(u"grp_ip")
        self.grp_ip.setEnabled(False)
        sizePolicy.setHeightForWidth(self.grp_ip.sizePolicy().hasHeightForWidth())
        self.grp_ip.setSizePolicy(sizePolicy)
        self.grp_ip.setAlignment(Qt.AlignLeading|Qt.AlignLeft|Qt.AlignTop)
        self.gridLayout_3 = QGridLayout(self.grp_ip)
        self.gridLayout_3.setObjectName(u"gridLayout_3")
        self.line_port = QLineEdit(self.grp_ip)
        self.line_port.setObjectName(u"line_port")
        sizePolicy2.setHeightForWidth(self.line_port.sizePolicy().hasHeightForWidth())
        self.line_port.setSizePolicy(sizePolicy2)
        self.line_port.setMaximumSize(QSize(60, 16777215))

        self.gridLayout_3.addWidget(self.line_port, 1, 1, 1, 1)

        self.btn_configure = QPushButton(self.grp_ip)
        self.btn_configure.setObjectName(u"btn_configure")
        sizePolicy2.setHeightForWidth(self.btn_configure.sizePolicy().hasHeightForWidth())
        self.btn_configure.setSizePolicy(sizePolicy2)

        self.gridLayout_3.addWidget(self.btn_configure, 6, 0, 1, 1)

        self.lblip = QLabel(self.grp_ip)
        self.lblip.setObjectName(u"lblip")
        sizePolicy2.setHeightForWidth(self.lblip.sizePolicy().hasHeightForWidth())
        self.lblip.setSizePolicy(sizePolicy2)

        self.gridLayout_3.addWidget(self.lblip, 0, 0, 1, 1)

        self.lblsubnet = QLabel(self.grp_ip)
        self.lblsubnet.setObjectName(u"lblsubnet")
        sizePolicy2.setHeightForWidth(self.lblsubnet.sizePolicy().hasHeightForWidth())
        self.lblsubnet.setSizePolicy(sizePolicy2)

        self.gridLayout_3.addWidget(self.lblsubnet, 2, 0, 1, 1)

        self.lblport = QLabel(self.grp_ip)
        self.lblport.setObjectName(u"lblport")
        sizePolicy2.setHeightForWidth(self.lblport.sizePolicy().hasHeightForWidth())
        self.lblport.setSizePolicy(sizePolicy2)

        self.gridLayout_3.addWidget(self.lblport, 0, 1, 1, 1)

        self.line_ip = QLineEdit(self.grp_ip)
        self.line_ip.setObjectName(u"line_ip")
        self.line_ip.setMaxLength(15)

        self.gridLayout_3.addWidget(self.line_ip, 1, 0, 1, 1)

        self.lbldefgate = QLabel(self.grp_ip)
        self.lbldefgate.setObjectName(u"lbldefgate")
        sizePolicy1.setHeightForWidth(self.lbldefgate.sizePolicy().hasHeightForWidth())
        self.lbldefgate.setSizePolicy(sizePolicy1)

        self.gridLayout_3.addWidget(self.lbldefgate, 4, 0, 1, 1)

        self.line_subnet = QLineEdit(self.grp_ip)
        self.line_subnet.setObjectName(u"line_subnet")
        self.line_subnet.setMaxLength(15)

        self.gridLayout_3.addWidget(self.line_subnet, 3, 0, 1, 2)

        self.line_defgate = QLineEdit(self.grp_ip)
        self.line_defgate.setObjectName(u"line_defgate")
        self.line_defgate.setMaxLength(15)

        self.gridLayout_3.addWidget(self.line_defgate, 5, 0, 1, 2)


        self.gridLayout_2.addWidget(self.grp_ip, 3, 0, 1, 6)

        self.line_hostport = QLineEdit(self.wgtcentral)
        self.line_hostport.setObjectName(u"line_hostport")
        sizePolicy2.setHeightForWidth(self.line_hostport.sizePolicy().hasHeightForWidth())
        self.line_hostport.setSizePolicy(sizePolicy2)
        self.line_hostport.setMaximumSize(QSize(60, 16777215))
        self.line_hostport.setMaxLength(5)

        self.gridLayout_2.addWidget(self.line_hostport, 0, 4, 1, 1)

        MainWindow.setCentralWidget(self.wgtcentral)
        self.menubar = QMenuBar(MainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 412, 20))
        self.menuHelp = QMenu(self.menubar)
        self.menuHelp.setObjectName(u"menuHelp")
        self.menuLog_level = QMenu(self.menuHelp)
        self.menuLog_level.setObjectName(u"menuLog_level")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        self.statusbar.setStyleSheet(u"background-color: rgb(230, 230, 230);")
        self.statusbar.setSizeGripEnabled(False)
        MainWindow.setStatusBar(self.statusbar)

        self.menubar.addAction(self.menuHelp.menuAction())
        self.menuHelp.addAction(self.menuLog_level.menuAction())
        self.menuHelp.addAction(self.actionHelp)
        self.menuLog_level.addAction(self.actionDebug)
        self.menuLog_level.addAction(self.actionInfo)
        self.menuLog_level.addAction(self.actionWarning)
        self.menuLog_level.addAction(self.actionError)
        self.menuLog_level.addAction(self.actionCritical)

        self.retranslateUi(MainWindow)

        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        self.actionDebug.setText(QCoreApplication.translate("MainWindow", u"Debug", None))
        self.actionInfo.setText(QCoreApplication.translate("MainWindow", u"Info", None))
        self.actionWarning.setText(QCoreApplication.translate("MainWindow", u"Warning", None))
        self.actionError.setText(QCoreApplication.translate("MainWindow", u"Error", None))
        self.actionCritical.setText(QCoreApplication.translate("MainWindow", u"Critical", None))
        self.actionHelp.setText(QCoreApplication.translate("MainWindow", u"Help", None))
        self.lblhostport.setText(QCoreApplication.translate("MainWindow", u"Port", None))
        self.btn_connect.setText(QCoreApplication.translate("MainWindow", u"Connect", None))
        self.lblhost.setText(QCoreApplication.translate("MainWindow", u"Host", None))
        self.grp_ip.setTitle(QCoreApplication.translate("MainWindow", u"IP Settings", None))
        self.btn_configure.setText(QCoreApplication.translate("MainWindow", u"Set configuration", None))
        self.lblip.setText(QCoreApplication.translate("MainWindow", u"IPv4 address", None))
        self.lblsubnet.setText(QCoreApplication.translate("MainWindow", u"Subnet", None))
        self.lblport.setText(QCoreApplication.translate("MainWindow", u"Port", None))
        self.lbldefgate.setText(QCoreApplication.translate("MainWindow", u"Default gateway", None))
        self.menuHelp.setTitle(QCoreApplication.translate("MainWindow", u"Help", None))
        self.menuLog_level.setTitle(QCoreApplication.translate("MainWindow", u"Log level", None))
        pass
    # retranslateUi

