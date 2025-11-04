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
        MainWindow.resize(526, 509)
        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(MainWindow.sizePolicy().hasHeightForWidth())
        MainWindow.setSizePolicy(sizePolicy)
        self.actionGraph = QAction(MainWindow)
        self.actionGraph.setObjectName(u"actionGraph")
        self.actionSave = QAction(MainWindow)
        self.actionSave.setObjectName(u"actionSave")
        self.actionLoad = QAction(MainWindow)
        self.actionLoad.setObjectName(u"actionLoad")
        self.wgtcentral = QWidget(MainWindow)
        self.wgtcentral.setObjectName(u"wgtcentral")
        self.gridLayout_2 = QGridLayout(self.wgtcentral)
        self.gridLayout_2.setObjectName(u"gridLayout_2")
        self.grp_set = QGroupBox(self.wgtcentral)
        self.grp_set.setObjectName(u"grp_set")
        sizePolicy1 = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.grp_set.sizePolicy().hasHeightForWidth())
        self.grp_set.setSizePolicy(sizePolicy1)
        self.gridLayout = QGridLayout(self.grp_set)
        self.gridLayout.setObjectName(u"gridLayout")
        self.lblchain = QLabel(self.grp_set)
        self.lblchain.setObjectName(u"lblchain")
        sizePolicy2 = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Preferred)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.lblchain.sizePolicy().hasHeightForWidth())
        self.lblchain.setSizePolicy(sizePolicy2)

        self.gridLayout.addWidget(self.lblchain, 0, 2, 1, 1)

        self.combo_chain = QComboBox(self.grp_set)
        self.combo_chain.addItem("")
        self.combo_chain.addItem("")
        self.combo_chain.setObjectName(u"combo_chain")
        sizePolicy.setHeightForWidth(self.combo_chain.sizePolicy().hasHeightForWidth())
        self.combo_chain.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.combo_chain, 0, 3, 1, 1)

        self.combo_gpfe = QComboBox(self.grp_set)
        self.combo_gpfe.addItem("")
        self.combo_gpfe.addItem("")
        self.combo_gpfe.setObjectName(u"combo_gpfe")
        sizePolicy.setHeightForWidth(self.combo_gpfe.sizePolicy().hasHeightForWidth())
        self.combo_gpfe.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.combo_gpfe, 0, 1, 1, 1)

        self.btn_connect = QPushButton(self.grp_set)
        self.btn_connect.setObjectName(u"btn_connect")
        sizePolicy.setHeightForWidth(self.btn_connect.sizePolicy().hasHeightForWidth())
        self.btn_connect.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.btn_connect, 0, 5, 1, 1)

        self.spc2 = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.gridLayout.addItem(self.spc2, 0, 4, 1, 1)

        self.lblgpfe = QLabel(self.grp_set)
        self.lblgpfe.setObjectName(u"lblgpfe")
        sizePolicy2.setHeightForWidth(self.lblgpfe.sizePolicy().hasHeightForWidth())
        self.lblgpfe.setSizePolicy(sizePolicy2)

        self.gridLayout.addWidget(self.lblgpfe, 0, 0, 1, 1)

        self.btn_setagc = QPushButton(self.grp_set)
        self.btn_setagc.setObjectName(u"btn_setagc")
        self.btn_setagc.setEnabled(False)
        sizePolicy.setHeightForWidth(self.btn_setagc.sizePolicy().hasHeightForWidth())
        self.btn_setagc.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.btn_setagc, 0, 6, 1, 1)


        self.gridLayout_2.addWidget(self.grp_set, 0, 0, 1, 1)

        self.tabs = QTabWidget(self.wgtcentral)
        self.tabs.setObjectName(u"tabs")
        sizePolicy3 = QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        sizePolicy3.setHorizontalStretch(0)
        sizePolicy3.setVerticalStretch(0)
        sizePolicy3.setHeightForWidth(self.tabs.sizePolicy().hasHeightForWidth())
        self.tabs.setSizePolicy(sizePolicy3)
        self.txtab = QWidget()
        self.txtab.setObjectName(u"txtab")
        self.gridLayout_4 = QGridLayout(self.txtab)
        self.gridLayout_4.setObjectName(u"gridLayout_4")
        self.btn_reset = QPushButton(self.txtab)
        self.btn_reset.setObjectName(u"btn_reset")
        self.btn_reset.setEnabled(False)
        sizePolicy.setHeightForWidth(self.btn_reset.sizePolicy().hasHeightForWidth())
        self.btn_reset.setSizePolicy(sizePolicy)
        self.btn_reset.setStyleSheet(u"background-color: rgb(234, 96, 96);\n"
"color: rgb(204, 33, 33);\n"
"border-color: rgb(230, 55, 55);")

        self.gridLayout_4.addWidget(self.btn_reset, 1, 5, 1, 1)

        self.btn_next = QPushButton(self.txtab)
        self.btn_next.setObjectName(u"btn_next")
        self.btn_next.setEnabled(False)
        sizePolicy.setHeightForWidth(self.btn_next.sizePolicy().hasHeightForWidth())
        self.btn_next.setSizePolicy(sizePolicy)

        self.gridLayout_4.addWidget(self.btn_next, 1, 1, 1, 1)

        self.btn_setdac = QPushButton(self.txtab)
        self.btn_setdac.setObjectName(u"btn_setdac")
        self.btn_setdac.setEnabled(False)
        sizePolicy.setHeightForWidth(self.btn_setdac.sizePolicy().hasHeightForWidth())
        self.btn_setdac.setSizePolicy(sizePolicy)
        self.btn_setdac.setStyleSheet(u"border-color: rgb(30, 55, 153);\n"
"color: rgb(30, 55, 153);\n"
"background-color: rgb(74, 105, 189);")

        self.gridLayout_4.addWidget(self.btn_setdac, 1, 2, 1, 1)

        self.btn_prev = QPushButton(self.txtab)
        self.btn_prev.setObjectName(u"btn_prev")
        self.btn_prev.setEnabled(False)
        sizePolicy.setHeightForWidth(self.btn_prev.sizePolicy().hasHeightForWidth())
        self.btn_prev.setSizePolicy(sizePolicy)

        self.gridLayout_4.addWidget(self.btn_prev, 1, 0, 1, 1)

        self.btn_cali = QPushButton(self.txtab)
        self.btn_cali.setObjectName(u"btn_cali")
        self.btn_cali.setEnabled(False)
        sizePolicy.setHeightForWidth(self.btn_cali.sizePolicy().hasHeightForWidth())
        self.btn_cali.setSizePolicy(sizePolicy)

        self.gridLayout_4.addWidget(self.btn_cali, 1, 4, 1, 1)

        self.spc1 = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.gridLayout_4.addItem(self.spc1, 1, 3, 1, 1)

        self.grp_table = QGroupBox(self.txtab)
        self.grp_table.setObjectName(u"grp_table")
        self.grp_table.setEnabled(False)
        sizePolicy1.setHeightForWidth(self.grp_table.sizePolicy().hasHeightForWidth())
        self.grp_table.setSizePolicy(sizePolicy1)
        self.grd_table = QGridLayout(self.grp_table)
        self.grd_table.setObjectName(u"grd_table")
        self.lbldac = QLabel(self.grp_table)
        self.lbldac.setObjectName(u"lbldac")
        sizePolicy.setHeightForWidth(self.lbldac.sizePolicy().hasHeightForWidth())
        self.lbldac.setSizePolicy(sizePolicy)
        self.lbldac.setAlignment(Qt.AlignCenter)

        self.grd_table.addWidget(self.lbldac, 0, 0, 1, 1)

        self.lbloutput = QLabel(self.grp_table)
        self.lbloutput.setObjectName(u"lbloutput")
        sizePolicy.setHeightForWidth(self.lbloutput.sizePolicy().hasHeightForWidth())
        self.lbloutput.setSizePolicy(sizePolicy)
        self.lbloutput.setAlignment(Qt.AlignCenter)

        self.grd_table.addWidget(self.lbloutput, 0, 2, 1, 1)

        self.lbladc = QLabel(self.grp_table)
        self.lbladc.setObjectName(u"lbladc")
        sizePolicy.setHeightForWidth(self.lbladc.sizePolicy().hasHeightForWidth())
        self.lbladc.setSizePolicy(sizePolicy)
        self.lbladc.setAlignment(Qt.AlignCenter)

        self.grd_table.addWidget(self.lbladc, 0, 1, 1, 1)


        self.gridLayout_4.addWidget(self.grp_table, 0, 0, 1, 6)

        self.tabs.addTab(self.txtab, "")
        self.rxtab = QWidget()
        self.rxtab.setObjectName(u"rxtab")
        self.gridLayout_6 = QGridLayout(self.rxtab)
        self.gridLayout_6.setObjectName(u"gridLayout_6")
        self.grpstep1 = QGroupBox(self.rxtab)
        self.grpstep1.setObjectName(u"grpstep1")
        sizePolicy1.setHeightForWidth(self.grpstep1.sizePolicy().hasHeightForWidth())
        self.grpstep1.setSizePolicy(sizePolicy1)
        self.grpstep1.setAlignment(Qt.AlignLeading|Qt.AlignLeft|Qt.AlignTop)
        self.gridLayout_5 = QGridLayout(self.grpstep1)
        self.gridLayout_5.setObjectName(u"gridLayout_5")
        self.line_setpt = QLineEdit(self.grpstep1)
        self.line_setpt.setObjectName(u"line_setpt")
        self.line_setpt.setEnabled(False)
        self.line_setpt.setReadOnly(True)

        self.gridLayout_5.addWidget(self.line_setpt, 0, 1, 1, 1)

        self.lblsetpt = QLabel(self.grpstep1)
        self.lblsetpt.setObjectName(u"lblsetpt")
        sizePolicy.setHeightForWidth(self.lblsetpt.sizePolicy().hasHeightForWidth())
        self.lblsetpt.setSizePolicy(sizePolicy)

        self.gridLayout_5.addWidget(self.lblsetpt, 0, 0, 1, 1)

        self.btn_clrsetpt = QPushButton(self.grpstep1)
        self.btn_clrsetpt.setObjectName(u"btn_clrsetpt")
        self.btn_clrsetpt.setEnabled(False)
        sizePolicy.setHeightForWidth(self.btn_clrsetpt.sizePolicy().hasHeightForWidth())
        self.btn_clrsetpt.setSizePolicy(sizePolicy)

        self.gridLayout_5.addWidget(self.btn_clrsetpt, 0, 3, 1, 1)

        self.btn_readsetpt = QPushButton(self.grpstep1)
        self.btn_readsetpt.setObjectName(u"btn_readsetpt")
        self.btn_readsetpt.setEnabled(False)
        sizePolicy.setHeightForWidth(self.btn_readsetpt.sizePolicy().hasHeightForWidth())
        self.btn_readsetpt.setSizePolicy(sizePolicy)

        self.gridLayout_5.addWidget(self.btn_readsetpt, 0, 2, 1, 1)

        self.btn_setpt = QPushButton(self.grpstep1)
        self.btn_setpt.setObjectName(u"btn_setpt")
        self.btn_setpt.setEnabled(False)
        sizePolicy.setHeightForWidth(self.btn_setpt.sizePolicy().hasHeightForWidth())
        self.btn_setpt.setSizePolicy(sizePolicy)

        self.gridLayout_5.addWidget(self.btn_setpt, 1, 2, 1, 1)

        self.line_defoffset = QLineEdit(self.grpstep1)
        self.line_defoffset.setObjectName(u"line_defoffset")
        # self.line_defoffset.setEnabled(False)
        # self.line_defoffset.setReadOnly(True)

        self.gridLayout_5.addWidget(self.line_defoffset, 1, 1, 1, 1)

        self.lbldefoffset = QLabel(self.grpstep1)
        self.lbldefoffset.setObjectName(u"lbldefoffset")
        sizePolicy.setHeightForWidth(self.lbldefoffset.sizePolicy().hasHeightForWidth())
        self.lbldefoffset.setSizePolicy(sizePolicy)

        self.gridLayout_5.addWidget(self.lbldefoffset, 1, 0, 1, 1)


        self.gridLayout_6.addWidget(self.grpstep1, 0, 0, 1, 4)

        self.grpstep2 = QGroupBox(self.rxtab)
        self.grpstep2.setObjectName(u"grpstep2")
        sizePolicy1.setHeightForWidth(self.grpstep2.sizePolicy().hasHeightForWidth())
        self.grpstep2.setSizePolicy(sizePolicy1)
        self.gridLayout_3 = QGridLayout(self.grpstep2)
        self.gridLayout_3.setObjectName(u"gridLayout_3")
        self.line_difoffset2 = QLineEdit(self.grpstep2)
        self.line_difoffset2.setObjectName(u"line_difoffset2")
        self.line_difoffset2.setEnabled(False)
        self.line_difoffset2.setReadOnly(True)

        self.gridLayout_3.addWidget(self.line_difoffset2, 2, 2, 1, 1)

        self.line_carlvl2 = QLineEdit(self.grpstep2)
        self.line_carlvl2.setObjectName(u"line_carlvl2")
        self.line_carlvl2.setEnabled(False)

        self.gridLayout_3.addWidget(self.line_carlvl2, 2, 1, 1, 1)

        self.lblinjsig = QLabel(self.grpstep2)
        self.lblinjsig.setObjectName(u"lblinjsig")
        sizePolicy1.setHeightForWidth(self.lblinjsig.sizePolicy().hasHeightForWidth())
        self.lblinjsig.setSizePolicy(sizePolicy1)

        self.gridLayout_3.addWidget(self.lblinjsig, 0, 0, 1, 1)

        self.line_offset = QLineEdit(self.grpstep2)
        self.line_offset.setObjectName(u"line_offset")
        self.line_offset.setEnabled(False)

        self.gridLayout_3.addWidget(self.line_offset, 4, 1, 1, 2)

        self.line_meandiff = QLineEdit(self.grpstep2)
        self.line_meandiff.setObjectName(u"line_meandiff")
        self.line_meandiff.setEnabled(False)
        self.line_meandiff.setReadOnly(True)

        self.gridLayout_3.addWidget(self.line_meandiff, 3, 1, 1, 2)

        self.line_injsig1 = QLineEdit(self.grpstep2)
        self.line_injsig1.setObjectName(u"line_injsig1")
        self.line_injsig1.setEnabled(False)
        self.line_injsig1.setReadOnly(True)

        self.gridLayout_3.addWidget(self.line_injsig1, 1, 0, 1, 1)

        self.line_injsig2 = QLineEdit(self.grpstep2)
        self.line_injsig2.setObjectName(u"line_injsig2")
        self.line_injsig2.setEnabled(False)
        self.line_injsig2.setReadOnly(True)

        self.gridLayout_3.addWidget(self.line_injsig2, 2, 0, 1, 1)

        self.lblmeandiff = QLabel(self.grpstep2)
        self.lblmeandiff.setObjectName(u"lblmeandiff")
        sizePolicy1.setHeightForWidth(self.lblmeandiff.sizePolicy().hasHeightForWidth())
        self.lblmeandiff.setSizePolicy(sizePolicy1)

        self.gridLayout_3.addWidget(self.lblmeandiff, 3, 0, 1, 1)

        self.line_carlvl1 = QLineEdit(self.grpstep2)
        self.line_carlvl1.setObjectName(u"line_carlvl1")
        self.line_carlvl1.setEnabled(False)

        self.gridLayout_3.addWidget(self.line_carlvl1, 1, 1, 1, 1)

        self.lbloffset = QLabel(self.grpstep2)
        self.lbloffset.setObjectName(u"lbloffset")
        sizePolicy1.setHeightForWidth(self.lbloffset.sizePolicy().hasHeightForWidth())
        self.lbloffset.setSizePolicy(sizePolicy1)

        self.gridLayout_3.addWidget(self.lbloffset, 4, 0, 1, 1)

        self.lbldifoffset = QLabel(self.grpstep2)
        self.lbldifoffset.setObjectName(u"lbldifoffset")
        sizePolicy1.setHeightForWidth(self.lbldifoffset.sizePolicy().hasHeightForWidth())
        self.lbldifoffset.setSizePolicy(sizePolicy1)

        self.gridLayout_3.addWidget(self.lbldifoffset, 0, 2, 1, 1)

        self.line_difoffset1 = QLineEdit(self.grpstep2)
        self.line_difoffset1.setObjectName(u"line_difoffset1")
        self.line_difoffset1.setEnabled(False)
        self.line_difoffset1.setReadOnly(True)

        self.gridLayout_3.addWidget(self.line_difoffset1, 1, 2, 1, 1)

        self.lblcarlvl = QLabel(self.grpstep2)
        self.lblcarlvl.setObjectName(u"lblcarlvl")
        sizePolicy1.setHeightForWidth(self.lblcarlvl.sizePolicy().hasHeightForWidth())
        self.lblcarlvl.setSizePolicy(sizePolicy1)

        self.gridLayout_3.addWidget(self.lblcarlvl, 0, 1, 1, 1)

        self.lbltimeconst = QLabel(self.grpstep2)
        self.lbltimeconst.setObjectName(u"lbltimeconst")

        self.gridLayout_3.addWidget(self.lbltimeconst, 5, 0, 1, 1)

        self.line_timeconst = QLineEdit(self.grpstep2)
        self.line_timeconst.setObjectName(u"line_timeconst")
        self.line_timeconst.setEnabled(False)
        self.line_timeconst.setMaxLength(5)

        self.gridLayout_3.addWidget(self.line_timeconst, 5, 1, 1, 2)


        self.gridLayout_6.addWidget(self.grpstep2, 4, 0, 1, 4)

        self.btn_rxcal = QPushButton(self.rxtab)
        self.btn_rxcal.setObjectName(u"btn_rxcal")
        self.btn_rxcal.setEnabled(False)
        sizePolicy.setHeightForWidth(self.btn_rxcal.sizePolicy().hasHeightForWidth())
        self.btn_rxcal.setSizePolicy(sizePolicy)

        self.gridLayout_6.addWidget(self.btn_rxcal, 8, 3, 1, 1)

        self.tabs.addTab(self.rxtab, "")

        self.gridLayout_2.addWidget(self.tabs, 2, 0, 1, 1)

        MainWindow.setCentralWidget(self.wgtcentral)
        self.menubar = QMenuBar(MainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 526, 20))
        self.menuView = QMenu(self.menubar)
        self.menuView.setObjectName(u"menuView")
        self.menuFile = QMenu(self.menubar)
        self.menuFile.setObjectName(u"menuFile")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        self.statusbar.setSizeGripEnabled(False)
        MainWindow.setStatusBar(self.statusbar)

        self.menubar.addAction(self.menuFile.menuAction())
        self.menubar.addAction(self.menuView.menuAction())
        self.menuFile.addAction(self.actionSave)
        self.menuFile.addAction(self.actionLoad)

        self.retranslateUi(MainWindow)

        self.tabs.setCurrentIndex(0)


        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        self.actionGraph.setText(QCoreApplication.translate("MainWindow", u"Graph", None))
        self.actionSave.setText(QCoreApplication.translate("MainWindow", u"Save calibration values", None))
        self.actionLoad.setText(QCoreApplication.translate("MainWindow", u"Load calibration values", None))
        self.grp_set.setTitle(QCoreApplication.translate("MainWindow", u"Settings", None))
        self.lblchain.setText(QCoreApplication.translate("MainWindow", u"Chain", None))
        self.combo_chain.setItemText(0, QCoreApplication.translate("MainWindow", u"1", None))
        self.combo_chain.setItemText(1, QCoreApplication.translate("MainWindow", u"2", None))

        self.combo_gpfe.setItemText(0, QCoreApplication.translate("MainWindow", u"1", None))
        self.combo_gpfe.setItemText(1, QCoreApplication.translate("MainWindow", u"2", None))

        self.btn_connect.setText(QCoreApplication.translate("MainWindow", u"Connect", None))
        self.lblgpfe.setText(QCoreApplication.translate("MainWindow", u"GPFE", None))
        self.btn_setagc.setText(QCoreApplication.translate("MainWindow", u"Disable AGC", None))
        self.btn_reset.setText(QCoreApplication.translate("MainWindow", u"Reset", None))
        self.btn_next.setText(QCoreApplication.translate("MainWindow", u"Next", None))
        self.btn_setdac.setText(QCoreApplication.translate("MainWindow", u"Set DAC", None))
        self.btn_prev.setText(QCoreApplication.translate("MainWindow", u"Previous", None))
        self.btn_cali.setText(QCoreApplication.translate("MainWindow", u"Calibrate", None))
        self.lbldac.setText(QCoreApplication.translate("MainWindow", u"DAC [0x0XXX]", None))
        self.lbloutput.setText(QCoreApplication.translate("MainWindow", u"Output level [dBm]", None))
        self.lbladc.setText(QCoreApplication.translate("MainWindow", u"ADC [0xXXXX]", None))
        self.tabs.setTabText(self.tabs.indexOf(self.txtab), QCoreApplication.translate("MainWindow", u"TX calibration", None))
        self.grpstep1.setTitle(QCoreApplication.translate("MainWindow", u"Step 1: Read RX ADC and configure default offset (AGC off)", None))
        self.lblsetpt.setText(QCoreApplication.translate("MainWindow", u"RX ADC [0xXXXX]", None))
#if QT_CONFIG(tooltip)
        self.btn_clrsetpt.setToolTip(QCoreApplication.translate("MainWindow", u"Clear ADC", None))
#endif // QT_CONFIG(tooltip)
        self.btn_clrsetpt.setText(QCoreApplication.translate("MainWindow", u"Clear", None))
#if QT_CONFIG(tooltip)
        self.btn_readsetpt.setToolTip(QCoreApplication.translate("MainWindow", u"Read ADC value", None))
#endif // QT_CONFIG(tooltip)
        self.btn_readsetpt.setText(QCoreApplication.translate("MainWindow", u"Read", None))
#if QT_CONFIG(tooltip)
        self.btn_setpt.setToolTip(QCoreApplication.translate("MainWindow", u"Set ADC and default offset", None))
#endif // QT_CONFIG(tooltip)
        self.btn_setpt.setText(QCoreApplication.translate("MainWindow", u"Set", None))
        self.line_defoffset.setText(QCoreApplication.translate("MainWindow", u"90.0", None))
        self.lbldefoffset.setText(QCoreApplication.translate("MainWindow", u"Default offset [dBm]", None))
        self.grpstep2.setTitle(QCoreApplication.translate("MainWindow", u"Step 2: Inject signal with noise density 110 dBm/Hz and read level (AGC on)", None))
#if QT_CONFIG(tooltip)
        self.lblinjsig.setToolTip(QCoreApplication.translate("MainWindow", u"Injected signal at input of drawer", None))
#endif // QT_CONFIG(tooltip)
        self.lblinjsig.setText(QCoreApplication.translate("MainWindow", u"Injected signal [dBm]", None))
        self.line_injsig1.setText(QCoreApplication.translate("MainWindow", u"-30.0", None))
        self.line_injsig2.setText(QCoreApplication.translate("MainWindow", u"-60.0", None))
#if QT_CONFIG(tooltip)
        self.lblmeandiff.setToolTip(QCoreApplication.translate("MainWindow", u"Mean difference offset", None))
#endif // QT_CONFIG(tooltip)
        self.lblmeandiff.setText(QCoreApplication.translate("MainWindow", u"Mean difference [dBm]", None))
#if QT_CONFIG(tooltip)
        self.lbloffset.setToolTip(QCoreApplication.translate("MainWindow", u"Default offset added by mean difference offset", None))
#endif // QT_CONFIG(tooltip)
        self.lbloffset.setText(QCoreApplication.translate("MainWindow", u"Offset [dBm]", None))
#if QT_CONFIG(tooltip)
        self.lbldifoffset.setToolTip(QCoreApplication.translate("MainWindow", u"Calculated offset between injected signal and carrier level", None))
#endif // QT_CONFIG(tooltip)
        self.lbldifoffset.setText(QCoreApplication.translate("MainWindow", u"Difference offset [dBm]", None))
        self.line_difoffset1.setText("")
#if QT_CONFIG(tooltip)
        self.lblcarlvl.setToolTip(QCoreApplication.translate("MainWindow", u"Carrier level read in the receiver (GUI)", None))
#endif // QT_CONFIG(tooltip)
        self.lblcarlvl.setText(QCoreApplication.translate("MainWindow", u"Carrier level [dBm]", None))
        self.lbltimeconst.setText(QCoreApplication.translate("MainWindow", u"Time constant [ms]", None))
        self.line_timeconst.setText(QCoreApplication.translate("MainWindow", u"1000", None))
#if QT_CONFIG(tooltip)
        self.btn_rxcal.setToolTip(QCoreApplication.translate("MainWindow", u"Calibrate RX", None))
#endif // QT_CONFIG(tooltip)
        self.btn_rxcal.setText(QCoreApplication.translate("MainWindow", u"Calibrate", None))
        self.tabs.setTabText(self.tabs.indexOf(self.rxtab), QCoreApplication.translate("MainWindow", u"RX calibration", None))
        self.menuView.setTitle(QCoreApplication.translate("MainWindow", u"View", None))
        self.menuFile.setTitle(QCoreApplication.translate("MainWindow", u"File", None))
        pass
    # retranslateUi

