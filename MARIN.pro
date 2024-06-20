TEMPLATE = app
TARGET = MARIN

CONFIG += c++11
QT += qml quick quickcontrols2 multimedia core multimediawidgets quickwidgets

SOURCES += src/main.cpp \
    src/camera.cpp \
    src/commandsreceivethread.cpp \
    src/gesturehandler.cpp \
    src/glwidget.cpp \
    src/openigtlsendthread.cpp \
    src/qmlmainwindow.cpp \
    src/receiver.cpp \
    src/sender.cpp \
    src/sendercommand.cpp \
    src/sendervideo.cpp \
    src/threadshandler.cpp \
    src/videoFrameGrabber.cpp \
    src/videoreceivethread.cpp

HEADERS += configs/constants.h \
    src/camera.h \
    src/commandsreceivethread.h \
    src/gesturehandler.h \
    src/glwidget.h \
    src/openigtlsendthread.h \
    src/qmlmainwindow.h \
    src/receiver.h \
    src/sender.h \
    src/sendercommand.h \
    src/sendervideo.h \
    src/threadshandler.h \
    src/videoFrameGrabber.h \
    src/videoreceivethread.h

RESOURCES += res.qrc \
    images.qrc \
    qml.qrc

deployment.files = ../MARIN/configs/OH264Config_MARIN.cfg
deployment.path =
QMAKE_BUNDLE_DATA += deployment

# Additional import path used to resolve QML modules in Qt Creator's code model
VERSION = 1.0.0
QMAKE_INFO_PLIST = Info.plist

INCLUDEPATH += \
    /PathToOpenIGTLink/Source \
    /PathToOpenIGTLink/Source/VideoStreaming \
    /PathToOpenIGTLink/Source/igtlutil \
    /PathToOpenIGTLinkBuild \
    /PathToOpenIGTLinkBuild/lib/Debug \
    /PathToOpenIGTLinkBuild/Deps/openh264 \
    /PathToOpenIGTLinkBuild/Deps/openh264/codec \
    /PathToOpenIGTLinkBuild/Deps/openh264/codec/api/wels \
    includes/libyuv \
    includes/libyuv/libyuv

#For now, MARIN looks for libraries in the lib folder. It is the user's responsability to compile necessary libraries and place them in proper locations.
#Should be present: libyuv (an arm64 build is provided), openigtlink and openh264
#TODO: should make a super build for external libraries

LIBS += -L$$PWD/lib/libyuv_internal/ -lyuv_internal
INCLUDEPATH += $$PWD/lib/libyuv_internal
DEPENDPATH += $$PWD/lib/libyuv_internal
PRE_TARGETDEPS += $$PWD/lib/libyuv_internal/libyuv_internal.a

LIBS += -L$$PWD/lib/libyuv_neon/ -lyuv_neon
INCLUDEPATH += $$PWD/lib/libyuv_neon
DEPENDPATH += $$PWD/lib/libyuv_neon
PRE_TARGETDEPS += $$PWD/lib/libyuv_neon/libyuv_neon.a

LIBS += -L$$PWD/lib/openIGTLink/ -lOpenIGTLink
INCLUDEPATH += $$PWD/lib/openIGTLink
DEPENDPATH += $$PWD/lib/openIGTLink
PRE_TARGETDEPS += $$PWD/lib/openIGTLink/libOpenIGTLink.a

LIBS += -L$$PWD/lib/openh264/ -lopenh264
INCLUDEPATH += $$PWD/lib/openh264
DEPENDPATH += $$PWD/lib/openh264
PRE_TARGETDEPS += $$PWD/lib/openh264/libopenh264.a
