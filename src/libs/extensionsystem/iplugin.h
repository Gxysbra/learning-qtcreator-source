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

#include <QObject>

namespace ExtensionSystem {

namespace Internal {
    class IPluginPrivate;
    class PluginSpecPrivate;
}

class PluginManager;
class PluginSpec;

class EXTENSIONSYSTEM_EXPORT IPlugin : public QObject
{
    Q_OBJECT

public:
    /*此枚举类型保存插件是同步关闭还是异步关闭*/
    enum ShutdownFlag {
        SynchronousShutdown,
        AsynchronousShutdown
    };

    IPlugin();
    ~IPlugin() override;

    /*
     * 插件的initialize（）函数适合于：
     *  1.在插件管理器的对象池中注册对象
     *  2.加载设置
     *  3.向菜单添加新菜单和新操作
     *  4.连接到其他插件的信号
     *
     * 该函数用于注册共享对象
     */
    virtual bool initialize(const QStringList &arguments, QString *errorString) = 0;
    /* 这是在全局对象池中查找由弱依赖插件提供的对象的好地方
     *
     * 该函数用于从对象池中寻找依赖
     */
    virtual void extensionsInitialized() {}
    /* 如果一个插件需要做一些不重要的设置，
     * 而这些设置不一定需要在启动时直接完成，
     * 但是仍然应该在很短的时间内完成，那么可以使用这个函数。
     */
    virtual bool delayedInitialize() { return false; }
    /*如果插件需要在关闭之前执行异步操作，则返回IPlugin::AsynchronousShutdown*/
    virtual ShutdownFlag aboutToShutdown() { return SynchronousShutdown; }
    /*当在另一个实例正在运行时使用-client参数执行时，将在正在运行的实例中调用该插件的此函数*/
    virtual QObject *remoteCommand(const QStringList & /* options */,
                                   const QString & /* workingDirectory */,
                                   const QStringList & /* arguments */) { return nullptr; }
    virtual QVector<QObject *> createTestObjects() const;

    /*返回与此插件对应的PluginSpec*/
    PluginSpec *pluginSpec() const;

signals:
    /*当后台关闭进程结束时发送，通知插件管理器*/
    void asynchronousShutdownFinished();

private:
    Internal::IPluginPrivate *d;

    friend class Internal::PluginSpecPrivate;
};

} // namespace ExtensionSystem
