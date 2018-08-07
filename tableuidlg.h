#ifndef TABLEUIDLG_H
#define TABLEUIDLG_H

#include <QDialog>
#include <qcheckbox.h>
#include <qvector.h>
#include <qdebug.h>
#include <QSettings>
namespace Ui {
class tableUIDlg;
}

class tableUIDlg : public QDialog
{
    Q_OBJECT

public:
    explicit tableUIDlg(QString,QWidget *parent = 0);
    ~tableUIDlg();
    QVector<QCheckBox *> check;
    QVector<bool> checkStatus;
    QString titleName;
    void saveTitle(QString mm);
private slots:
    void on_pbtnOk_clicked();

    void on_chbCancel_clicked();

private:
    Ui::tableUIDlg *ui;
signals:

};

#endif // TABLEUIDLG_H
