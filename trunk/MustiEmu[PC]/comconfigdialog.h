/****************************************************************************
**
** Copyright (C) 2009 Du Hui.
**
****************************************************************************/


#ifndef COMCONFIGDIALOG_H
#define COMCONFIGDIALOG_H

#include <QtGui/QDialog>
#include "qserialcomm.h"

namespace Ui {
    class comConfigDialog;
}

class comConfigDialog : public QDialog {
    Q_OBJECT
public:
    comConfigDialog( const QString & commPort, QWidget *parent = 0);
    ~comConfigDialog();
    void accept();
    void reject();
    const PORTPROPERTY & property()
    {
        return m_CommProperty;
    }

private slots:
    void setBaudRate(QString value);
    void setDataBits(int value);
    void setParity(int value);
    void setStopBits(int value);
    void setFlowControl(int value);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::comConfigDialog *m_ui;
    QString m_CommPort;
    PORTPROPERTY m_CommProperty;
};

#endif // COMCONFIGDIALOG_H
