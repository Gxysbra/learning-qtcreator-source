TEMPLATE  = subdirs
CONFIG   += ordered

SUBDIRS += \
    libs \
    app \
    plugins

# delegate deployqt target
deployqt.CONFIG += recursive
deployqt.recurse = libs app plugins
QMAKE_EXTRA_TARGETS += deployqt
