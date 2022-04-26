#ifndef PROJECTSETUPWIDGET_H
#define PROJECTSETUPWIDGET_H

#include <QWidget>
#include "../../src/XML_Parsing/ProjectSetup.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ProjectSetupWidget; }
QT_END_NAMESPACE

class ProjectSetupWidget : public QWidget
{
    Q_OBJECT

public:
    ProjectSetupWidget(QWidget *parent = nullptr);
    ~ProjectSetupWidget();

private:
    void setupWidget();
    ProjectSetup getInformationFromForm();
    void setInformationToForm(const ProjectSetup&);
    void validateData();
    void openFile(const QString&);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::ProjectSetupWidget *ui;

    QString m_filename;
    bool isModifiedFile = false;

    QString m_defaultPath;

public slots:
    void slotRoundTheClockModeChanged(int);
    void slotEveryMounthGraphicChanged(int);

    void slotTimeModified(const QTime&);
    void slotSpinBoxOrCheckBoxModified(int);

    void slotModifyFile();

    void slotOpen();
    void slotSave();
    void slotSaveAs();

    void slotShowNavigationButtons(bool showState);

    void slotSetDefaulPath(const QString&);

};
#endif // PROJECTSETUPWIDGET_H
