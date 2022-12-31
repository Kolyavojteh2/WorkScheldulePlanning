#include "startapplicationwidget.h"
#include "ui_startapplicationwidget.h"

StartApplicationWidget::StartApplicationWidget(const QString& path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartApplicationWidget),
    m_projectsPath(path)
{
    ui->setupUi(this);

    connect(ui->p_button_CreateNewProject, SIGNAL(clicked()),
            this, SIGNAL(clickedCreateNewProject()));
    connect(ui->p_button_CreateNewProjectBasedExisting, SIGNAL(clicked()),
            this, SIGNAL(clickedCreateNewProjectBasedOnTheExisting()));
    connect(ui->p_button_OpenProject, SIGNAL(clicked()),
            this, SIGNAL(clickedOpenProject()));

    QDir projectDir(m_projectsPath);
    QList<QString> listOfProjects;
    listOfProjects = projectDir.entryList(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot, QDir::SortFlag::Time);

    if (!listOfProjects.isEmpty())
    {
        QLabel *lbl_RecentlyUsed = new QLabel(tr("<h4>Recently used projects:</h4>"));
        ui->p_layout_LastProjects->addWidget(lbl_RecentlyUsed);

        for (auto it : listOfProjects)
        {
            QString itemText;
            itemText.append(it);

            QPushButton *item = new QPushButton(itemText);
            item->setFlat(true);
            item->setCursor(Qt::CursorShape::PointingHandCursor);
            item->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
            connect(item, SIGNAL(clicked()),
                    this, SLOT(sendLastProject()));

            ui->p_layout_LastProjects->addWidget(item);
        }
    }

    QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->p_layout_LastProjects->addSpacerItem(spacer);

    this->setStyleSheet("QPushButton {"
                        "text-align: left;"
                        "text-decoration: underline;"
                        "padding-top: 5;"
                        "padding-right: 10;"
                        "padding-bottom: 5;"
                        "padding-left: 10;"
                        "}");
}

void StartApplicationWidget::sendLastProject()
{
    QPushButton *button = dynamic_cast<QPushButton*>(sender());
    emit clickedOpenLastProject(button->text());
}

StartApplicationWidget::~StartApplicationWidget()
{
    delete ui;
}
