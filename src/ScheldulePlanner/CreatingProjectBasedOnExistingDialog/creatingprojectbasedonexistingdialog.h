#ifndef CREATINGPROJECTBASEDONEXISTINGDIALOG_H
#define CREATINGPROJECTBASEDONEXISTINGDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QDateTime>
#include <QDir>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>

#include "../../src/XML_Parsing/ConfigurationCreationProjectBasedExisting.h"

namespace Ui {
class CreatingProjectBasedOnExistingDialog;
}

class CreatingProjectBasedOnExistingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreatingProjectBasedOnExistingDialog(QWidget *parent = nullptr);
    ~CreatingProjectBasedOnExistingDialog();

private:
    Ui::CreatingProjectBasedOnExistingDialog *ui;

    int duration;
    QString m_defaultProjectsPath;

signals:
    void definedConfiguration(const ConfigurationCreationProjectBasedExisting&);
    void rejected();

private slots:
    void enablingCheckBoxes();
    void changeDateTime();
    void checkExistingProject();
    void prepareConfiguration();

public slots:
    void setPeriod(const QDateTime& start, const QDateTime& end);
    void setDefaultProjectPath(const QString&);
};

#endif // CREATINGPROJECTBASEDONEXISTINGDIALOG_H
