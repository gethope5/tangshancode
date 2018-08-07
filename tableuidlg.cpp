#include "tableuidlg.h"
#include "ui_tableuidlg.h"

tableUIDlg::tableUIDlg(QString title,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tableUIDlg)
{
    ui->setupUi(this);
    titleName=title;
    check.clear();
    check<<ui->chb1;
    check<<ui->chb2;
    check<<ui->chb3;
    check<<ui->chb4;
    check<<ui->chb5;
    check<<ui->chb6;
    check<<ui->chb7;
    check<<ui->chb8;
    check<<ui->chb9;
    check<<ui->chb10;
    check<<ui->chb11;
    check<<ui->chb12;
    check<<ui->chb13;
    check<<ui->chb14;
    check<<ui->chb15;
    check<<ui->chb16;
    check<<ui->chb17;
    check<<ui->chb18;
    check<<ui->chb19;
    check<<ui->chb20;


    QString tmp;

    for(int i=0;i<check.count();i++)
    {
        QCheckBox *chk=check.at(i);
        if(i<titleName.split(",").count())
        {
            tmp=titleName.split(",").at(i);
            chk->setText(tmp.left(tmp.count()-1));
            if(tmp.right(1).toInt())
            {
                check.at(i)->setCheckState(Qt::Checked);
            }
            else
            {
                check.at(i)->setCheckState(Qt::Unchecked);
            }
        }
        else
        {
            check.at(i)->setVisible(false);
        }
    }
    this->setWindowTitle(QString::fromWCharArray(L"线路设置"));
}

tableUIDlg::~tableUIDlg()
{
    delete ui;
}
void tableUIDlg::on_pbtnOk_clicked()
{
    QStringList titleList=titleName.split(",");
    QStringList titleSave;
    for(int i=0;i<titleList.count();i++)
    {
        QString tmp=titleList.at(i);
        if(check.at(i)->checkState()==Qt::Checked)
        {
            tmp.replace(tmp.count()-1,1,"1");
            titleList.replace(i,tmp);
            titleSave<<"1";
        }
        else
        {
            tmp.replace(tmp.count()-1,1,"0");
            titleList.replace(i,tmp);
            titleSave<<"0";
        }
    }
    titleName=titleList.join(",");
    //    qDebug()<<"titleName"<<titleName;
    saveTitle(titleSave.join(","));
    accept();
}

void tableUIDlg::saveTitle(QString mm)
{
    QSettings setting("Config.ini",QSettings::IniFormat);
    setting.setValue("system/railTitle",mm);
}
void tableUIDlg::on_chbCancel_clicked()
{
    reject();
}
