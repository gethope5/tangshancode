#ifndef _DATA_BASE_H_
#define _DATA_BASE_H_

#include <QString>
#include <QStringList>

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include "syshead.h"
class CDataBase
{
	public:
        CDataBase();
        static QString railTableName;
        static QString title;
        static QString fields;
        virtual ~CDataBase();
		
        void initDb(void);
        bool addConnection(const QString &dbName, const QString &host,
                  const QString &user, const QString &passwd);

        /*create table*/
        void createTable(void);
        bool createRaiList(QString name, QString sxName, QString xxName);
        void createRailLine(QString name);

        /*insert data to 线路表*/
        bool insertRailLine(QString tbName, oneLineData_t & dval);



        void Transaction(void);
        void Commit(void);
        void Rollback(void);

        QSqlDatabase currDatabase() const;
        bool bConnected() { return beConnected; }
        bool emptyTable(QString tableName);
        void dropTables();
        bool updateRailList(QString railName,QString tableName,bool bDirection);
        bool isLineExist(QString railName);
        void dropLine(QString railName);

        QStringList getRailList(void);
        QStringList getRailListALL(void);

        QStringList getUserList(void);
        QString     getPasswd(QString user, bool *bok);
        int         getLoginLevel(QString user, bool *bok);

	protected:

    protected:
		bool beConnected;       
        QSqlDatabase m_db;
        void getTitleConfig(void);
};

class MeasureDB
{
public:
    MeasureDB(void);
    ~MeasureDB();
    void createDb(const QString& dbName);
    bool createTables( void);

    QSqlDatabase m_db;
    bool modifyRecord(int id, const QString& station,
                              const QString& pid,
                              const int structFlg);

    bool insertRecord_detailAndVerify(MeasureDebugData & dval,int nSide,bool flag=true);
    bool insertRecord_MeasureEx(MeasureIdEx_t & dval);
    bool insertRecord_overPars(MeasureDebugData & dval);

    void Transaction(void);
    void Commit(void);
    QSqlDatabase currDatabase(void);
    QString curDataName;
    QStringList tables(void);
    bool updateDetailTb(MeasureDebugData dval,int nSide,QString str);
    void isOpenDB(void);
    void isCloseDB(void);
    void closeDb(void);

    static int dbCount;
    static QString verifyTb;
    static QString verifyTitle;
    static QString verifyFields;

    static QString detailDataTb;
    static QString overDataTb;
    static QString simpleDataTb;

    static QString detailDataTitle;
    static QString detailOldTitle;
    static QString detailOldFields;
    static QString detailNewTitle;
    static QString detailNewFields;
    static QString detailTableFields;

    static QString overDataTitle;
    static QString overTableField;
    static QString simpleDataTitle;
    static QString simpleTableFields;

    static bool overAlarm;
    static bool bMeasure;       //true 测量中;false,暂停测量
    static bool bSimDraw;       //
    static bool bSimUI;         //调试调试窗口
    static bool bRadarStatus;   //传感器雷达连接完毕标志位，判断是否可以进行测量的标志
    static bool bRadarPower;
    static bool bVerify;        //测量、标定选择位。true,正常测量；false,测量参数标定
    static bool oneTableData;   //true,所有数据被存储在一张数据表格之中；false，单跨数据被存储在精检表格之中
    static bool moving;         //true,设备移动中；false，设备未移动
    static bool bPoleDir;       //杆号变化方向,true，增加;false,减少
    static int nPoleNum;
    static QString strFileData;
};

#endif


