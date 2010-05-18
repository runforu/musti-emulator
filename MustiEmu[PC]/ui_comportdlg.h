/********************************************************************************
** Form generated from reading ui file 'comportdlg.ui'
**
** Created: Tue Sep 8 16:11:10 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_COMPORTDLG_H
#define UI_COMPORTDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>

QT_BEGIN_NAMESPACE

class Ui_comPortDlg
{
public:
    QHBoxLayout *horizontalLayout;
    QListWidget *listWidget;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *comPortDlg)
    {
        if (comPortDlg->objectName().isEmpty())
            comPortDlg->setObjectName(QString::fromUtf8("comPortDlg"));
        comPortDlg->resize(400, 300);
        QFont font;
        font.setFamily(QString::fromUtf8("Verdana"));
        font.setPointSize(10);
        comPortDlg->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/com_ports_search.png"), QSize(), QIcon::Normal, QIcon::Off);
        comPortDlg->setWindowIcon(icon);
        horizontalLayout = new QHBoxLayout(comPortDlg);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        listWidget = new QListWidget(comPortDlg);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        horizontalLayout->addWidget(listWidget);

        buttonBox = new QDialogButtonBox(comPortDlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox);


        retranslateUi(comPortDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), comPortDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), comPortDlg, SLOT(reject()));
        QObject::connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), comPortDlg, SLOT(accept()));

        QMetaObject::connectSlotsByName(comPortDlg);
    } // setupUi

    void retranslateUi(QDialog *comPortDlg)
    {
        comPortDlg->setWindowTitle(QApplication::translate("comPortDlg", "Available Com Ports", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(comPortDlg);
    } // retranslateUi

};

namespace Ui {
    class comPortDlg: public Ui_comPortDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMPORTDLG_H
