QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
LIBS += -L/usr/local/lib/libwiringPi.so -lwiringPi

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    receiverCanvas.cpp \
    receivethread.cpp \
    receivewindow.cpp \
    scribblearea.cpp \
    sendthread.cpp

HEADERS += \
    commandcode.h \
    mainwindow.h \
    queue.h \
    receiverCanvas.h \
    receivethread.h \
    receivewindow.h \
    scribblearea.h \
    sendthread.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
