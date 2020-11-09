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

#include "pluginspec.h"

#include <utils/algorithm.h>

#include <QElapsedTimer>
#include <QObject>
#include <QReadWriteLock>
#include <QScopedPointer>
#include <QSet>
#include <QStringList>

#include <queue>

QT_BEGIN_NAMESPACE
class QTime;
class QTimer;
class QSettings;
class QEventLoop;
QT_END_NAMESPACE

namespace ExtensionSystem {

class PluginManager;

namespace Internal {

class PluginSpecPrivate;

class EXTENSIONSYSTEM_EXPORT PluginManagerPrivate : public QObject
{
    Q_OBJECT
public:
    PluginManagerPrivate(PluginManager *pluginManager);
    ~PluginManagerPrivate() override;

    // Object pool operations
    /*添加对象进对象池*/
    void addObject(QObject *obj);
    /*从对象池中移除*/
    void removeObject(QObject *obj);

    // Plugin operations
    /*检查问题插件*/
    void checkForProblematicPlugins();
    /*加载所有插件*/
    void loadPlugins();
    /*关闭并删除所有插件*/
    void shutdown();
    /*设置插件搜索路径*/
    void setPluginPaths(const QStringList &paths);
    /*利用深度优先搜索完成拓扑排序，是该插件管理器的核心算法*/
    const QVector<ExtensionSystem::PluginSpec *> loadQueue();
    /*加载单个插件*/
    void loadPlugin(PluginSpec *spec, PluginSpec::State destState);
    /*遍历所有插件，解析其依赖项*/
    void resolveDependencies();
    /*启用所有开机启动插件的依赖项*/
    void enableDependenciesIndirectly();
    ////////////////性能分析用函数//////////////////////////////////
    void initProfiling();
    void profilingSummary() const;
    void profilingReport(const char *what, const PluginSpec *spec = nullptr);
    //////////////////////////////////////////////////////////////
    /*定义要用于有关已启用和已禁用插件的信息的用户特定设置*/
    void setSettings(QSettings *settings);
    /*定义全局（独立于用户）设置，用于有关默认禁用插件的信息*/
    void setGlobalSettings(QSettings *settings);
    /*从全局设置和用户设置中读取默认禁用插件和默认启用插件*/
    void readSettings();
    /*保存用户设置，即不同于插件默认设置的用户设置*/
    void writeSettings();

    class TestSpec {
    public:
        TestSpec(PluginSpec *pluginSpec, const QStringList &testFunctionsOrObjects = QStringList())
            : pluginSpec(pluginSpec)
            , testFunctionsOrObjects(testFunctionsOrObjects)
        {}
        PluginSpec *pluginSpec = nullptr;
        QStringList testFunctionsOrObjects;
    };

    bool containsTestSpec(PluginSpec *pluginSpec) const
    {
        return Utils::contains(testSpecs, [pluginSpec](const TestSpec &s) { return s.pluginSpec == pluginSpec; });
    }

    void removeTestSpec(PluginSpec *pluginSpec)
    {
        testSpecs = Utils::filtered(testSpecs, [pluginSpec](const TestSpec &s) { return s.pluginSpec != pluginSpec; });
    }

    /////////////////////属性/////////////////////////////////
    /*按插件类别分类*/
    QHash<QString, QVector<PluginSpec *>> pluginCategories;
    /*插件集合*/
    QVector<PluginSpec *> pluginSpecs;
    std::vector<TestSpec> testSpecs;
    /*插件搜寻路径列表*/
    QStringList pluginPaths;
    /*识别的插件IID*/
    QString pluginIID;
    /*对象池列表*/
    QVector<QObject *> allObjects;      // ### make this a QVector<QPointer<QObject> > > ?
    /*默认禁用插件*/
    QStringList defaultDisabledPlugins; // Plugins/Ignored from install settings
    /*默认启用插件*/
    QStringList defaultEnabledPlugins; // Plugins/ForceEnabled from install settings
    /*已禁用插件*/
    QStringList disabledPlugins;
    /*命令行强制启用的插件*/
    QStringList forceEnabledPlugins;
    // delayed initialization
    /*延迟初始化计时器*/
    QTimer *delayedInitializeTimer = nullptr;
    /*延迟初始化插件队列*/
    std::queue<PluginSpec *> delayedInitializeQueue;
    // ansynchronous shutdown
    /*异步关闭插件集合*/
    QSet<PluginSpec *> asynchronousPlugins;  // plugins that have requested async shutdown
    /*事件循环以等待异步关闭插件全部关闭*/
    QEventLoop *shutdownEventLoop = nullptr; // used for async shutdown

    /*命令行参数*/
    QStringList arguments;
    /*重启用参数*/
    QStringList argumentsForRestart;
    // 以下四个变量用于分析性能
    ////////////////////////////////////////////////////
    QScopedPointer<QElapsedTimer> m_profileTimer;
    QHash<const PluginSpec *, int> m_profileTotal;
    int m_profileElapsedMS = 0;
    unsigned m_profilingVerbosity = 0;
    /////////////////////////////////////////////////////
    /*用户设置，可以多次设置*/
    QSettings *settings = nullptr;
    /*全局设置*/
    QSettings *globalSettings = nullptr;
    /////////////////////////////////////////////////////////

    // Look in argument descriptions of the specs for the option.
    // 查找具有给定名称选项的插件，以及指示是否要求参数
    PluginSpec *pluginForOption(const QString &option, bool *requiresArgument) const;
    // 查找指定名称的插件
    PluginSpec *pluginByName(const QString &name) const;

    // used by tests
    static PluginSpec *createSpec();
    static PluginSpecPrivate *privateSpec(PluginSpec *spec);

    /////////////////////属性/////////////////////////////////
    mutable QReadWriteLock m_lock;

    /*所有插件是否初始化完毕*/
    bool m_isInitializationDone = false;
    /////////////////////////////////////////////////////////

private:
    PluginManager *q;

    /*槽函数，用于延迟初始化插件*/
    void nextDelayedInitialize();
    /*槽函数，用于相应异步关闭插件的信号*/
    void asyncShutdownFinished();
    /*读取并保存所有插件的元数据*/
    void readPluginPaths();
    /*递归构建加载队列*/
    bool loadQueue(PluginSpec *spec,
                   QVector<ExtensionSystem::PluginSpec *> &queue,
                   QVector<ExtensionSystem::PluginSpec *> &circularityCheckQueue);
    /*停止所有插件*/
    void stopAll();
    /*与加载队列反向销毁所有插件*/
    void deleteAll();

#ifdef WITH_TESTS
    void startTests();
#endif
};

} // namespace Internal
} // namespace ExtensionSystem
