#ifndef PROJECTSETUPWIDGET_H
#define PROJECTSETUPWIDGET_H

#include <QWidget>
#include <QDialog>
#include "../../src/XML_Parsing/ProjectSetup.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ProjectSetupWidget; }
QT_END_NAMESPACE

class ProjectSetupWidget : public QDialog
{
    Q_OBJECT

public:
    ProjectSetupWidget(QWidget *parent = nullptr);
    ~ProjectSetupWidget();

private:
    void setupWidget();
    ProjectSetup getInformationFromForm();
    void setInformationToForm(const ProjectSetup&);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::ProjectSetupWidget *ui;

    bool isSaved = false;

signals:
    void signalSendConfiguration(ProjectSetup);

public slots:
    void slotTimeModified(const QTime&);
    void slotDateTimeModified(const QDateTime&);

    void slotSave();
    void slotSetConfiguration(ProjectSetup);
};
#endif // PROJECTSETUPWIDGET_H
