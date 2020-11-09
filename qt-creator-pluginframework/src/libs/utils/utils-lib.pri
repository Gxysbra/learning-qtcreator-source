shared {
    DEFINES += UTILS_LIBRARY
} else {
    DEFINES += QTCREATOR_UTILS_STATIC_LIB
}

!win32:{
    isEmpty(IDE_LIBEXEC_PATH) | isEmpty(IDE_BIN_PATH): {
        warning("using utils-lib.pri without IDE_LIBEXEC_PATH or IDE_BIN_PATH results in empty QTC_REL_TOOLS_PATH")
        DEFINES += QTC_REL_TOOLS_PATH=$$shell_quote(\"\")
    } else {
        DEFINES += QTC_REL_TOOLS_PATH=$$shell_quote(\"$$relative_path($$IDE_LIBEXEC_PATH, $$IDE_BIN_PATH)\")
    }
}

QT += widgets gui network qml xml

CONFIG += exceptions # used by portlist.cpp, textfileformat.cpp, and ssh/*

win32: LIBS += -luser32 -lshell32
# PortsGatherer
win32: LIBS += -liphlpapi -lws2_32

SOURCES += \
    $$PWD/benchmarker.cpp \
    $$PWD/environment.cpp \
    $$PWD/namevaluedictionary.cpp \
    $$PWD/namevalueitem.cpp \
    $$PWD/qtcprocess.cpp \
    $$PWD/stringutils.cpp \
    $$PWD/temporarydirectory.cpp \
    $$PWD/synchronousprocess.cpp \
    $$PWD/savefile.cpp \
    $$PWD/fileutils.cpp \
    $$PWD/stylehelper.cpp \
    $$PWD/qtcassert.cpp \
    $$PWD/hostosinfo.cpp \
    $$PWD/itemviews.cpp \
    $$PWD/treemodel.cpp \
    $$PWD/theme/theme.cpp \
    $$PWD/categorysortfiltermodel.cpp \
    $$PWD/icon.cpp \
    $$PWD/utilsicons.cpp

HEADERS += \
    $$PWD/environmentfwd.h \
    $$PWD/benchmarker.h \
    $$PWD/environment.h \
    $$PWD/namevaluedictionary.h \
    $$PWD/namevalueitem.h \
    $$PWD/qtcprocess.h \
    $$PWD/utils_global.h \
    $$PWD/stringutils.h \
    $$PWD/temporarydirectory.h \
    $$PWD/synchronousprocess.h \
    $$PWD/savefile.h \
    $$PWD/fileutils.h \
    $$PWD/qtcassert.h \
    $$PWD/stylehelper.h \
    $$PWD/hostosinfo.h \
    $$PWD/osspecificaspects.h \
    $$PWD/itemviews.h \
    $$PWD/treemodel.h \
    $$PWD/algorithm.h \
    $$PWD/theme/theme.h \
    $$PWD/theme/theme_p.h \
    $$PWD/executeondestruction.h \
    $$PWD/categorysortfiltermodel.h \
    $$PWD/utilsicons.h \
    $$PWD/icon.h \
    $$PWD/optional.h \
    $$PWD/../3rdparty/optional/optional.hpp \
    $$PWD/predicates.h \
    $$PWD/porting.h

RESOURCES += $$PWD/utils.qrc

osx {
    HEADERS += \
        $$PWD/theme/theme_mac.h \
        $$PWD/fileutils_mac.h

    OBJECTIVE_SOURCES += \
        $$PWD/theme/theme_mac.mm \
        $$PWD/fileutils_mac.mm \
        $$PWD/processhandle_mac.mm

    LIBS += -framework Foundation -framework AppKit
}

#include(touchbar/touchbar.pri)
include(mimetypes/mimetypes.pri)
