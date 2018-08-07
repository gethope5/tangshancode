#ifndef VERIFYSETDLG_H
#define VERIFYSETDLG_H

#include <QDialog>

#include "syshead.h"
namespace Ui {
class verifySetDlg;
}
class verifySetDlg : public QDialog
{
    Q_OBJECT

public:
    explicit verifySetDlg(QWidget *parent = 0);
    ~verifySetDlg();
    char verifyValue;
private slots:
    void on_pbtnOk_clicked();

    void on_chbCancel_clicked();

private:
    Ui::verifySetDlg *ui;

};

#endif // VERIFYSETDLG_H
