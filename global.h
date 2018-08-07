#ifndef GLOBAL_H
#define GLOBAL_H

#include "database.h"
#include "prerunsettingdlg.h"
//#include <QSqlRecord>
class PoleBase{
public:
    QString station;
    QString tunnel;
    QString poleID;
    double   dist;
    double   sum_dist;
    int     structFlag;
};
class CalcPole
{
public:
    ulong nID;          //数据库中对应的ID
    QString poleID;     //pole ID
    double sum_dist;    //该处的里程

};
class Global;
class Global
{
public:  

    ~Global();
//    void setVelocityThread(GetVelocity *pthread);
    CDataBase gbase;


    static Global*  getInstance();
//    GetVelocity* getVelocity()
//    {
//        return thread;
//    }
    void setPostionAsProfile(RunProfile& profile);
    PoleBase getPoleBase(int idx) const
    {
        PoleBase tmp;
        if(idx<polesets.count())
        {
            return polesets[idx];
        }
        else
            return tmp;
    }
    QVector<PoleBase>& getPoleSets()
    {
        return polesets;
    }

    void setSelectFilePath( QString strPath )
    {
        m_strDatabasePath = strPath;
    }

    QString  getSelectFilePath( )
    {
        return m_strDatabasePath;
    }

public:
    QVector<CalcPole> calcPolesets;
    QVector<double> changedPos;

private:
    QString  m_strDatabasePath;

protected:
    static Global * pinstance;
//    GetVelocity *thread;
    Global();


protected:
    QVector<PoleBase> polesets;
       //计算出来的支柱序列
};

#endif // GLOBAL_H
