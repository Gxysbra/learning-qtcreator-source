/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#pragma once

#include "extensionsystem_global.h"
#include <aggregation/aggregate.h>

#include <QObject>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QTextStream;
class QSettings;
QT_END_NAMESPACE

namespace ExtensionSystem {
class IPlugin;
class PluginSpec;

namespace Internal { class PluginManagerPrivate; }

class EXTENSIONSYSTEM_EXPORT PluginManager : public QObject
{
    Q_OBJECT

public:
    /*单例模式*/
    static PluginManager *instance();

    PluginManager();
    ~PluginManager() override;

    // Object pool operations
    /*向对象池中注册对象*/
    static void addObject(QObject *obj);
    /*从对象池中销毁对象*/
    static void removeObject(QObject *obj);
    /*获取对象池中的所有对象*/
    static QVector<QObject *> allObjects();
    /*内部使用*/
    static QReadWriteLock *listLock();

    // This is useful for soft dependencies using pure interfaces.
    /*返回一个在对象池中注册的T类型的对象*/
    template <typename T> static T *getObject()
    {
        QReadLocker lock(listLock());
        const QVector<QObject *> all = allObjects();
        for (QObject *obj : all) {
            if (T *result = qobject_cast<T *>(obj))
                return result;
        }
        return nullptr;
    }
    /*返回一个在对象池中注册的并且满足一定条件的T类型的对象*/
    template <typename T, typename Predicate> static T *getObject(Predicate predicate)
    {
        QReadLocker lock(listLock());
        const QVector<QObject *> all = allObjects();
        for (QObject *obj : all) {
            if (T *result = qobject_cast<T *>(obj))
                if (predicate(result))
                    return result;
        }
        return 0;
    }
    /*返回一个在对象池中注册的具有给定对象名称的对象*/
    static QObject *getObjectByName(const QString &name);

    // Plugin operations
    /*按加载顺序返回插件列表，利用该加载队列简化了许多依赖项操作*/
    static QVector<PluginSpec *> loadQueue();
    /*加载所有插件搜索路径下的所有插件*/
    static void loadPlugins();
    /*返回所有插件搜索路径*/
    static QStringList pluginPaths();
    /*设置插件搜索路径*/
    static void setPluginPaths(const QStringList &paths);
    /*返回有效插件必须具有的IID*/
    static QString pluginIID();
    /*设置有效插件必须具有的IID*/
    static void setPluginIID(const QString &iid);
    /*在插件搜索路径中找到的所有插件的列表*/
    static const QVector<PluginSpec *> plugins();
    /*返回以插件类别为key的插件集合*/
    static QHash<QString, QVector<PluginSpec *>> pluginCollections();
    /*如果任何插件在启用时有错误，则返回true*/
    static bool hasError();
    /*返回插件启用时的错误列表*/
    static const QStringList allErrors();
    /*返回所有依赖于指定插件(直接或间接)的插件集合*/
    static const QSet<PluginSpec *> pluginsRequiringPlugin(PluginSpec *spec);
    /*返回指定插件的所有依赖插件(直接或间接)集合*/
    static const QSet<PluginSpec *> pluginsRequiredByPlugin(PluginSpec *spec);
    /*检查有问题的插件*/
    static void checkForProblematicPlugins();

    // Settings
    /*定义要用于有关已启用和已禁用插件的信息的用户特定设置*/
    static void setSettings(QSettings *settings);
    /*返回用于有关已启用和已禁用插件的信息的特定于用户的设置*/
    static QSettings *settings();
    /*定义全局（独立于用户）设置，用于有关默认禁用插件的信息*/
    static void setGlobalSettings(QSettings *settings);
    /*获取全局设置*/
    static QSettings *globalSettings();
    /*写入设置*/
    static void writeSettings();

    // command line arguments
    /*解析后剩下的参数（既不是启动参数也不是插件参数）。通常，这是要打开的文件列表*/
    static QStringList arguments();
    /*自动重新启动应用程序时应使用的参数*/
    static QStringList argumentsForRestart();
    /*获取args中的命令行选项列表并解析它们*/
    static bool parseOptions(const QStringList &args,
        const QMap<QString, bool> &appOptions,
        QMap<QString, QString> *foundAppOptions,
        QString *errorString);
    /*使用指定的optionIndentation和descriptionIndentation格式化命令行帮助的插件管理器的启动选项*/
    static void formatOptions(QTextStream &str, int optionIndentation, int descriptionIndentation);
    /*使用指定的optionIndentation和descriptionIndentation格式化命令行帮助的插件规范的插件选项*/
    static void formatPluginOptions(QTextStream &str, int optionIndentation, int descriptionIndentation);
    /*格式化命令行帮助的插件规范版本*/
    static void formatPluginVersions(QTextStream &str);
    /*序列化插件选项和参数*/
    static QString serializedArguments();

    static bool testRunRequested();

    // 分析报告
    static void profilingReport(const char *what, const PluginSpec *spec = nullptr);

    // 平台名字
    static QString platformName();

    static bool isInitializationDone();

    void remoteArguments(const QString &serializedArguments, QObject *socket);
    /*关闭并删除所有插件*/
    void shutdown();

    QString systemInformation() const;

signals:
    /*在对象池中注册对象后发送*/
    void objectAdded(QObject *obj);
    /*在从对象池中注销对象之前发送*/
    void aboutToRemoveObject(QObject *obj);

    void pluginsChanged();
    /*当插件运行并且所有延迟的初始化完成时发送*/
    void initializationDone();
    void testsFinished(int failedTests);

    friend class Internal::PluginManagerPrivate;
};

} // namespace ExtensionSystem
