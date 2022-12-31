#ifndef WISHESWORKERSWIDGET_H
#define WISHESWORKERSWIDGET_H

#include <QWidget>
#include <QDialog>
#include "../../src/XML_Parsing/SingleWish.h"
#include "../../src/XML_Parsing/Positions.h"
#include <QListWidget>
#include <QSignalMapper>

#include "../projectdataloader.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WishesWorkersWidget; }
QT_END_NAMESPACE

class WishesWorkersWidget : public QDialog
{
    Q_OBJECT

public:
    WishesWorkersWidget(QWidget *parent = nullptr);
    int getWorkerID() const;
    ~WishesWorkersWidget();

private:
    void setupWidget();
    void validateData();

    void resetData();
    SingleWish getInformationFromForm();
    void setInformationToForm(const SingleWish&);

    int findLastIDInList();
    //void openFile(const QString&);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::WishesWorkersWidget *ui;

    Wishes m_wishes;
    QString m_currentEditWish;
    Positions m_positions;

    //QString m_defaultPath;
    QString m_pathWorkersInformation;
    QString m_pathPositionsInformation;

    QString m_filename;
    //bool isModifiedFile = false;
    bool isSaved = false;
    bool isDisableWorkerID = false;

    QSignalMapper *p_editingMapper;

signals:
    void signalSendWishes(Wishes);

public slots:
    void slotAddWish();
    void slotDeleteWish();
    void slotEditWish(QListWidgetItem *item);

    void slotChangedPosition(int);
    void slotChangedIDWorker(const QString &text);

    void slotCheckIDWorker();
    void slotUpdateData(const QString& attributeName);

    void slotSave();

    void slotSetPathWorkersInformation(const QString&);

    void slotSetWishes(Wishes);
    void slotDisableWorkerID(bool);
    void slotSetWorkerID(int);

};
#endif // WISHESWORKERSWIDGET_H
