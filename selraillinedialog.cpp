#include "selraillinedialog.h"
#include "ui_selraillinedialog.h"

SelRailLineDialog::SelRailLineDialog(bool direct,QWidget *parent) :
    QDialog(parent),
    bDirection(true),
    ui(new Ui::SelRailLineDialog)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    bDirection=direct;

    pbkGrnd = new QLabel(this);
    pbkGrnd->setPixmap(QPixmap("res/backgrnddlg.png"));
    pbkGrnd->setScaledContents(true);

    ui->setupUi(this);
    pbkGrnd->resize(size().width(), size().height());
    setDirect(direct);
    connect(ui->pbtnEnter, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->pbtnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->rdbSX,SIGNAL( clicked()),this,SLOT(slot_RailDirChanged()));
    connect(ui->rdbXX,SIGNAL( clicked()),this,SLOT(slot_RailDirChanged()));
}
void SelRailLineDialog::setDirect(bool b)
{
    if(b)
    {
        ui->rdbSX->setChecked(true);
    }
    else
    {
        ui->rdbXX->setChecked(true);
    }
}
//idx = -1 表示现在没有选定表格
void SelRailLineDialog::setRailList(QStringList &list, int idx)
{
    qDebug()<<"I got the railist "<<list;
    ui->rlCBox->addItems(list);
    ui->rlCBox->setCurrentIndex(idx == -1 ? 0 : idx);
}

int SelRailLineDialog::getselIndx()
{
    return ui->rlCBox->currentIndex();

}
QString SelRailLineDialog::curRail(void)
{
    return ui->rlCBox->currentText();
}
SelRailLineDialog::~SelRailLineDialog()
{
    delete ui;
}
void SelRailLineDialog::slot_RailDirChanged(void)
{
    if(ui->rdbSX->isChecked())
        bDirection=true;
    if(ui->rdbXX->isChecked())
        bDirection=false;
    qDebug()<<"direct"<<bDirection;
}
