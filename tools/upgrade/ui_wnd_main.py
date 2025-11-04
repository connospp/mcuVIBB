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


class Ui_Dialog(object):
    def setupUi(self, Dialog):
        if not Dialog.objectName():
            Dialog.setObjectName(u"Dialog")
        Dialog.resize(406, 173)
        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(Dialog.sizePolicy().hasHeightForWidth())
        Dialog.setSizePolicy(sizePolicy)
        self.gridLayout = QGridLayout(Dialog)
        self.gridLayout.setObjectName(u"gridLayout")
        self.btn_select = QPushButton(Dialog)
        self.btn_select.setObjectName(u"btn_select")
        sizePolicy.setHeightForWidth(self.btn_select.sizePolicy().hasHeightForWidth())
        self.btn_select.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.btn_select, 1, 6, 1, 1)

        self.spinBox = QSpinBox(Dialog)
        self.spinBox.setObjectName(u"spinBox")
        sizePolicy.setHeightForWidth(self.spinBox.sizePolicy().hasHeightForWidth())
        self.spinBox.setSizePolicy(sizePolicy)
        self.spinBox.setMaximum(255)

        self.gridLayout.addWidget(self.spinBox, 3, 1, 1, 1)

        self.pushButton = QPushButton(Dialog)
        self.pushButton.setObjectName(u"pushButton")
        self.pushButton.setStyleSheet(u"background-color: rgb(235, 76, 73);\n"
"color: rgb(182, 40, 40);")

        self.gridLayout.addWidget(self.pushButton, 3, 6, 1, 1)

        self.label_2 = QLabel(Dialog)
        self.label_2.setObjectName(u"label_2")
        sizePolicy1 = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Preferred)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.label_2.sizePolicy().hasHeightForWidth())
        self.label_2.setSizePolicy(sizePolicy1)

        self.gridLayout.addWidget(self.label_2, 3, 0, 1, 1)

        self.progress_bar = QProgressBar(Dialog)
        self.progress_bar.setObjectName(u"progress_bar")
        font = QFont()
        font.setItalic(True)
        self.progress_bar.setFont(font)
        self.progress_bar.setValue(0)

        self.gridLayout.addWidget(self.progress_bar, 5, 0, 1, 7)

        self.line_image = QLineEdit(Dialog)
        self.line_image.setObjectName(u"line_image")

        self.gridLayout.addWidget(self.line_image, 1, 0, 1, 6)

        self.lbl_status = QLabel(Dialog)
        self.lbl_status.setObjectName(u"lbl_status")
        sizePolicy1.setHeightForWidth(self.lbl_status.sizePolicy().hasHeightForWidth())
        self.lbl_status.setSizePolicy(sizePolicy1)
        font1 = QFont()
        font1.setFamily(u"Sans Serif")
        font1.setPointSize(9)
        font1.setBold(False)
        font1.setItalic(True)
        font1.setWeight(50)
        self.lbl_status.setFont(font1)
        self.lbl_status.setStyleSheet(u"font: italic 9pt \"Sans Serif\";\n"
"color: rgb(63, 63, 63);")

        self.gridLayout.addWidget(self.lbl_status, 6, 0, 1, 7)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 3, 4, 1, 2)

        self.label = QLabel(Dialog)
        self.label.setObjectName(u"label")
        sizePolicy1.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy1)

        self.gridLayout.addWidget(self.label, 3, 2, 1, 1)

        self.spinBox_2 = QSpinBox(Dialog)
        self.spinBox_2.setObjectName(u"spinBox_2")
        sizePolicy.setHeightForWidth(self.spinBox_2.sizePolicy().hasHeightForWidth())
        self.spinBox_2.setSizePolicy(sizePolicy)
        self.spinBox_2.setMaximum(4096)

        self.gridLayout.addWidget(self.spinBox_2, 3, 3, 1, 1)

        self.gridLayout_2 = QGridLayout()
        self.gridLayout_2.setObjectName(u"gridLayout_2")
        self.lineEdit = QLineEdit(Dialog)
        self.lineEdit.setObjectName(u"lineEdit")

        self.gridLayout_2.addWidget(self.lineEdit, 1, 0, 1, 1)

        self.label_3 = QLabel(Dialog)
        self.label_3.setObjectName(u"label_3")

        self.gridLayout_2.addWidget(self.label_3, 0, 0, 1, 1)

        self.label_4 = QLabel(Dialog)
        self.label_4.setObjectName(u"label_4")

        self.gridLayout_2.addWidget(self.label_4, 0, 1, 1, 1)

        self.spinBox_3 = QSpinBox(Dialog)
        self.spinBox_3.setObjectName(u"spinBox_3")
        sizePolicy.setHeightForWidth(self.spinBox_3.sizePolicy().hasHeightForWidth())
        self.spinBox_3.setSizePolicy(sizePolicy)
        self.spinBox_3.setMinimum(1)
        self.spinBox_3.setMaximum(65535)
        self.spinBox_3.setValue(5000)

        self.gridLayout_2.addWidget(self.spinBox_3, 1, 1, 1, 1)


        self.gridLayout.addLayout(self.gridLayout_2, 0, 0, 1, 7)


        self.retranslateUi(Dialog)
        self.btn_select.clicked.connect(Dialog.select_file)

        QMetaObject.connectSlotsByName(Dialog)
    # setupUi

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(QCoreApplication.translate("Dialog", u"Upgrade MCU", None))
        self.btn_select.setText(QCoreApplication.translate("Dialog", u"Select", None))
#if QT_CONFIG(shortcut)
        self.btn_select.setShortcut(QCoreApplication.translate("Dialog", u"F", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton.setText(QCoreApplication.translate("Dialog", u"Burn", None))
        self.label_2.setText(QCoreApplication.translate("Dialog", u"Version", None))
        self.line_image.setPlaceholderText(QCoreApplication.translate("Dialog", u"MCU image [*.SREC]", None))
        self.lbl_status.setText(QCoreApplication.translate("Dialog", u"Select the MCU image and click OK.", None))
        self.label.setText(QCoreApplication.translate("Dialog", u".", None))
        self.label_3.setText(QCoreApplication.translate("Dialog", u"IP address", None))
        self.label_4.setText(QCoreApplication.translate("Dialog", u"Port", None))
    # retranslateUi

