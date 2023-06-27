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
    customScrollContainer.cpp \
    customWidgets.cpp \
    datadisplaywidget.cpp \
    graph_view.cpp \
    main.cpp \
    mainwindow.cpp \
    motioncontrolwidget.cpp \
    slidedialog.cpp \
    slidepage.cpp

HEADERS += \
    customScrollContainer.h \
    customWidgets.h \
    datadisplaywidget.h \
    graph_view.h \
    mainwindow.h \
    motioncontrolwidget.h \
    slidedialog.h \
    slidepage.h

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
