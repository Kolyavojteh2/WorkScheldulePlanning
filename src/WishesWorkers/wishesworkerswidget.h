#ifndef WISHESWORKERSWIDGET_H
#define WISHESWORKERSWIDGET_H

#include <QWidget>
#include "../../src/XML_Parsing/SingleWish.h"
#include "../../src/XML_Parsing/Positions.h"
#include <QListWidget>
#include <QSignalMapper>

QT_BEGIN_NAMESPACE
namespace Ui { class WishesWorkersWidget; }
QT_END_NAMESPACE

class WishesWorkersWidget : public QWidget
{
    Q_OBJECT

public:
    WishesWorkersWidget(QWidget *parent = nullptr);
    ~WishesWorkersWidget();

private:
    void setupWidget();
    void validateData();

    void resetData();
    SingleWish getInformationFromForm();
    void setInformationToForm(const SingleWish&);

    int findLastIDInList();
    void openFile(const QString&);
    void loadPositions();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::WishesWorkersWidget *ui;

    Wishes m_wishes;
    QString m_currentEditWish;
    Positions m_positions;

    QString m_defaultPath;
    QString m_pathWorkersInformation;
    QString m_pathPositionsInformation;

    QString m_filename;
    bool isModifiedFile = false;

    QSignalMapper *p_editingMapper;

public slots:
    void slotAddWish();
    void slotDeleteWish();
    void slotEditWish(QListWidgetItem *item);

    void slotModifyFile();
    void slotChangedPosition(int);
    void slotChangedIDWorker(const QString &text);

    void slotCheckIDWorker();
    void slotUpdateData(const QString& attributeName);

    void slotCreateNew();
    void slotOpen();
    void slotSave();
    void slotSaveAs();

    void slotSetDefaulPath(const QString&);
    void slotSetPathWorkersInformation(const QString&);
    void slotSetPathPositionsInformation(const QString&);

    void slotShowNavigationButtons(bool showState);

};
#endif // WISHESWORKERSWIDGET_H
