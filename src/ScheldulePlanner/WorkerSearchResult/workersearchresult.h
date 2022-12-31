#ifndef WORKERSEARCHRESULT_H
#define WORKERSEARCHRESULT_H

#include <QWidget>
#include <QDialog>
#include <QRegExp>
#include <QStringList>
#include <QTreeWidgetItem>
#include <QMenu>
#include <QString>

#include "../../src/XML_Parsing/Worker.h"
#include "../../src/XML_Parsing/Positions.h"

namespace Ui {
class WorkerSearchResult;
}

class WorkerSearchResult : public QDialog
{
    Q_OBJECT

public:
    explicit WorkerSearchResult(QWidget *parent = nullptr);
    ~WorkerSearchResult();

private:
    Ui::WorkerSearchResult *ui;

    Workers m_listOfWorkers;
    Positions m_listOfPositions;

public slots:
    void setWorkers(Workers);
    void setPositions(Positions);
    void updateResults();
};

#endif // WORKERSEARCHRESULT_H
