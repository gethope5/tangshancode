#ifndef CONFIRMPOLECHANGEDIALOG_H
#define CONFIRMPOLECHANGEDIALOG_H

#include <QDialog>
#include <qtextcodec.h>
namespace Ui {
    class ConFirmPoleChangeDialog;
}

class ConFirmPoleChangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConFirmPoleChangeDialog(QString currPole,
                                     QString topole, QWidget *parent = 0);
    ~ConFirmPoleChangeDialog();

public slots:
    void changePole(QString poleId);
private:
    QString m_topole;
    Ui::ConFirmPoleChangeDialog *ui;
};

#endif // CONFIRMPOLECHANGEDIALOG_H
