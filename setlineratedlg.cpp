#include "setlineratedlg.h"
#include "ui_setlineratedlg.h"
#include <qdebug.h>
setLineRateDlg::setLineRateDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setLineRateDlg)
{
    ui->setupUi(this);
    connect(ui->pbtn1,SIGNAL(clicked()),this,SLOT(slot_setRate()));
    connect(ui->pbtn2,SIGNAL(clicked()),this,SLOT(slot_setRate()));
    connect(ui->pbtn3,SIGNAL(clicked()),this,SLOT(slot_setRate()));
    connect(ui->pbtn4,SIGNAL(clicked()),this,SLOT(slot_setRate()));
    connect(ui->pbtn5,SIGNAL(clicked()),this,SLOT(slot_setRate()));
    connect(ui->pbtn6,SIGNAL(clicked()),this,SLOT(slot_setRate()));
    connect(ui->pbtn7,SIGNAL(clicked()),this,SLOT(slot_setRate()));
    connect(ui->pbtn8,SIGNAL(clicked()),this,SLOT(slot_setRate()));
    connect(ui->pbtnExposure,SIGNAL(clicked()),this,SLOT(slot_setRate()));
}
setLineRateDlg::~setLineRateDlg()
{
    delete ui;
}
void setLineRateDlg::slot_Expousre(eDeviceType type,QString value)
{
    switch(type)
    {
    case eVideo1:
        ui->lblExposure1->setText( value);
        break;
    case eVideo2:
        ui->lblExposure2->setText( value);
        break;
    }
}
void setLineRateDlg::slot_setRate(void)
{
    QPushButton *pbtn=(QPushButton *)this->sender();
    if(pbtn==ui->pbtnExposure)
    {
        emit setExpousre(eVideo1,ui->lblExposure1->text());
        emit setExpousre(eVideo2,ui->lblExposure2->text());
    }
    else
    {
        qDebug()<<"cur speed"<<pbtn->text().left(pbtn->text().indexOf("("));
        emit setRate(eVideo1,pbtn->text().left(pbtn->text().indexOf("(")));
        emit setRate(eVideo2,pbtn->text().left(pbtn->text().indexOf("(")));
    }
}
void setLineRateDlg::slot_lineRate(eDeviceType type,QString value)
{
    value=QString::number(3.6*value.toFloat()/1000.0);
    qDebug()<<"get line rate"<<type<<value;
    switch (type)
    {
    case eVideo1:
        ui->lneLineRate1->setText(value);
        break;
    case eVideo2:
        ui->lneLineRate2->setText(value);
        break;
    }
}
