#ifndef RAILLINEEDIT_H
#define RAILLINEEDIT_H
#include <QtGui>
#include <QtSql>
#include <qtextcodec.h>
#include "ui_raillineedit.h"
#include <QMessageBox>
#include "excel/excelparse.h"
#include "hoverbitmapbtn.h"
#include "tableuidlg.h"
#include "selraillinedialog.h"
namespace Ui {
    class RailLineEdit;
}

//功能：管理线路基础信息
class RailLineEdit : public QWidget, private Ui::RailLineEdit
{
    Q_OBJECT

public:
    explicit RailLineEdit(QWidget *parent = 0);
    ~RailLineEdit();

    void updateActions();
    void showDataTable(const QString &dataName);
    void insertRow();
    void deleteRow();
    void updateRailLine(QString railName);
public slots:
    void Insert_triggered()
    { insertRow(); }
    void DelRow_triggered()
    { deleteRow(); }
    void currentChanged() {updateActions(); }
    void delDataTable();
    void slot_createMenus();
    void ToBeijing_triggered();
    //     void LeaveBeijing_triggered();
    void slot_showRailLine();/*
     void slot_LoadRailType();*/
    void slot_railTableDisplay(bool flag);
    void slot_expandUI(bool b);
private:
    Ui::RailLineEdit *ui;
    void updataRailSelect(bool flag);
    QMenu* editMenu;
    QMenu* selectSubMenu;

    bool   bDirection;  //当前查看的是上行表
    QString railLineName;   //当前的线路名称
    QString railTable[2];   //线路名中的实际数据库表名
    QString m_tableName;
    int excel(QString fileName );
    void disXXTable(void);
    void disSXTable(bool);

    QButtonGroup displayRDB;
    void loadFileRail(void);
    bool m_bUIVisible;
    void setExpand(bool flag);
    QString getField(QSqlQueryModel * query,QString titles,QString fields,bool );
    void updateRailInfo(void);
    QString curRail;
    SelRailLineDialog *selectdlg;
    QString getCurKm(int id);

signals:
    void railTableSelect(bool);
    void returnMainUI(void);
    //QSqlTableModel *model;
    void showMessage(QString);
private slots:
    void on_pbtnLoad_clicked();
    void on_pbtnUIExpand_clicked();
    void on_pbtnSet_clicked();
};

#endif // RAILLINEEDIT_H
