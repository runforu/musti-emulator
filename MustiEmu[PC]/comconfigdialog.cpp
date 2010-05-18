/****************************************************************************
**
** Copyright (C) 2009 Du Hui.
**
****************************************************************************/


#include "comconfigdialog.h"
#include "ui_comconfigdialog.h"

comConfigDialog::comConfigDialog( const QString & commPort, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::comConfigDialog),
    m_CommPort(commPort)
{
    m_ui->setupUi(this);
    this->setWindowTitle(m_CommPort);
}

comConfigDialog::~comConfigDialog()
{
    delete m_ui;
}

void comConfigDialog::accept()
{
    QDialog::accept();
}

void comConfigDialog::reject()
{
    QDialog::reject();
}

void comConfigDialog::setBaudRate(QString value)
{
    bool b;
    int v = value.toInt(&b);
    if(b)
    {
        m_CommProperty.BaudRate = v;
    }
}

void comConfigDialog::setDataBits(int value)
{
    m_CommProperty.DataBits = value;
}

void comConfigDialog::setParity(int value)
{
    m_CommProperty.Parity = value;
}

void comConfigDialog::setStopBits(int value)
{
    m_CommProperty.StopBits = value;
}

void comConfigDialog::setFlowControl(int value)
{
    switch ( value )
    {
    default: //fall through
    case 0:
        m_CommProperty.FlowControl = FLOW_TYPE_OFF;
        break;
    case 1:
        m_CommProperty.FlowControl = FLOW_TYPE_HARDWARE;
        break;
    case 2:
        m_CommProperty.FlowControl = FLOW_TYPE_XONXOFF;
        break;
    }
}

void comConfigDialog::changeEvent(QEvent *e)
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
