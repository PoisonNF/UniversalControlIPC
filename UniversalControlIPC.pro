QT += core gui
QT += svg
QT += 3dcore 3drender 3dinput 3dextras
QT += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    datadisplaywidget.cpp \
    frame/customScrollContainer.cpp \
    frame/customWidgets.cpp \
    frame/graph_implement.cpp \
    frame/graph_view.cpp \
    frame/scenemodifier.cpp \
    frame/slidedialog.cpp \
    frame/slidepage.cpp \
    main.cpp \
    mainwindow.cpp \
    motioncontrolwidget.cpp

HEADERS += \
    datadisplaywidget.h \
    frame/customScrollContainer.h \
    frame/customWidgets.h \
    frame/graph_implement.h \
    frame/graph_view.h \
    frame/scenemodifier.h \
    frame/slidedialog.h \
    frame/slidepage.h \
    mainwindow.h \
    motioncontrolwidget.h

FORMS += \
    mainwindow.ui

RC_ICONS = logo.ico


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    fonts.qrc \
    icons.qrc

include(Qjoysticks/Qjoysticks.pri)
