#ifndef SETDRAWPARDLG_H
#define SETDRAWPARDLG_H
#include <QtWidgets>
#include <QDialog>
#include <qtextcodec.h>
#include "dlgdrawstly.h"
#include "syshead.h"
namespace Ui {
    class setDrawParDlg;
}
class setDrawParDlg : public QDialog
{
    Q_OBJECT

public:
    explicit setDrawParDlg(QWidget *parent = 0);
    ~setDrawParDlg();
    bool bDrawStyle;
private slots:
    void on_pbtnSetWireColor_clicked();

    void on_pbtnSaveBasePar_clicked();
    void setScanColor_solt(QColor curColor);
    void slot_curveVisiable(void);
    void on_pbtnGetDefault_clicked();

signals:
    void colorChanged(curveType,QColor);
    void curveVisiable(curveType,bool);
    void curveUIParChanged(void);
private:
    Ui::setDrawParDlg *ui;
    void initialPar(bool f=true);
    void getDisplayInitial(void);
};

#endif // SETDRAWPARDLG_H
