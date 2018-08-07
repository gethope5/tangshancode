#ifndef RECORDMEASUREFORM_H
#define RECORDMEASUREFORM_H

//#include <QWidget>
#include <qtgui>
//#include "excelcontrol.h"
#include "raillinesqlmodel.h"
#include <QtSql>
#include <QMessageBox>
#include "subexcelsave.h"
#include <QtWidgets>
#include <qtextcodec.h>
#include "database.h"
#include "excel/excelparse.h"
#include "modelpoledlg.h"

#define DEBUG_DATA 0
#define DEBUG_TITLE 1
namespace Ui {
    class recordMeasureForm;
}
struct recordPole
{
  long startId;
  long endId;

};
class recordMeasureForm : public QWidget
{
    Q_OBJECT
public:
    explicit recordMeasureForm(QString filePrefix,QWidget *parent = 0);
    ~recordMeasureForm();
    int setCurDatabase(QSqlDatabase * ,QString);
    void showDataTable(const QString &dataName);
    bool Opendatabase(const QString  &filepath);

    QStringList sOriginalDatas;
    void updateShowTable(bool flag);
    void  clearTable(void);
private slots:
    void on_pbtnDataOutput_clicked();
    void on_pbtnOpenFile_clicked();
    void slot_DisplayTable();
    void slot_csvSaveStatus(bool);
    void on_pbtnDataReplay_clicked();

    void on_pbtnUIExpand_clicked();
    void slot_expandUI(bool);
    void on_pbtnModifyPole_clicked();

    void on_pbtnUpdate_clicked();

    void on_pbtnOverdata_clicked();
    void slot_updateSimpleTable();
    void slot_DisplayData(void);
private:
    Ui::recordMeasureForm *ui;
    QVector<double> dDistance;
    QVector<double> dCarrierHeight;
    QVector<QPoint> wires1;
    QVector<QPoint> wires2;
    RailLineSqlModel *model;
    QSqlDatabase m_db;
//    excelControl setXLSFile;
    void modifyXLSFile();
    QProgressDialog *progress;
    QTableView *tableView;
    QString xlsTitleName;
    QString dataFilePaths;//存储数据文件的路径及文件名
    void exportCSVFile(QString filePaths);
    SubExcelSave subCSVFile;
    void getFileData(QString,QVector<QPoint> &,QVector<QPoint> &,QVector<double> &, QVector<QPair<double,int> > &);
    bool m_bUIVisible;

    void setExpand(bool flag);
    QVector<QPair<double,int> > poles;
    void modelPole(int startId,int endId,int pole);
    int getLimitHeight(int poleId,bool flag);
    void getDropperStep(double nWalkDistance,QVector<double> &dropperSteps,float curWalk);
    void parse_dropperGrade(QVector<double> isDropper,double startDist,double endDist);
    int nFirstDropperStep;
    int minDropperStep;
    int maxDropperStep;
    void readConfig(void);
    QVector<MeasureDebugData> prePoleData;
    QVector<QPair<int,QPair<double,QString> > > tbPoles;//id,poles
    void updateAllWireWidth(void);
    void update1WireWidth(MeasureIdEx_t &dval);
    int getRecordCount(QString tb);
    void updateSimpleTableId(void);
    bool indexPoleNum(QString poleNum);

    void updateAllMidWireWidth(void);
    bool indexMidWireWidth(QString poleNum);
	
    bool insertRecord(  MeasureIdEx_t  &dval);
    bool getDetailData(int id,MeasureIdEx_t & dval);
    void setSimpleTableCheck(bool flag);
    QString simpleTbFilter;
    QString detailTbFilter;
    void openDbUiInitial(void);
    bool deleteSimpleRecord(int id);
    void getFilePoles(QVector<QPair<double,int> > &poles);
    int getMidWire(double curDist);
    QPair<bool,double> prePoleDist;
    QVector<QPair<int ,double> > allDist;
    int getDistIndex(QString table);
    void getDataFromDb(QSqlQuery &query,QVector<QPoint> &wire1,QVector<QPoint> &wire2,QString strCarrier);
    void getDbType(void);
    QString strFileSavePath;
    QString sPathPrefix;
signals:
    void dataReplay(QVector<double> ,QVector<QPoint> ,QVector<QPoint> ,QVector<double> ,QVector<QPair<double,int> > &);
    void showMessage(QString );
protected:
    bool  eventFilter(QObject *o, QEvent *e );
};

#endif // RECORDMEASUREFORM_H
