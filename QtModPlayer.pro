#-------------------------------------------------
#
# Project created by QtCreator 2018-05-13T13:20:02
#
#-------------------------------------------------

QT       += core gui concurrent sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(resources/QtAwesome/QtAwesome.pri)

ICON = Resources/experience_speakers_twin.icns

INCLUDEPATH += QtDarkStyle


TARGET = QtModPlayer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        QtDarkStyle/DarkStyle.cpp \
    ThreadedModFileCheck.cpp \
    PlaylistWidget/PlaylistControls.cpp \
    PlaylistWidget/BufferedTableModel.cpp \
    PlaylistWidget/PlaylistWidget.cpp \
    MainWindow.cpp \
    PlayerWidget.cpp \
    DBManager/DBManager.cpp \
    DBManager/ThreadedModFileInserter.cpp \
    SoundManager.cpp \
    WidgetStateStore.cpp \
    UiStateObject.cpp \
    ThreadedModFileCounter.cpp

HEADERS += \
    QtDarkStyle/DarkStyle.h \
    ThreadedModFileCheck.h \
    PlaylistWidget/PlaylistControls.h \
    PlaylistWidget/BufferedTableModel.h \
    PlaylistWidget/PlaylistWidget.h \
    MainWindow.h \
    PlayerWidget.h \
    DBManager/DBManager.h \
    DBManager/ThreadedModFileInserter.h \
    SoundManager.h \
    WidgetStateStore.h \
    UiStateObject.h \
    ThreadedModFileCounter.h

RESOURCES += \
        QtDarkStyle/darkstyle.qrc



INCLUDEPATH += \
    $$PWD/libraries/libopenmpt/include \
    $$PWD/libraries/portaudio/include \

DEPENDPATH += \
    $$PWD/libraries/libopenmpt/include \
    $$PWD/libraries/portaudio/include

# Link
macx: LIBS += \
    -L$$PWD/libraries/libopenmpt/lib/ -lopenmpt \
    -L$$PWD/libraries/portaudio/lib/ -lportaudio

macx: PRE_TARGETDEPS += \
    $$PWD/libraries/libopenmpt/lib/libopenmpt.a \
    $$PWD/libraries/portaudio/lib/libportaudio.a

## Include Database file
macx {
    db_files.path = Contents/Resources/db
    db_files.files = $$PWD/db/modmusic.db
    QMAKE_BUNDLE_DATA += db_files
}
