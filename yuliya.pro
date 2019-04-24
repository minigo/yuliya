QT += widgets gui sql charts

HEADERS       = mainwindow.h \
                mdichild.h \
    connectiondialog.h \
    projectswidget.h \
    redmineinstance.h \
    redmineobjects.h \
    projectsettingsdialog.h \
    charts/mainslice.h \
    charts/donutbreakdownchart.h \
    charts/drilldownslice.h \
    charts/drilldownchart.h \
    metrics/maturitymetrics.h \
    metrics/faulttolerancemetrics.h \
    metrics/recoverabilitymetrics.h \
    metrics/reliabilitycompliancemetrics.h \
    metrics/resultinginternalmetrics.h \
    metrics/resultingexternalmetrics.h
SOURCES       = main.cpp \
                mainwindow.cpp \
                mdichild.cpp \
    connectiondialog.cpp \
    projectswidget.cpp \
    redmineinstance.cpp \
    redmineobjects.cpp \
    projectsettingsdialog.cpp \
    charts/mainslice.cpp \
    charts/donutbreakdownchart.cpp \
    charts/drilldownslice.cpp \
    charts/drilldownchart.cpp \
    metrics/maturitymetrics.cpp \
    metrics/faulttolerancemetrics.cpp \
    metrics/recoverabilitymetrics.cpp \
    metrics/reliabilitycompliancemetrics.cpp \
    metrics/resultinginternalmetrics.cpp \
    metrics/resultingexternalmetrics.cpp
RESOURCES     = yuliya.qrc

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/mainwindows/mdi
#sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS yuliya.pro images
#sources.path = $$[QT_INSTALL_EXAMPLES]/mainwindows/mdi
#INSTALLS += target sources

#symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
#maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

#symbian: warning(This example might not fully work on Symbian platform)
#simulator: warning(This example might not fully work on Simulator platform)

FORMS += \
    connectiondialog.ui \
    projectsettingsdialog.ui
