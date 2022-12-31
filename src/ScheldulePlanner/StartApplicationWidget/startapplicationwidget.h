#ifndef STARTAPPLICATIONWIDGET_H
#define STARTAPPLICATIONWIDGET_H

#include <QWidget>
#include <QFile>
#include <QDir>
#include <QList>
#include <QPushButton>
#include <QSpacerItem>

namespace Ui {
class StartApplicationWidget;
}

class StartApplicationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StartApplicationWidget(const QString& path, QWidget *parent = nullptr);
    ~StartApplicationWidget();

signals:
    void clickedOpenLastProject(const QString&);
    void clickedOpenProject();
    void clickedCreateNewProject();
    void clickedCreateNewProjectBasedOnTheExisting();

private:
    Ui::StartApplicationWidget *ui;
    QString m_projectsPath;

private slots:
    void sendLastProject();


};

#endif // STARTAPPLICATIONWIDGET_H
