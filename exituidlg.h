#ifndef EXITUIDLG_H
#define EXITUIDLG_H

#include <QDialog>
#include <QMessageBox>
#include <qtextcodec.h>
namespace Ui {
    class exitUIDlg;
}

class exitUIDlg : public QDialog
{
    Q_OBJECT

public:
    explicit exitUIDlg(QWidget *parent = 0);
    ~exitUIDlg();
    bool sleepSys(void);

private slots:
    void on_checkBox_clicked();

private:
    Ui::exitUIDlg *ui;
};

#endif // EXITUIDLG_H
