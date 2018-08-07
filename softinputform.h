#ifndef SOFTINPUTFORM_H
#define SOFTINPUTFORM_H

#include <QWidget>
#include <QtWidgets>
#include "softboarddlg.h"
#include <QDebug>
namespace Ui {
    class softInputForm;
}

class softInputForm : public QWidget
{
    Q_OBJECT

public:
    explicit softInputForm(QWidget *parent = 0);
    ~softInputForm();
    void setLblName(QString name);
    void setValue(QString strValue);
    void setLineWidth(int maxWidth,int minWidth);
    QString getValue(void );
    void setCustomLabel(QString buffer);
    void setLineEidtWidth(int width);
//    void
private slots:
    void on_pbtnIconKey_clicked();
    void slot_setValue(int i);

private:
    Ui::softInputForm *ui;
    softBoardDlg *softBoard;
    QString sCustomLabel;
};

#endif // SOFTINPUTFORM_H
