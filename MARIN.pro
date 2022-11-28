TEMPLATE = app
TARGET = MARIN

CONFIG += c++11
QT += qml quick quickcontrols2 multimedia core multimediawidgets quickwidgets

SOURCES += src/main.cpp \
    src/gestureHandler.cpp \
    src/glwidget.cpp \
    src/receiver.cpp \
    src/sender.cpp \
    src/qmlmainwindow.cpp \
    src/threadshandler.cpp \
    src/commandsreceivethread.cpp \
    src/openigtlsendthread.cpp \
    src/videoreceivethread.cpp \
    src/camera.cpp \
    src/videoFrameGrabber.cpp

HEADERS += configs/constants.h \
    src/gestureHandler.h \
    src/glwidget.h \
    src/receiver.h \
    src/sender.h \
    src/qmlmainwindow.h \
    src/threadshandler.h \
    src/commandsreceivethread.h \
    src/openigtlsendthread.h \
    src/videoreceivethread.h \
    src/camera.h \
    src/videoFrameGrabber.h

RESOURCES += res.qrc \
    images.qrc \
    qml.qrc

deployment.files = ../MARIN/configs/OH264Config_MARIN.cfg
deployment.path =
QMAKE_BUNDLE_DATA += deployment

# Additional import path used to resolve QML modules in Qt Creator's code model
QMAKE_INFO_PLIST = Info.plist

INCLUDEPATH += \
    /PathToOpenIGTLink/Source \
    /PathToOpenIGTLink/Source/VideoStreaming \
    /PathToOpenIGTLink/Source/igtlutil \
    /PathToOpenIGTLinkBuild \
    /PathToOpenIGTLinkBuild/lib/Debug \
    /PathToOpenIGTLinkBuild/Deps/openh264 \
    /PathToOpenIGTLinkBuild/Deps/openh264/codec \
    /PathToOpenIGTLinkBuild/Deps/openh264/codec/api/svc \
    /PathToLibyuv/src/include \
    /PathToLibyuv/src/include/libyuv

#For now, MARIN looks for libraries in the lib folder. It is the user's responsability to compile necessary libraries and place them in proper locations.
#Should be present: libyuv, openigtlink and openh264 (sub-components: welsdec welsenc, processing, common)
#TODO: should make a super build for external libraries

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/libyuv_internal/release/ -lyuv_internal
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/libyuv_internal/debug/ -lyuv_internal
else:unix: LIBS += -L$$PWD/lib/libyuv_internal/ -lyuv_internal

INCLUDEPATH += $$PWD/lib/libyuv_internal
DEPENDPATH += $$PWD/lib/libyuv_internal

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libyuv_internal/release/libyuv_internal.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libyuv_internal/debug/libyuv_internal.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libyuv_internal/release/yuv_internal.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libyuv_internal/debug/yuv_internal.lib
else:unix: PRE_TARGETDEPS += $$PWD/lib/libyuv_internal/libyuv_internal.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/libyuv_neon/release/ -lyuv_neon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/libyuv_neon/debug/ -lyuv_neon
else:unix: LIBS += -L$$PWD/lib/libyuv_neon/ -lyuv_neon

INCLUDEPATH += $$PWD/lib/libyuv_neon
DEPENDPATH += $$PWD/lib/libyuv_neon

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libyuv_neon/release/libyuv_neon.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libyuv_neon/debug/libyuv_neon.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libyuv_neon/release/yuv_neon.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libyuv_neon/debug/yuv_neon.lib
else:unix: PRE_TARGETDEPS += $$PWD/lib/libyuv_neon/libyuv_neon.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/common/release/ -lcommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/common/debug/ -lcommon
else:unix: LIBS += -L$$PWD/lib/common/ -lcommon

INCLUDEPATH += $$PWD/lib/common
DEPENDPATH += $$PWD/lib/common

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/common/release/libcommon.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/common/debug/libcommon.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/common/release/common.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/common/debug/common.lib
else:unix: PRE_TARGETDEPS += $$PWD/lib/common/libcommon.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/processing/release/ -lprocessing
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/processing/debug/ -lprocessing
else:unix: LIBS += -L$$PWD/lib/processing/ -lprocessing

INCLUDEPATH += $$PWD/lib/processing
DEPENDPATH += $$PWD/lib/processing

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/processing/release/libprocessing.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/processing/debug/libprocessing.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/processing/release/processing.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/processing/debug/processing.lib
else:unix: PRE_TARGETDEPS += $$PWD/lib/processing/libprocessing.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/welsdec/release/ -lwelsdec
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/welsdec/debug/ -lwelsdec
else:unix: LIBS += -L$$PWD/lib/welsdec/ -lwelsdec

INCLUDEPATH += $$PWD/lib/welsdec
DEPENDPATH += $$PWD/lib/welsdec

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/welsdec/release/libwelsdec.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/welsdec/debug/libwelsdec.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/welsdec/release/welsdec.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/welsdec/debug/welsdec.lib
else:unix: PRE_TARGETDEPS += $$PWD/lib/welsdec/libwelsdec.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/welsenc/release/ -lwelsenc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/welsenc/debug/ -lwelsenc
else:unix: LIBS += -L$$PWD/lib/welsenc/ -lwelsenc

INCLUDEPATH += $$PWD/lib/welsenc
DEPENDPATH += $$PWD/lib/welsenc

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/welsenc/release/libwelsenc.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/welsenc/debug/libwelsenc.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/welsenc/release/welsenc.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/welsenc/debug/welsenc.lib
else:unix: PRE_TARGETDEPS += $$PWD/lib/welsenc/libwelsenc.a
```

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/openIGTLink/release/ -lOpenIGTLink
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/openIGTLink/debug/ -lOpenIGTLink
else:unix: LIBS += -L$$PWD/lib/openIGTLink/ -lOpenIGTLink

INCLUDEPATH += $$PWD/lib/openIGTLink
DEPENDPATH += $$PWD/lib/openIGTLink

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/openIGTLink/release/libOpenIGTLink.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/openIGTLink/debug/libOpenIGTLink.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/openIGTLink/release/OpenIGTLink.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/openIGTLink/debug/OpenIGTLink.lib
else:unix: PRE_TARGETDEPS += $$PWD/lib/openIGTLink/libOpenIGTLink.a
