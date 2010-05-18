/****************************************************************************
**
** Copyright (C) 2009 Du Hui.
**
****************************************************************************/


#ifndef COMPORTDLG_H
#define COMPORTDLG_H

#include <QtGui/QDialog>

class QSerialComm;

namespace Ui {
    class comPortDlg;
}

class comPortDlg : public QDialog {
    Q_OBJECT
public:
    comPortDlg(QWidget *parent = 0);
    ~comPortDlg();
    void accept();
    void reject();
    QString selectedCommPorts()
    {
        return m_selectedCommPorts;
    }
protected:
    void changeEvent(QEvent *e);

private:
    int listAvailableComms();

    QSerialComm * m_Comm;
    QString  m_selectedCommPorts;

    Ui::comPortDlg *m_ui;
};

#endif // COMPORTDLG_H
