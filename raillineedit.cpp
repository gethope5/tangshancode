﻿#include "raillineedit.h"
#include "ui_raillineedit.h"

#include "global.h"

#include <QDebug>

RailLineEdit::RailLineEdit(QWidget *parent) :
    QWidget(parent)
  ,bDirection(true)
{
//    ui->setupUi(this);

    setupUi(this);

    //model = new QSqlTableModel(tableView, Global::getInstance()->gbase.currDatabase());
    //    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),this, SLOT(currentChanged()));
    //    connect(tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slot_createMenus()));
    connect(this->pbtnDelete, SIGNAL(clicked()), this, SLOT(delDataTable()));
    connect(this->pbtnView, SIGNAL(clicked()), this, SLOT(slot_showRailLine()));
    connect(actionInsert, SIGNAL(triggered()), this, SLOT(Insert_triggered()));
    connect(actionDelRow, SIGNAL(triggered()), this, SLOT(DelRow_triggered()));
    //关联上下行表格显示信号与槽
    connect(actionToBeijing, SIGNAL(triggered()), this, SLOT(ToBeijing_triggered()));
    connect(actionLeftBeijing, SIGNAL(triggered()), this, SLOT(ToBeijing_triggered()));

    connect(this,SIGNAL(railTableSelect(bool)),this,SLOT(slot_railTableDisplay(bool)));
    this->pbtnLoad->initialPbtnUI("./images/railwayAdd-focus.png","./images/railwayAdd.png");
    this->pbtnView->initialPbtnUI("./images/railwaySelect-focus.png","./images/railwaySelect.png");
    this->pbtnDelete->initialPbtnUI("./images/railwayDelete-focus.png","./images/railwayDelete.png");
    this->pbtnSet->initialPbtnUI("./images/uiSet-focus.png","./images/uiSet.png");
}

RailLineEdit::~RailLineEdit()
{

}
void RailLineEdit::slot_showRailLine()
{
    //1.获取当前线路数据库中所有的线路信息
    int currIdx = -1;
    QStringList list = Global::getInstance()->gbase.getRailList();
    qDebug()<<"table list"<<list;
    for( int i=0; i<list.size(); i++)
        qDebug()<<"++++ Railline list:"<<i<< list.at(i);
    if (list.isEmpty())
    {
        return;
    }
    if(!railLineName.isNull())
        currIdx = list.indexOf(railLineName);
    else
    {
        railLineName=list.at(0);
        currIdx=0;
    }
    qDebug()<<"rail name"<<railLineName<<currIdx;
    //2.调用选取线路对话框
    selectdlg=new SelRailLineDialog(bDirection,this);
    selectdlg->move(this->width()/2-selectdlg->width()/2,this->height()/2-selectdlg->height()/2);
    selectdlg->setRailList(list, currIdx);
    selectdlg->setDirect(bDirection);
    if (selectdlg->exec())
    {
        bDirection=selectdlg->bDirection;
        int idx = selectdlg->getselIndx();
        curRail=list.at(idx);
        //qDebug()<<"idx value"<<idx<<currIdx<<curRail<<bDirection;
        //if (idx != currIdx)
        updateRailLine(list[idx]);
    }
}
//函数功能：将线路信息进行更新显示
void RailLineEdit::updateRailInfo(void)
{
    if(curRail.isNull())
        return ;
    if(bDirection)
    {
        //        ui->lblRail->setText("jingshen");
        //        ui->lblRail->setText("1");
        emit showMessage(curRail+"  "+QString::fromWCharArray(L"上行"));
        //        qDebug()<<"cur rail "<<curRail+" 2" +QString::fromWCharArray(L"上行");
    }
    else
    {
        //        ui->lblRail->setText("2");
        emit showMessage(curRail+"  "+QString::fromWCharArray(L"下行"));
        //        qDebug()<<"cur rail "<<curRail+" 2" +QString::fromWCharArray(L"下行");
    }
}
void RailLineEdit::updateRailLine(QString railName)
{
    QSqlQuery query(Global::getInstance()->gbase.currDatabase());
    QString cmd = "SELECT * FROM railList where name = \'" + railName + "\'";
    qDebug()<<"the cmd is : " << cmd;
    if(query.exec(cmd))
    {
        if (query.next())
        {
            railLineName = railName;
            railTable[0] = query.value(1).toString();
            railTable[1] = query.value(2).toString();
            if (bDirection)
            {
                m_tableName = railTable[0];
                actionToBeijing->setChecked(true);
                actionLeftBeijing->setChecked(false);
            }
            else
            {
                actionToBeijing->setChecked(false);
                actionLeftBeijing->setChecked(true);
                m_tableName = railTable[1];
            }
            qDebug()<<"cur table namt"<<m_tableName ;
            showDataTable(m_tableName);
        }
    }
    else
    {
        qDebug()<<"table query is not ok";
    }
}
void RailLineEdit::slot_createMenus(void)
{
    QMenu em(this);//editMenu = new QMenu();
    selectSubMenu = em.addMenu(QTextCodec::codecForName("gbk")->toUnicode("选择线路表"));
    selectSubMenu->addAction(actionToBeijing);
    selectSubMenu->addAction(actionLeftBeijing);

    em.addSeparator();
    em.addAction(actionInsert);
    em.addAction(actionDelRow);
    actionToBeijing->setChecked(bDirection);
    actionLeftBeijing->setChecked(!bDirection);
    em.exec(QCursor::pos());
}

void RailLineEdit::disSXTable(bool flag)
{
    if(!railLineName.isEmpty())
    {
        if(flag)
        {
            m_tableName = railTable[0];
            showDataTable(m_tableName);
        }
        else
        {
            m_tableName = railTable[1];
            showDataTable(m_tableName);
        }
    }
}

void RailLineEdit::ToBeijing_triggered()
{
    QAction *rdb=(QAction*)this->sender();
    if(rdb==actionToBeijing)
    {
        actionToBeijing->setChecked(true);
        emit railTableSelect(true);
    }
    else if(rdb==actionLeftBeijing)
    {
        actionLeftBeijing->setChecked(true);
        emit railTableSelect(false);
    }
}
void RailLineEdit::slot_railTableDisplay(bool flag)
{
    disSXTable(flag);
    //    updataRailSelect(flag);
}
void RailLineEdit::updataRailSelect(bool flag)
{
    if(flag)
    {
        actionToBeijing->setChecked(true);
    }
    else
    {
        actionLeftBeijing->setChecked(true);
    }
}
void RailLineEdit::delDataTable()
{
    //用于存放需要删除的上下行表表格的名字
    QStringList DeleteTable;
    if (m_tableName.isEmpty())
    {
        QMessageBox::information(this, QTextCodec::codecForName("gbk")->toUnicode("提示"), QTextCodec::codecForName("gbk")->toUnicode("请选择要删除的线路表!"));
        return;
    }

    int ret = QMessageBox::question(this, QTextCodec::codecForName("gbk")->toUnicode("提示"), QTextCodec::codecForName("gbk")->toUnicode("确定要删除吗？"),
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::Yes);


    if( QMessageBox::No  == ret )
    {
        return;
    }

    QSqlQuery query(Global::getInstance()->gbase.currDatabase());
    QString QueryrailLineName= "SELECT * FROM railList where name = \"" + railLineName + "\"";
    if(query.exec(QueryrailLineName))
    {
        while(query.next())
        {
            qDebug()  <<"railLineName"<<query.value(0).toString();
            DeleteTable<<query.value(1).toString();
            DeleteTable<<query.value(2).toString();
            break;
        }
    }
    else
        qDebug()  <<"railLineName is not";

    int count;

    for (int i=0;i<DeleteTable.size();i++)
    {
        if(query.exec(QString("SELECT * FROM %1").arg(DeleteTable.at(i))))
            if(query.last())
                count=query.value(0).toInt();
        if(query.exec(QString("delete from %1 where ID <= %2").arg(DeleteTable.at(i)).arg(count)))
            qDebug()<<"the table"<<DeleteTable.at(i)<<"is deleted!";
        else
            qDebug()<<"the table"<<DeleteTable.at(i)<<"is not deleted!";
    }
    query.finish();

    if(query.exec(QString("delete from railList where name=\"%1\"").arg(railLineName)))

        qDebug()<<railLineName<<"is deleted from the table railList!"<<endl;
    else
        qDebug()<<railLineName<<"is not deleted from the table railList!"<<endl;
    showDataTable(m_tableName);
    tableView->reset();
}

void RailLineEdit::insertRow()
{
#if 0

#ifdef  __QT4_7_VERSION__
    QElapsedTimer tm;
    tm.start ();

    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    QModelIndex insertIndex = tableView->currentIndex();
    int row = insertIndex.row() == -1 ? 0 : insertIndex.row() ;
    //    qDebug()<<"insert row "<<row;
    model->updateRowIndex(row, true);

    qDebug()<<"model : "<<model->lastError();
    qDebug() << "The slow operation took" << tm.elapsed();
    tm.restart();
    model->submitAll();
    model->setEditStrategy(QSqlTableModel::OnRowChange);
    //    qDebug() << "The slow operation took == " << tm.elapsed();
    tm.restart();
    model->insertRow(row);
    model->setPrimKeyAsRow(row);

    insertIndex = model->index(row, 1);
    tableView->setCurrentIndex(insertIndex);
    tableView->edit(insertIndex);

    //    qDebug() << "The slow operation took" << tm.elapsed();
#endif
#else

#ifdef  __QT4_7_VERSION__
    QElapsedTimer tm;
    tm.start ();

    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    QModelIndex insertIndex = tableView->currentIndex();
    int row = insertIndex.row();
    if(row==-1)
        return;
    else
    {
        QSqlQuery cc=model->query();
        QString update=QString("update %1 set id=id+1 where id>%2").arg(model->tableName()).arg(row+1);
        qDebug()<<"update value"<<update;
        cc.exec(update);
        showDataTable(m_tableName);
        row++;
    }
    //        qDebug()<<"model : "<<model->lastError();
    //        qDebug() << "The slow operation took" << tm.elapsed();
    tm.restart();
    model->submitAll();
    model->setEditStrategy(QSqlTableModel::OnRowChange);
    tm.restart();
    model->insertRow(row);
    model->setPrimKeyAsRow(row);

    insertIndex = model->index(row, 1);
    tableView->setCurrentIndex(insertIndex);
    tableView->edit(insertIndex);
#endif
#endif
}
//函数功能:删除记录
//       2012-09-24:采用sql语句对记录进行删除,对记录id的更新也采用sql语句
//                  (原来的方式是通过QSqlTableModel中的RemoveRow删除,通过setData更新id,不知为何,不成功)
void RailLineEdit::deleteRow()
{
#if 1
    int ret = QMessageBox::question(this, QTextCodec::codecForName("gbk")->toUnicode("提示"), QTextCodec::codecForName("gbk")->toUnicode("确定要删除当前记录吗？"),QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes);
    if( QMessageBox::No  == ret )
    {
        return;
    }
    int CurrentRowID=tableView->currentIndex().row()+1;
    //    QSqlQuery cc=model->query();
    //    cc.exec(QString("delete from %1 where id=%2").arg(model->tableName()).arg(CurrentRowID));
    //    cc.exec(QString("update %1 set id=id-1 where id>%2").arg(model->tableName()).arg(CurrentRowID));

    showDataTable(m_tableName);
#else
    qDebug()<<"delete is "<<model->DeleteRecord(tableView->currentIndex().row());
    model->submitAll();
#endif
}
void RailLineEdit::showDataTable(const QString &tableName)
{
    updateRailInfo();
    QSqlQueryModel *queryModel = new QSqlQueryModel(this);
    QSqlQuery query(Global::getInstance()->gbase.currDatabase());
    QString strSql;
    QString fields;
    fields=getField( queryModel,CDataBase::title,CDataBase::fields,false);

    strSql=QString("SELECT %1 from %2").arg(fields).arg(tableName);
    bool bExec=query.exec(strSql);
    if(!bExec)
        qDebug()<<"error query"<<bExec<<strSql;
    queryModel->setQuery(query);

    while (queryModel->canFetchMore())
        queryModel->fetchMore();
    getField( queryModel,CDataBase::title,CDataBase::fields,true);
    this->tableView->setModel(queryModel);
    updateActions();
}
//函数功能：获得sql的field字段(false)或设置显示标题栏
QString RailLineEdit::getField(QSqlQueryModel * query,QString titles,QString fields,bool flag)
{
    QStringList tmp1;
    int j=0;
    for(int i=0;i<titles.split(',').count();i++)
    {
        QString tmp=titles.split(',').at(i);
        if(tmp.right(1).toInt()==1)
        {
            if (flag)
            {
                //qDebug()<<"title"<<i-j<<i;
                query->setHeaderData(i-j,Qt::Horizontal, tmp.left(tmp.count()-1));
            }
            else
            {
                tmp1<<fields.split(',').at(i);//+",";//.left(tmp.count()-1);
            }
        }
        else
            j++;
    }
    //    qDebug()<<"fields="<<tmp1;//<<CDataBase::title;
    return tmp1.join(',');
}
void RailLineEdit::updateActions()
{
    bool enableIns = qobject_cast<QSqlTableModel *>(tableView->model());
    bool enableDel = enableIns && tableView->currentIndex().isValid();

    actionInsert->setEnabled(enableIns);
    actionDelRow->setEnabled(true);
}

//void RailLineEdit::on_pbtnExport_clicked()
//{
//    const QString filepath = QFileDialog::getSaveFileName(this, QTextCodec::codecForName("gbk")->toUnicode("加载xls文档"), "", QTextCodec::codecForName("gbk")->toUnicode("xls文档 (*.xls)"));
//    //    filepath=QString("c:/6.xls");
//    if (filepath.isEmpty())
//    {
//        return;
//    }twoplotform.obj:-1: error: LNK2038: 检测到“_ITERATOR_DEBUG_LEVEL”的不匹配项: 值“0”不匹配值“2”(main.obj 中)

//    //打开文件，取得工作簿
//    excelControl j(filepath );
//    qDebug()<<"filepath="<<filepath;
//    const QString xlsTitle(QTextCodec::codecForName("gbk")->toUnicode("编号,站区名称,隧道名称,支柱号,跨距,结构标志,曲线标志,公里标,供电段名称,车间名称,工区名称,导高标准值,拉出标准值"));
//    j.exportXLS(Global::getInstance()->gbase.currDatabase(),(model->tableName()),filepath,xlsTitle);
////    int sheetCounts=j.getSheetsCount();
////    QString sheetName;
////    if(sheetCounts)
////        sheetName=j.getSheetName(1);
////    else
////        return;
////    j.selectSheet(sheetName);


////    QSqlQuery query(Global::getInstance()->gbase.currDatabase());
////    QSqlQuery quer;
////    query.exec(QString("select * from %1").arg(model->tableName()));
////    int coloumnCount=query.record().count();
////    qDebug()<<"record count="<<coloumnCount;

////    QString xlsTitle(QTextCodec::codecForName("gbk")->toUnicode("编号,站区名称,隧道名称,支柱号,跨距,结构标志,曲线标志,公里标,供电段名称,车间名称,工区名称,导高标准值,拉出标准值"));

////    j.selectSheet(1);
////    for(int i=1;i<coloumnCount+1;i++)
////    {
////        j.setCellString(1, i, QString(xlsTitle.split(",").at(i-1)));
////    }
////    query.exec(QString("select * from %1").arg(model->tableName()));
////    int rowCount=2;
////    while(query.next())
////    {
////        for(int i=1;i<14;i++)
////        {
//////            qDebug()<<"insert value="<<rowCount<<i<<query.value(i).toString();
////            j.setCellString(rowCount, i, QString(query.value(i-1).toString()));
////        }
////        rowCount++;
////    }

////    //插入数据

////    //        j.selectSheet("abc");

////    //        j.setCellString(1, 7, "addString");

////    j.save(filepath);
////    j.close();
////    qDebug()<<"App End";
//}

void RailLineEdit::on_pbtnLoad_clicked()
{
    loadFileRail();
}
void RailLineEdit::loadFileRail(void)
{
    QString filepath = QFileDialog::getOpenFileName(this, QTextCodec::codecForName("gbk")->toUnicode("加载xls文档"), "", QTextCodec::codecForName("gbk")->toUnicode("xls文档 (*.xls)"));
    if(filepath.isEmpty())
    {
        return;
    }

    qDebug()<<"1. the filepath : " << filepath;
    QFileInfo fi(filepath);

    QString filename = fi.completeBaseName ();
    QString tiptext = QTextCodec::codecForName("gbk")->toUnicode("此线路") + filename + QTextCodec::codecForName("gbk")->toUnicode("已存在，加载后会覆盖原线路<br />是否继续加载");
    qDebug()<<"filename"<<filename;

    if (Global::getInstance()->gbase.isLineExist(filename))
    {
        if(QMessageBox::information(this,"tishi",tiptext,QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel)==QMessageBox::Ok)
        {
            qDebug()<<"dlg enter" << filename;
            Global::getInstance()->gbase.dropLine(filename);
        }
        else
        {

            qDebug()<<"dlg CANCEL";
            return;
        }
    }
#if 0
    QApplication::setOverrideCursor(Qt::WaitCursor);
    //开始导入XLS格式的线路基础数据
    excelControl inportXLSFile;
    inportXLSFile.inportXLS(filepath,filename);
    //开始导入结束
    updateRailLine(filename);
    QApplication::restoreOverrideCursor();
#else
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QElapsedTimer timer;
    timer.start();
    ExcelParse   loadEnginner;

    errcode pfret = loadEnginner.parseFile(filepath.toUtf8().constData());
    if (ERR_NORMAL != pfret)
    {
        //        TipDialog td(this, QTextCodec::codecForName("gbk")->toUnicode(ExcelParse::convertErrcode(pfret)));
        //        td.exec();
        qDebug(" Load failure!"); /*2011-11-13 */
    }
    else
    {
        qDebug(" Load success!");  /*2011-11-13 */
        updateRailLine(filename);
    }
    qDebug()<<"timer value="<<timer.elapsed();
    QApplication::restoreOverrideCursor();
#endif
}
void RailLineEdit::on_pbtnUIExpand_clicked()
{
    setExpand(m_bUIVisible);
    m_bUIVisible=!m_bUIVisible;
}
void RailLineEdit::slot_expandUI(bool b)
{
    setExpand(b);
    m_bUIVisible=b;
}
void RailLineEdit::setExpand(bool flag)
{
    if(flag)
    {
        this->groupBox->setVisible(false);
        this->pbtnUIExpand->setText("<<");
    }
    else
    {
        this->groupBox->setVisible(true);
        this->pbtnUIExpand->setText(">>");
    }
}
void RailLineEdit::on_pbtnSet_clicked()
{
    tableUIDlg dlg(CDataBase::title,this) ;
    if(dlg.exec())
    {
        CDataBase::title=dlg.titleName;
        showDataTable(m_tableName);
    }
}
