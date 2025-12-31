QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport # Choosing modules (group of classes) needed for the app

CONFIG += c++11 # Setting the standard of C++

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutdialog.cpp \
    changedatasetinfodialog.cpp \
    dataset.cpp \
    datasetwindow.cpp \
    editaxisdialog.cpp \
    errordialog.cpp \
    fftdialog.cpp \
    functiondialog.cpp \
    graphstyledialog.cpp \
    graphwindow.cpp \
    helpdialog.cpp \
    interpolationdialog.cpp \
    main.cpp \
    parentwindow.cpp \
    plotdatadialog.cpp \
    plothistogramdatadialog.cpp \
    qcustomplot.cpp

HEADERS += \
    aboutdialog.h \
    atmsp.h \
    changedatasetinfodialog.h \
    dataset.h \
    datasetwindow.h \
    editaxisdialog.h \
    errordialog.h \
    fftdialog.h \
    functiondialog.h \
    graphstyledialog.h \
    graphwindow.h \
    helpdialog.h \
    interpolationdialog.h \
    parentwindow.h \
    plotdatadialog.h \
    plothistogramdatadialog.h \
    qcustomplot.h

FORMS += \
    aboutdialog.ui \
    changedatasetinfodialog.ui \
    datasetwindow.ui \
    editaxisdialog.ui \
    errordialog.ui \
    fftdialog.ui \
    functiondialog.ui \
    graphstyledialog.ui \
    graphwindow.ui \
    helpdialog.ui \
    interpolationDialog.ui \
    parentwindow.ui \
    plotdatadialog.ui \
    plothistogramdatadialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/GSLlib/ -lgsl

INCLUDEPATH += $$PWD/GSLinclude
DEPENDPATH += $$PWD/GSLlib

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/GSLlib/gsl.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/GSLlib/libgsl.a



win32: LIBS += -L$$PWD/GSLlib/ -lgslcblas

INCLUDEPATH += $$PWD/GSLinclude
DEPENDPATH += $$PWD/GSLlib

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/GSLlib/gslcblas.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/GSLlib/libgslcblas.a

RESOURCES += \
    resources.qrc
