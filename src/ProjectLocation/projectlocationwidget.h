#ifndef PROJECTLOCATIONWIDGET_H
#define PROJECTLOCATIONWIDGET_H

#include <QWidget>
#include <QSignalMapper>
#include "../../src/XML_Parsing/ProjectLocations.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ProjectLocationWidget; }
QT_END_NAMESPACE

class ProjectLocationWidget : public QWidget
{
    Q_OBJECT

public:
    ProjectLocationWidget(QWidget *parent = nullptr);
    ~ProjectLocationWidget();

private:
    void setupWidget();
    void validateData();
    ProjectLocations getInformationFromForm();
    void setInformationToForm(const ProjectLocations& info);
    void openFile(const QString &filename);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::ProjectLocationWidget *ui;

    QString m_filename;
    bool isModifiedFile = false;
    QSignalMapper* m_sigmapperButtons;

    QString m_defaultPath;

public slots:
    void slotGetDirPath(QWidget*);
    void slotModifyFile(const QString&);

    void slotOpen();
    void slotSave();
    void slotSaveAs();

    void slotShowNavigationButtons(bool showState);

    void slotSetDefaulPath(const QString&);
};
#endif // PROJECTLOCATIONWIDGET_H
