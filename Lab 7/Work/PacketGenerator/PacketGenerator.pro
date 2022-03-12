QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/IcmpView.cpp \
    src/UdpView.cpp \
    src/TcpView.cpp \
    src/IpView.cpp \
    src/main.cpp \
    src/MainWindow.cpp \
    third_party/MemoryViewer.cpp \
    src/SavingWindow.cpp \
    src/PoolWindow.cpp \
    src/ChangingDeviceWindow.cpp

HEADERS += \
    src/CheckSum.h \
    src/GeneratedPackets.h \
    src/MainWindow.h \
    third_party/MemoryViewer.h \
    src/SavingWindow.h \
    src/PoolWindow.h \
    src/ChangingDeviceWindow.h

FORMS += \
    ui/ChangingDeviceWindow.ui \
    ui/PoolWindow.ui \
    ui/SavingWindow.ui \
    ui/MainWindow.ui

LIBS += -lpcap
LIBS += -lpthread
LIBS += ../pcap/libPcap++.a
LIBS += ../pcap/libPacket++.a
LIBS += ../pcap/libCommon++.a

INCLUDEPATH += third_party/
INCLUDEPATH += ../pcap/include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
