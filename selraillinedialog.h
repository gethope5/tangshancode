#ifndef SELRAILLINEDIALOG_H
#define SELRAILLINEDIALOG_H

#include <QLabel>
#include <QDialog>
#include <QDebug>
namespace Ui {
    class SelRailLineDialog;
}

class SelRailLineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelRailLineDialog(bool,QWidget *parent = 0);
    ~SelRailLineDialog();
    void setRailList(QStringList &list, int idx);
    int getselIndx();
    bool bDirection;
    QString curRail(void);
    void setDirect(bool);
private:
    Ui::SelRailLineDialog *ui;    
    QLabel *pbkGrnd;
private slots:
    void slot_RailDirChanged(void);
};

#endif // SELRAILLINEDIALOG_H
