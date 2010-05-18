/********************************************************************************
** Form generated from reading ui file 'comconfigdialog.ui'
**
** Created: Tue Sep 8 16:11:10 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_COMCONFIGDIALOG_H
#define UI_COMCONFIGDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_comConfigDialog
{
public:
    QHBoxLayout *horizontalLayout_6;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *comboBox;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QComboBox *comboBox_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QComboBox *comboBox_3;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QComboBox *comboBox_4;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QComboBox *comboBox_5;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *comConfigDialog)
    {
        if (comConfigDialog->objectName().isEmpty())
            comConfigDialog->setObjectName(QString::fromUtf8("comConfigDialog"));
        comConfigDialog->resize(400, 300);
        QFont font;
        font.setFamily(QString::fromUtf8("Verdana"));
        font.setPointSize(10);
        comConfigDialog->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/com_config.png"), QSize(), QIcon::Normal, QIcon::Off);
        comConfigDialog->setWindowIcon(icon);
        horizontalLayout_6 = new QHBoxLayout(comConfigDialog);
        horizontalLayout_6->setSpacing(32);
        horizontalLayout_6->setMargin(16);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(comConfigDialog);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        comboBox = new QComboBox(comConfigDialog);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        horizontalLayout->addWidget(comboBox);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(comConfigDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        comboBox_2 = new QComboBox(comConfigDialog);
        comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));

        horizontalLayout_2->addWidget(comboBox_2);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(comConfigDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);

        comboBox_3 = new QComboBox(comConfigDialog);
        comboBox_3->setObjectName(QString::fromUtf8("comboBox_3"));

        horizontalLayout_3->addWidget(comboBox_3);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_4 = new QLabel(comConfigDialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_4->addWidget(label_4);

        comboBox_4 = new QComboBox(comConfigDialog);
        comboBox_4->setObjectName(QString::fromUtf8("comboBox_4"));

        horizontalLayout_4->addWidget(comboBox_4);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_5 = new QLabel(comConfigDialog);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_5->addWidget(label_5);

        comboBox_5 = new QComboBox(comConfigDialog);
        comboBox_5->setObjectName(QString::fromUtf8("comboBox_5"));

        horizontalLayout_5->addWidget(comboBox_5);


        verticalLayout->addLayout(horizontalLayout_5);

        verticalLayout->setStretch(0, 1);
        verticalLayout->setStretch(1, 1);
        verticalLayout->setStretch(2, 1);
        verticalLayout->setStretch(3, 1);
        verticalLayout->setStretch(4, 1);

        horizontalLayout_6->addLayout(verticalLayout);

        buttonBox = new QDialogButtonBox(comConfigDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout_6->addWidget(buttonBox);


        retranslateUi(comConfigDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), comConfigDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), comConfigDialog, SLOT(reject()));
        QObject::connect(comboBox, SIGNAL(currentIndexChanged(QString)), comConfigDialog, SLOT(setBaudRate(QString)));
        QObject::connect(comboBox_3, SIGNAL(currentIndexChanged(int)), comConfigDialog, SLOT(setParity(int)));
        QObject::connect(comboBox_4, SIGNAL(currentIndexChanged(int)), comConfigDialog, SLOT(setStopBits(int)));
        QObject::connect(comboBox_5, SIGNAL(currentIndexChanged(int)), comConfigDialog, SLOT(setFlowControl(int)));
        QObject::connect(comboBox_2, SIGNAL(currentIndexChanged(int)), comConfigDialog, SLOT(setDataBits(int)));

        comboBox->setCurrentIndex(6);
        comboBox_2->setCurrentIndex(4);
        comboBox_5->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(comConfigDialog);
    } // setupUi

    void retranslateUi(QDialog *comConfigDialog)
    {
        comConfigDialog->setWindowTitle(QApplication::translate("comConfigDialog", "Config Com Port", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("comConfigDialog", "Bits per second:", 0, QApplication::UnicodeUTF8));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("comConfigDialog", "9600", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "14400", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "19200", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "38400", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "56000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "57600", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "115200", 0, QApplication::UnicodeUTF8)
        );
        label_2->setText(QApplication::translate("comConfigDialog", "Data bits:", 0, QApplication::UnicodeUTF8));
        comboBox_2->clear();
        comboBox_2->insertItems(0, QStringList()
         << QApplication::translate("comConfigDialog", "4", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "5", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "6", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "7", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "8", 0, QApplication::UnicodeUTF8)
        );
        label_3->setText(QApplication::translate("comConfigDialog", "Parity:", 0, QApplication::UnicodeUTF8));
        comboBox_3->clear();
        comboBox_3->insertItems(0, QStringList()
         << QApplication::translate("comConfigDialog", "NONE", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "ODD", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "EVEN", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "MARK", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "SPACE", 0, QApplication::UnicodeUTF8)
        );
        label_4->setText(QApplication::translate("comConfigDialog", "Stop bits:", 0, QApplication::UnicodeUTF8));
        comboBox_4->clear();
        comboBox_4->insertItems(0, QStringList()
         << QApplication::translate("comConfigDialog", "1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "1.5", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "2", 0, QApplication::UnicodeUTF8)
        );
        label_5->setText(QApplication::translate("comConfigDialog", "Flow control:", 0, QApplication::UnicodeUTF8));
        comboBox_5->clear();
        comboBox_5->insertItems(0, QStringList()
         << QApplication::translate("comConfigDialog", "Off", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "Hardware", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("comConfigDialog", "Xon / Xoff", 0, QApplication::UnicodeUTF8)
        );
        Q_UNUSED(comConfigDialog);
    } // retranslateUi

};

namespace Ui {
    class comConfigDialog: public Ui_comConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMCONFIGDIALOG_H
