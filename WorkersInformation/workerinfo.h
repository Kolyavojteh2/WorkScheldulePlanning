#ifndef WORKERINFO_H
#define WORKERINFO_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class WorkerInfo; }
QT_END_NAMESPACE

class WorkerInfo : public QWidget
{
    Q_OBJECT

public:
    WorkerInfo(QWidget *parent = nullptr);
    ~WorkerInfo();

private:
    Ui::WorkerInfo *ui;
};
#endif // WORKERINFO_H
