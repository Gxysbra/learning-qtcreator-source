include(qtcreator.pri)

#version check qt
#检查qt版本，不能低于5.12.0
!minQtVersion(5, 12, 0) {
    message("Cannot build $$IDE_DISPLAY_NAME with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.12.0.")
}

#暂时忽略该项
#include(doc/doc.pri)

TEMPLATE  = subdirs
#按子目录顺序编译
CONFIG   += ordered

SUBDIRS = src #share
unix:!macx:!isEmpty(copydata):SUBDIRS += bin
!isEmpty(BUILD_TESTS):SUBDIRS += tests

#在最终的目标包括的文件
DISTFILES += README.md \

#平台相关信息
contains(QT_ARCH, i386): ARCHITECTURE = x86
else: ARCHITECTURE = $$QT_ARCH

macx: PLATFORM = "mac"
else:win32: PLATFORM = "windows"
else:linux-*: PLATFORM = "linux-$${ARCHITECTURE}"
else: PLATFORM = "unknown"

BASENAME = $$(INSTALL_BASENAME)
isEmpty(BASENAME): BASENAME = qt-creator-$${PLATFORM}$(INSTALL_EDITION)-$${QTCREATOR_VERSION}$(INSTALL_POSTFIX)

linux {
    appstream.files = share/metainfo/org.qt-project.qtcreator.appdata.xml
    appstream.path = $$QTC_PREFIX/share/metainfo/

    desktop.files = share/applications/org.qt-project.qtcreator.desktop
    desktop.path = $$QTC_PREFIX/share/applications/

    INSTALLS += appstream desktop
}

macx {
    APPBUNDLE = "$$OUT_PWD/bin/$${IDE_APP_TARGET}.app"
    BINDIST_SOURCE.release = "$$OUT_PWD/bin/$${IDE_APP_TARGET}.app"
    BINDIST_SOURCE.debug = "$$OUT_PWD/bin"
    BINDIST_EXCLUDE_ARG.debug = "--exclude-toplevel"
    deployqt.commands = $$PWD/scripts/deployqtHelper_mac.sh \"$${APPBUNDLE}\" \"$$[QT_INSTALL_BINS]\" \"$$[QT_INSTALL_TRANSLATIONS]\" \"$$[QT_INSTALL_PLUGINS]\" \"$$[QT_INSTALL_IMPORTS]\" \"$$[QT_INSTALL_QML]\"
    codesign.commands = codesign --deep -o runtime -s \"$(SIGNING_IDENTITY)\" $(SIGNING_FLAGS) \"$${APPBUNDLE}\"
    dmg.commands = python -u \"$$PWD/scripts/makedmg.py\" \"$${BASENAME}.dmg\" \"Qt Creator\" \"$$IDE_SOURCE_TREE\" \"$$OUT_PWD/bin\"
    #dmg.depends = deployqt
    QMAKE_EXTRA_TARGETS += codesign dmg
} else {
    BINDIST_SOURCE.release = "$(INSTALL_ROOT)$$QTC_PREFIX"
    BINDIST_EXCLUDE_ARG.release = "--exclude-toplevel"
    BINDIST_SOURCE.debug = $${BINDIST_SOURCE.release}
    BINDIST_EXCLUDE_ARG.debug = $${BINDIST_EXCLUDE_ARG.release}
    deployqt.commands = python -u $$PWD/scripts/deployqt.py -i \"$(INSTALL_ROOT)$$QTC_PREFIX/bin/$${IDE_APP_TARGET}\" \"$(QMAKE)\"
    deployqt.depends = install
    # legacy dummy target
    win32: QMAKE_EXTRA_TARGETS += deployartifacts
}

INSTALLER_ARCHIVE_FROM_ENV = $$(INSTALLER_ARCHIVE)
isEmpty(INSTALLER_ARCHIVE_FROM_ENV) {
    INSTALLER_ARCHIVE = $$OUT_PWD/$${BASENAME}-installer-archive.7z
} else {
    INSTALLER_ARCHIVE = $$OUT_PWD/$$(INSTALLER_ARCHIVE)
}

INSTALLER_ARCHIVE_DEBUG = $$INSTALLER_ARCHIVE
INSTALLER_ARCHIVE_DEBUG ~= s/(.*)[.]7z/\1-debug.7z

bindist.commands = python -u $$PWD/scripts/createDistPackage.py $$OUT_PWD/$${BASENAME}.7z \"$${BINDIST_SOURCE.release}\"
bindist_installer.commands = python -u $$PWD/scripts/createDistPackage.py $${BINDIST_EXCLUDE_ARG.release} $${INSTALLER_ARCHIVE} \"$${BINDIST_SOURCE.release}\"
bindist_debug.commands = python -u $$PWD/scripts/createDistPackage.py --debug $${BINDIST_EXCLUDE_ARG.debug} $${INSTALLER_ARCHIVE_DEBUG} \"$${BINDIST_SOURCE.debug}\"

#适配命令路径中，Unix的/和Windows的\
win32 {
    deployqt.commands ~= s,/,\\\\,g
    bindist.commands ~= s,/,\\\\,g
    bindist_installer.commands ~= s,/,\\\\,g
}

deployqt.CONFIG += recursive
deployqt.recurse = src

QMAKE_EXTRA_TARGETS += deployqt bindist bindist_installer bindist_debug
