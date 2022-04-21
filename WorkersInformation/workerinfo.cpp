#include "workerinfo.h"
#include "ui_workerinfo.h"

WorkerInfo::WorkerInfo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WorkerInfo)
{
    ui->setupUi(this);
}

WorkerInfo::~WorkerInfo()
{
    delete ui;
}

