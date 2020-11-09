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
#include "iplugin.h"

#include <QJsonObject>
#include <QObject>
#include <QPluginLoader>
#include <QRegularExpression>
#include <QStringList>
#include <QVector>
#include <QXmlStreamReader>

namespace ExtensionSystem {

class IPlugin;
class PluginManager;

namespace Internal {

class EXTENSIONSYSTEM_EXPORT PluginSpecPrivate : public QObject
{
    Q_OBJECT

public:
    PluginSpecPrivate(PluginSpec *spec);

    /*
     * 实际的插件加载工作在这里完成，读取插件元数据
     * 如果不是本系统所识别的插件则返回false，否则进行元数据解析返回true
     * 当解析出错时设置错误信息
     */
    bool read(const QString &fileName);
    /*检查给定pluginName和version的插件是否兼容于该插件，即插件名匹配以及版本处于该插件版本与兼容版本之间*/
    bool provides(const QString &pluginName, const QString &version) const;
    /*
     * 解析并记录该插件所需的依赖项，参数为已有插件集合
     * 如果插件有错误，则不进行依赖项解析
     * 返回是否成功解析依赖项，即找到全部依赖项
     */
    bool resolveDependencies(const QVector<PluginSpec *> &specs);
    // 加载插件
    bool loadLibrary();
    // 初始化插件
    bool initializePlugin();
    // 初始化拓展
    bool initializeExtensions();
    // 延迟初始化
    bool delayedInitialize();
    // 停止插件
    IPlugin::ShutdownFlag stop();
    // 删除插件
    void kill();

    // 设置插件的启用禁用信息
    void setEnabledBySettings(bool value);
    void setEnabledByDefault(bool value);
    void setForceEnabled(bool value);
    void setForceDisabled(bool value);

    ////////////////属性///////////////////////////////////
    /*插件装载器*/
    QPluginLoader loader;

    /*插件名*/
    QString name;
    /*插件版本*/
    QString version;
    /*插件兼容版本*/
    QString compatVersion;
    /*作为About Plugins...对话框的提示，用户不能手动禁用此插件。只用于核心插件。*/
    bool required = false;
    /*如果设置，则插件不会显示在关于插件对话框的默认视图中，但前提是用户请求查看所有插件。它仍然与-version命令行选项一起显示。*/
    bool hiddenByDefault = false;
    /*实验性插件默认情况下不加载，但必须由用户显式启用。对于可能对用户体验产生负面影响的新插件，应该启用此属性。*/
    bool experimental = false;
    /*DisabledByDefault: 如果设置，则默认情况下不会加载相应的插件，但必须由用户显式启用。对于不希望被这么多人使用的插件，应该这样做，以证明额外的资源消耗是合理的。*/
    bool enabledByDefault = true;
    /*插件供应商*/
    QString vendor;
    /*插件版权*/
    QString copyright;
    /*插件许可证*/
    QString license;
    /*插件描述*/
    QString description;
    /*插件url*/
    QString url;
    /*插件类别*/
    QString category;
    /*插件适用平台的正则表达式，为空代表适用任何平台*/
    QRegularExpression platformSpecification;
    /*插件依赖项，包括必须的和可选的*/
    QVector<PluginDependency> dependencies;
    /*插件元数据*/
    QJsonObject metaData;
    /*是否由配置文件启动*/
    bool enabledBySettings = true;
    /*是否间接启动*/
    bool enabledIndirectly = false;
    /*是否由命令行强制启动*/
    bool forceEnabled = false;
    /*是否由命令行强制禁止*/
    bool forceDisabled = false;

    /*插件所在目标的绝对路径*/
    QString location;
    /*插件文件绝对路径*/
    QString filePath;
    /*由命令行传递给插件的命令行参数列表*/
    QStringList arguments;

    /*插件依赖项映射*/
    QHash<PluginDependency, PluginSpec *> dependencySpecs;
    /*插件各参数的描述*/
    PluginSpec::PluginArgumentDescriptions argumentDescriptions;
    /*该插件对应的IPlugin实例*/
    IPlugin *plugin = nullptr;

    /*插件当前状态*/
    PluginSpec::State state = PluginSpec::Invalid;
    /*插件生命周期中是否有错误产生*/
    bool hasError = false;
    /*错误原因*/
    QString errorString;
    ///////////////////////////////////////////////////////////

    /*判断给定的version参数是否是有效的插件版本格式*/
    static bool isValidVersion(const QString &version);
    /*比较插件版本大小*/
    static int versionCompare(const QString &version1, const QString &version2);

    /*
     * 返回它实际上间接启用的插件
     * 返回启动该插件所需要的另外启动的依赖项，即原本不需要其启动的依赖项
     */
    QVector<PluginSpec *> enableDependenciesIndirectly(bool enableTestDependencies = false);

    /*解析插件元数据，返回是否是该系统识别的插件元数据*/
    bool readMetaData(const QJsonObject &pluginMetaData);

private:
    PluginSpec *q;

    /*存储错误信息，并恒返回true*/
    bool reportError(const QString &err);
    /*返回插件版本的正则表达式*/
    static const QRegularExpression &versionRegExp();
};

} // namespace Internal
} // namespace ExtensionSystem
