/****************************************************************************
**
** Copyright (C) 2009 Du Hui.
**
****************************************************************************/


#include "comportdlg.h"
#include "ui_comportdlg.h"
#include "qserialcomm.h"

comPortDlg::comPortDlg(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::comPortDlg)
{
    m_ui->setupUi(this);
    m_Comm = new QSerialComm( this );
    listAvailableComms();
}

comPortDlg::~comPortDlg()
{
    delete m_ui;
    delete m_Comm;
}

void comPortDlg::accept()
{
    QDialog::accept();
    QList<QListWidgetItem*> items = m_ui->listWidget->selectedItems();
    if( items.count() != 1 )
        return setResult(QDialog::Rejected);
    m_selectedCommPorts = items.first()->text();
}

void comPortDlg::reject()
{
    QDialog::reject();
}

int comPortDlg::listAvailableComms()
{
    QString comDeviceName("\\\\.\\COM%1");
    int counts(0);
    for (int i = 0; i < 256; i++)
    {
        QString str = comDeviceName.arg(i);
        if( m_Comm->open(str) )
        {
            QListWidgetItem *item = new QListWidgetItem(m_ui->listWidget);
            item->setText( str );
            m_ui->listWidget->addItem(item);
            counts++;
        }
    }
    return counts;
}

void comPortDlg::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
