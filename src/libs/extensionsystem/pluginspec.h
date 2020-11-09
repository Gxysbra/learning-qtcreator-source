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

#include <QString>
#include <QHash>
#include <QVector>

QT_BEGIN_NAMESPACE
class QStringList;
class QRegularExpression;
QT_END_NAMESPACE

namespace ExtensionSystem {

namespace Internal {

class OptionsParser;
class PluginSpecPrivate;
class PluginManagerPrivate;

} // Internal

class IPlugin;
class PluginView;

struct EXTENSIONSYSTEM_EXPORT PluginDependency
{
    enum Type {
        Required,
        Optional,
        Test
    };

    PluginDependency() : type(Required) {}

    QString name;
    QString version;
    Type type;
    bool operator==(const PluginDependency &other) const;
    QString toString() const;
};

uint qHash(const ExtensionSystem::PluginDependency &value);

struct EXTENSIONSYSTEM_EXPORT PluginArgumentDescription
{
    QString name;
    QString parameter;
    QString description;
};

class EXTENSIONSYSTEM_EXPORT PluginSpec
{
public:
    /*State指示插件加载时所经历的状态*/
    enum State { Invalid, Read, Resolved, Loaded, Initialized, Running, Stopped, Deleted};

    ~PluginSpec();

    // information from the xml file, valid after 'Read' state is reached
    /*返回插件名*/
    QString name() const;
    /*返回插件版本*/
    QString version() const;
    /*返回插件兼容版本*/
    QString compatVersion() const;
    /*返回插件供应商*/
    QString vendor() const;
    /*返回插件版权*/
    QString copyright() const;
    /*返回插件许可证*/
    QString license() const;
    /*返回插件描述*/
    QString description() const;
    /*返回插件URL*/
    QString url() const;
    /*返回插件类别*/
    QString category() const;
    QString revision() const;
    /*返回插件的工作平台*/
    QRegularExpression platformSpecification() const;
    /*返回插件是否在主机平台上工作*/
    bool isAvailableForHostPlatform() const;
    /*返回是否需要插件*/
    bool isRequired() const;
    /*返回插件是否设置了实验标志*/
    bool isExperimental() const;
    /*返回插件在默认情况下是否已启用*/
    bool isEnabledByDefault() const;
    /*返回是否应在启动时加载插件，同时考虑默认启用状态和用户设置*/
    bool isEnabledBySettings() const;
    /*返回是否在启动时加载插件*/
    bool isEffectivelyEnabled() const;
    /*如果由于用户取消选择此插件或其依赖项而导致加载未完成，则返回true*/
    bool isEnabledIndirectly() const;
    /*返回是否通过命令行上的-load选项启用插件*/
    bool isForceEnabled() const;
    /*返回是否通过命令行上的-noload选项禁用插件*/
    bool isForceDisabled() const;
    /*返回插件的依赖项*/
    QVector<PluginDependency> dependencies() const;
    /*返回插件元数据*/
    QJsonObject metaData() const;

    /*返回插件处理的命令行参数的说明列表*/
    using PluginArgumentDescriptions = QVector<PluginArgumentDescription>;
    PluginArgumentDescriptions argumentDescriptions() const;

    // other information, valid after 'Read' state is reached
    /*返回包含插件的目录的绝对路径*/
    QString location() const;
    /*返回插件的绝对路径*/
    QString filePath() const;

    /*返回特定于插件的命令行参数。启动时设置*/
    QStringList arguments() const;
    /*将特定于插件的命令行参数设置为arguments*/
    void setArguments(const QStringList &arguments);
    /*将argument添加到特定于插件的命令行参数*/
    void addArgument(const QString &argument);

    /*返回此插件是否可用于填充给定插件名称和版本的依赖项*/
    bool provides(const QString &pluginName, const QString &version) const;

    // dependency specs, valid after 'Resolved' state is reached
    /*返回已解析为现有插件规范的依赖项列表*/
    QHash<PluginDependency, PluginSpec *> dependencySpecs() const;
    /* 返回插件是否需要plugins指定的任何插件
     * 如果plugins中存在该插件所必须的依赖项，则返回true
     */
    bool requiresAny(const QSet<PluginSpec *> &plugins) const;

    // linked plugin instance, valid after 'Loaded' state is reached
    /*如果已成功加载插件库，则返回相应的IPlugin实例*/
    IPlugin *plugin() const;

    // state
    /*返回插件当前状态*/
    State state() const;
    /*返回读取或启动插件时是否发生错误*/
    bool hasError() const;
    /*在发生错误时返回详细的（可能是多行）错误描述*/
    QString errorString() const;

    /*设置插件是否在启动时加载*/
    void setEnabledBySettings(bool value);

private:
    /*私有构造函数，禁止外部构造，只允许友元类访问该构造函数以创建该类型对象*/
    PluginSpec();

    Internal::PluginSpecPrivate *d;
    /*用于读取插件信息已进行显示*/
    friend class PluginView;
    friend class Internal::OptionsParser;
    /*用于调用PluginSpec的私有构造函数创建该类实例*/
    friend class Internal::PluginManagerPrivate;
    /*用于D指针和Q指针*/
    friend class Internal::PluginSpecPrivate;
};

} // namespace ExtensionSystem
