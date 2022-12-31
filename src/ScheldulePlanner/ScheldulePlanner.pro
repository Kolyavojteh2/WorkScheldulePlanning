QT       += core gui xml charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AutomatedAlgorithmsBuildingSchedules/automatedalgorithmsbuildingschedules.cpp \
    CreatingProjectBasedOnExistingDialog/creatingprojectbasedonexistingdialog.cpp \
    DurationOfWorkShiftDialog/durationofworkshiftdialog.cpp \
    FindAssignedShifts/findassignedshifts.cpp \
    FindRequirements/findrequirements.cpp \
    FindShifts/findshifts.cpp \
    FindVacation/findvacation.cpp \
    FindWishes/findwishes.cpp \
    PositionsManager/positionswidget.cpp \
    ProjectSetupWidget/projectsetupwidget.cpp \
    RequirementsManager/requirement.cpp \
    ShiftCharts/charttimerangedialog.cpp \
    ShiftCharts/shiftcharts.cpp \
    SingleShiftWidget/singleshiftwidget.cpp \
    StartApplicationWidget/startapplicationwidget.cpp \
    VacationsManager/vacationwidget.cpp \
    WishesManager/wishesworkerswidget.cpp \
    WorkerInformation/workersinformation.cpp \
    WorkerSearchResult/workersearchresult.cpp \
    main.cpp \
    mainwindow.cpp \
    projectdataloader.cpp \
    projectdatasaver.cpp \
 #   shiftcharts.cpp \
    workersprioritylists.cpp

HEADERS += \
    AutomatedAlgorithmsBuildingSchedules/automatedalgorithmsbuildingschedules.h \
    CreatingProjectBasedOnExistingDialog/creatingprojectbasedonexistingdialog.h \
    DurationOfWorkShiftDialog/durationofworkshiftdialog.h \
    FindAssignedShifts/findassignedshifts.h \
    FindRequirements/findrequirements.h \
    FindShifts/findshifts.h \
    FindVacation/findvacation.h \
    FindWishes/findwishes.h \
    PositionsManager/positionswidget.h \
    ProjectSetupWidget/projectsetupwidget.h \
    RequirementsManager/requirement.h \
    ShiftCharts/charttimerangedialog.h \
    ShiftCharts/shiftcharts.h \
    SingleShiftWidget/singleshiftwidget.h \
    StartApplicationWidget/startapplicationwidget.h \
    VacationsManager/vacationwidget.h \
    WishesManager/wishesworkerswidget.h \
    WorkerInformation/workersinformation.h \
    WorkerSearchResult/workersearchresult.h \
    mainwindow.h \
    projectdataloader.h \
    projectdatasaver.h \
#    shiftcharts.h \
    workersprioritylists.h

FORMS += \
    CreatingProjectBasedOnExistingDialog/creatingprojectbasedonexistingdialog.ui \
    DurationOfWorkShiftDialog/durationofworkshiftdialog.ui \
    FindAssignedShifts/findassignedshifts.ui \
    FindRequirements/findrequirements.ui \
    FindShifts/findshifts.ui \
    FindVacation/findvacation.ui \
    FindWishes/findwishes.ui \
    PositionsManager/positionswidget.ui \
    ProjectSetupWidget/projectsetupwidget.ui \
    RequirementsManager/requirement.ui \
    ShiftCharts/charttimerangedialog.ui \
    SingleShiftWidget/singleshiftwidget.ui \
    StartApplicationWidget/startapplicationwidget.ui \
    VacationsManager/vacationwidget.ui \
    WishesManager/wishesworkerswidget.ui \
    WorkerInformation/workersinformation.ui \
    WorkerSearchResult/workersearchresult.ui \
    mainwindow.ui

include(../../common.pri)
include(../../app.pri)

LIBS += -lXML_Parsing

# Default rules for deployment.
# qnx: target.path = /tmp/$${TARGET}/bin
# else: unix:!android: target.path = /opt/$${TARGET}/bin
# !isEmpty(target.path): INSTALLS += target

DISTFILES += \
    StartApplicationWidget/StartWidgetStyleSheet.css

RESOURCES += \
    Icon.qrc
