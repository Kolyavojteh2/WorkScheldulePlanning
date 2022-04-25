#ifndef WORKERSINFORMATION_H
#define WORKERSINFORMATION_H

#include <QWidget>
#include <QString>
#include <QMenu>
#include "../../src/XML_Parsing/Worker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WorkersInformation; }
QT_END_NAMESPACE

class WorkersInformation : public QWidget
{
    Q_OBJECT

public:
    WorkersInformation(QWidget *parent = nullptr);
    WorkersInformation(const QString filename, QWidget *parent = nullptr);
    ~WorkersInformation();

private:
    Worker getInformationFromForm();
    void resetForm();
    void validateData();
    void openFile(const QString&);

    void setupWidget();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::WorkersInformation *ui;
    QMenu *p_contextMenu;

    QString m_filename;
    QString m_defaultPath;
    Worker m_oldVersion;

public slots:
    void slotOpen();
    void slotSave();
    void slotSaveAs();
    void slotClose();
    void slotCreateNew();
    void slotAddPosition();
    void slotDeletePosition();
    void slotContextMenu(const QPoint&);
    void slotChangeWindowTitle(const QString&);

    void slotSetDefaulPath(const QString&);
};
#endif // WORKERSINFORMATION_H
