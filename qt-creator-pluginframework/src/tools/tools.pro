include(../../qtcreator.pri)

TEMPLATE = subdirs

SUBDIRS = 3rdparty

!isEmpty(BREAKPAD_SOURCE_DIR) {
    SUBDIRS += qtcrashhandler
} else {
    linux-* {
        # Build only in debug mode.
        debug_and_release|CONFIG(debug, debug|release) {
            SUBDIRS += qtcreatorcrashhandler
        }
    }
}

OTHER_FILES += \
    tools.qbs \
    icons/exportapplicationicons.sh \
    icons/exportdocumenttypeicons.sh

# delegate deployqt target
deployqt.CONFIG += recursive
deployqt.recurse = perfparser
QMAKE_EXTRA_TARGETS += deployqt
