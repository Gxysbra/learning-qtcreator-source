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

#include "pluginspec.h"

#include "pluginspec_p.h"
#include "iplugin.h"
#include "iplugin_p.h"
#include "pluginmanager.h"

#include <utils/algorithm.h>
#include <utils/hostosinfo.h>
#include <utils/qtcassert.h>
#include <utils/stringutils.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QPluginLoader>

/*!
    \class ExtensionSystem::PluginDependency
    \inheaderfile extensionsystem/pluginspec.h
    \inmodule QtCreator

    \brief The PluginDependency class contains the name and required compatible
    version number of a plugin's dependency.

    This reflects the data of a dependency object in the plugin's meta data.
    The name and version are used to resolve the dependency. That is,
    a plugin with the given name and
    plugin \c {compatibility version <= dependency version <= plugin version} is searched for.

    See also ExtensionSystem::IPlugin for more information about plugin dependencies and
    version matching.
*/

/*!
    \variable ExtensionSystem::PluginDependency::name
    String identifier of the plugin.
*/

/*!
    \variable ExtensionSystem::PluginDependency::version
    Version string that a plugin must match to fill this dependency.
*/

/*!
    \variable ExtensionSystem::PluginDependency::type
    Defines whether the dependency is required or optional.
    \sa ExtensionSystem::PluginDependency::Type
*/

/*!
    \enum ExtensionSystem::PluginDependency::Type
    Whether the dependency is required or optional.
    \value Required
           Dependency needs to be there.
    \value Optional
           Dependency is not necessarily needed. You need to make sure that
           the plugin is able to load without this dependency installed, so
           for example you may not link to the dependency's library.
    \value Test
           Dependency needs to be force-loaded for running tests of the plugin.
*/

/*!
    \class ExtensionSystem::PluginSpec
    \inheaderfile extensionsystem/pluginspec.h
    \inmodule QtCreator

    \brief The PluginSpec class contains the information of the plugin's embedded meta data
    and information about the plugin's current state.

    The plugin spec is also filled with more information as the plugin
    goes through its loading process (see PluginSpec::State).
    If an error occurs, the plugin spec is the place to look for the
    error details.
*/

/*!
    \enum ExtensionSystem::PluginSpec::State
    The State enum indicates the states the plugin goes through while
    it is being loaded.

    The state gives a hint on what went wrong in case of an error.

    \value  Invalid
            Starting point: Even the plugin meta data was not read.
    \value  Read
            The plugin meta data has been successfully read, and its
            information is available via the PluginSpec.
    \value  Resolved
            The dependencies given in the description file have been
            successfully found, and are available via the dependencySpecs() function.
    \value  Loaded
            The plugin's library is loaded and the plugin instance created
            (available through plugin()).
    \value  Initialized
            The plugin instance's IPlugin::initialize() function has been called
            and returned a success value.
    \value  Running
            The plugin's dependencies are successfully initialized and
            extensionsInitialized has been called. The loading process is
            complete.
    \value Stopped
            The plugin has been shut down, i.e. the plugin's IPlugin::aboutToShutdown() function has been called.
    \value Deleted
            The plugin instance has been deleted.
*/

/*!
    \class ExtensionSystem::PluginArgumentDescription
    \inheaderfile extensionsystem/pluginspec.h
    \inmodule QtCreator

    \brief The PluginArgumentDescriptions class holds a list of descriptions of
    command line arguments that a plugin processes.

    \sa PluginSpec::argumentDescriptions()
*/

using namespace ExtensionSystem;
using namespace ExtensionSystem::Internal;

/*!
    \fn uint ExtensionSystem::qHash(const ExtensionSystem::PluginDependency &value)
    \internal
*/
uint ExtensionSystem::qHash(const PluginDependency &value)
{
    return qHash(value.name);
}

/*!
    \internal
*/
bool PluginDependency::operator==(const PluginDependency &other) const
{
    return name == other.name && version == other.version && type == other.type;
}

static QString typeString(PluginDependency::Type type)
{
    switch (type) {
    case PluginDependency::Optional:
        return QString(", optional");
    case PluginDependency::Test:
        return QString(", test");
    case PluginDependency::Required:
    default:
        return QString();
    }
}

/*!
    \internal
*/
QString PluginDependency::toString() const
{
    return name + " (" + version + typeString(type) + ")";
}

/*!
    \internal
*/
PluginSpec::PluginSpec()
    : d(new PluginSpecPrivate(this))
{
}

/*!
    \internal
*/
PluginSpec::~PluginSpec()
{
    delete d;
    d = nullptr;
}

/*!
    Returns the plugin name. This is valid after the PluginSpec::Read state is
    reached.
*/
QString PluginSpec::name() const
{
    return d->name;
}

/*!
    Returns the plugin version. This is valid after the PluginSpec::Read state
    is reached.
*/
QString PluginSpec::version() const
{
    return d->version;
}

/*!
    Returns the plugin compatibility version. This is valid after the
    PluginSpec::Read state is reached.
*/
QString PluginSpec::compatVersion() const
{
    return d->compatVersion;
}

/*!
    Returns the plugin vendor. This is valid after the PluginSpec::Read
    state is reached.
*/
QString PluginSpec::vendor() const
{
    return d->vendor;
}

/*!
    Returns the plugin copyright. This is valid after the PluginSpec::Read
     state is reached.
*/
QString PluginSpec::copyright() const
{
    return d->copyright;
}

/*!
    Returns the plugin license. This is valid after the PluginSpec::Read
    state is reached.
*/
QString PluginSpec::license() const
{
    return d->license;
}

/*!
    Returns the plugin description. This is valid after the PluginSpec::Read
    state is reached.
*/
QString PluginSpec::description() const
{
    return d->description;
}

/*!
    Returns the plugin URL where you can find more information about the plugin.
    This is valid after the PluginSpec::Read state is reached.
*/
QString PluginSpec::url() const
{
    return d->url;
}

/*!
    Returns the category that the plugin belongs to. Categories are used to
    group plugins together in the UI.
    Returns an empty string if the plugin does not belong to a category.
*/
QString PluginSpec::category() const
{
    return d->category;
}

QString PluginSpec::revision() const
{
    const QJsonValue revision = metaData().value("Revision");
    if (revision.isString())
        return revision.toString();
    return QString();
}

/*!
    Returns a QRegularExpression matching the platforms this plugin works on.
    An empty pattern implies all platforms.
    \since 3.0
*/

QRegularExpression PluginSpec::platformSpecification() const
{
    return d->platformSpecification;
}

/*!
    Returns whether the plugin works on the host platform.
*/
bool PluginSpec::isAvailableForHostPlatform() const
{
    return d->platformSpecification.pattern().isEmpty()
            || d->platformSpecification.match(PluginManager::platformName()).hasMatch();
}

/*!
    Returns whether the plugin is required.
*/
bool PluginSpec::isRequired() const
{
    return d->required;
}

/*!
    Returns whether the plugin has its experimental flag set.
*/
bool PluginSpec::isExperimental() const
{
    return d->experimental;
}

/*!
    Returns whether the plugin is enabled by default.
    A plugin might be disabled because the plugin is experimental, or because
    the installation settings define it as disabled by default.
*/
bool PluginSpec::isEnabledByDefault() const
{
    return d->enabledByDefault;
}

/*!
    Returns whether the plugin should be loaded at startup,
    taking into account the default enabled state, and the user's settings.

    \note This function might return \c false even if the plugin is loaded
    as a requirement of another enabled plugin.

    \sa isEffectivelyEnabled()
*/
bool PluginSpec::isEnabledBySettings() const
{
    return d->enabledBySettings;
}

/*!
    Returns whether the plugin is loaded at startup.
    \sa isEnabledBySettings()
*/
bool PluginSpec::isEffectivelyEnabled() const
{
    /*
     * 先判断是否适用于该平台
     * 然后判断是否于命令行强制启动或间接关联启动
     * 再判断是否命令行强制禁止
     * 最后判断是否由配置文件启动
     */
    if (!isAvailableForHostPlatform())
        return false;
    if (isForceEnabled() || isEnabledIndirectly())
        return true;
    if (isForceDisabled())
        return false;
    return isEnabledBySettings();
}

/*!
    Returns \c true if loading was not done due to user unselecting this
    plugin or its dependencies.
*/
bool PluginSpec::isEnabledIndirectly() const
{
    return d->enabledIndirectly;
}

/*!
    Returns whether the plugin is enabled via the \c -load option on the
    command line.
*/
bool PluginSpec::isForceEnabled() const
{
    return d->forceEnabled;
}

/*!
    Returns whether the plugin is disabled via the \c -noload option on the
     command line.
*/
bool PluginSpec::isForceDisabled() const
{
    return d->forceDisabled;
}

/*!
    The plugin dependencies. This is valid after the PluginSpec::Read state is reached.
*/
QVector<PluginDependency> PluginSpec::dependencies() const
{
    return d->dependencies;
}

/*!
    Returns the plugin meta data.
*/
QJsonObject PluginSpec::metaData() const
{
    return d->metaData;
}

/*!
    Returns a list of descriptions of command line arguments the plugin processes.
*/

PluginSpec::PluginArgumentDescriptions PluginSpec::argumentDescriptions() const
{
    return d->argumentDescriptions;
}

/*!
    Returns the absolute path to the directory containing the plugin.
*/
QString PluginSpec::location() const
{
    return d->location;
}

/*!
    Returns the absolute path to the plugin.
*/
QString PluginSpec::filePath() const
{
    return d->filePath;
}

/*!
    Returns command line arguments specific to the plugin. Set at startup.
*/

QStringList PluginSpec::arguments() const
{
    return d->arguments;
}

/*!
    Sets the command line arguments specific to the plugin to \a arguments.
*/

void PluginSpec::setArguments(const QStringList &arguments)
{
    d->arguments = arguments;
}

/*!
    Adds \a argument to the command line arguments specific to the plugin.
*/

void PluginSpec::addArgument(const QString &argument)
{
    d->arguments.push_back(argument);
}


/*!
    Returns the state in which the plugin currently is.
    See the description of the PluginSpec::State enum for details.
*/
PluginSpec::State PluginSpec::state() const
{
    return d->state;
}

/*!
    Returns whether an error occurred while reading or starting the plugin.
*/
bool PluginSpec::hasError() const
{
    return d->hasError;
}

/*!
    Returns a detailed, possibly multi-line, error description in case of an
    error.
*/
QString PluginSpec::errorString() const
{
    return d->errorString;
}

/*!
    Returns whether this plugin can be used to fill in a dependency of the given
    \a pluginName and \a version.

        \sa PluginSpec::dependencies()
*/
bool PluginSpec::provides(const QString &pluginName, const QString &version) const
{
    return d->provides(pluginName, version);
}

/*!
    Returns the corresponding IPlugin instance, if the plugin library has
    already been successfully loaded. That is, the PluginSpec::Loaded state
    is reached.
*/
IPlugin *PluginSpec::plugin() const
{
    return d->plugin;
}

/*!
    Returns the list of dependencies, already resolved to existing plugin specs.
    Valid if PluginSpec::Resolved state is reached.

    \sa PluginSpec::dependencies()
*/
QHash<PluginDependency, PluginSpec *> PluginSpec::dependencySpecs() const
{
    return d->dependencySpecs;
}

/*!
    Returns whether the plugin requires any of the plugins specified by
    \a plugins.
*/
bool PluginSpec::requiresAny(const QSet<PluginSpec *> &plugins) const
{
    return Utils::anyOf(d->dependencySpecs.keys(), [this, &plugins](const PluginDependency &dep) {
        return dep.type == PluginDependency::Required
               && plugins.contains(d->dependencySpecs.value(dep));
    });
}

/*!
    Sets whether the plugin should be loaded at startup to \a value.

    \sa isEnabledBySettings()
*/
void PluginSpec::setEnabledBySettings(bool value)
{
    d->setEnabledBySettings(value);
}

//==========PluginSpecPrivate==================
/*
 * 匿名命名空间
 * 增加一层间接性，集中管理插件元数据定义，从而当插件元数据定义需要修改时直接到这修改即可
 */
namespace {
    const char PLUGIN_METADATA[] = "MetaData";
    const char PLUGIN_NAME[] = "Name";
    const char PLUGIN_VERSION[] = "Version";
    const char PLUGIN_COMPATVERSION[] = "CompatVersion";
    const char PLUGIN_REQUIRED[] = "Required";
    const char PLUGIN_EXPERIMENTAL[] = "Experimental";
    const char PLUGIN_DISABLED_BY_DEFAULT[] = "DisabledByDefault";
    const char VENDOR[] = "Vendor";
    const char COPYRIGHT[] = "Copyright";
    const char LICENSE[] = "License";
    const char DESCRIPTION[] = "Description";
    const char URL[] = "Url";
    const char CATEGORY[] = "Category";
    const char PLATFORM[] = "Platform";
    const char DEPENDENCIES[] = "Dependencies";
    const char DEPENDENCY_NAME[] = "Name";
    const char DEPENDENCY_VERSION[] = "Version";
    const char DEPENDENCY_TYPE[] = "Type";
    const char DEPENDENCY_TYPE_SOFT[] = "optional";
    const char DEPENDENCY_TYPE_HARD[] = "required";
    const char DEPENDENCY_TYPE_TEST[] = "test";
    const char ARGUMENTS[] = "Arguments";
    const char ARGUMENT_NAME[] = "Name";
    const char ARGUMENT_PARAMETER[] = "Parameter";
    const char ARGUMENT_DESCRIPTION[] = "Description";
}
/*!
    \internal
*/
PluginSpecPrivate::PluginSpecPrivate(PluginSpec *spec)
    : q(spec)
{
    if (Utils::HostOsInfo::isMacHost())
        loader.setLoadHints(QLibrary::ExportExternalSymbolsHint);
}

/*!
    \internal
    Returns false if the file does not represent a Qt Creator plugin.
*/
bool PluginSpecPrivate::read(const QString &fileName)
{
    qCDebug(pluginLog) << "\nReading meta data of" << fileName;
    // 初始化插件元数据
    // 初始化为空字符串
    name
        = version
        = compatVersion
        = vendor
        = copyright
        = license
        = description
        = url
        = category
        = location
        = QString();
    state = PluginSpec::Invalid;
    hasError = false;
    errorString.clear();
    dependencies.clear();
    metaData = QJsonObject();
    QFileInfo fileInfo(fileName);
    location = fileInfo.absolutePath();
    filePath = fileInfo.absoluteFilePath();
    loader.setFileName(filePath);
    // 如果fileName有问题或其他原因导致加载不了插件，则返回false
    // 同时由于以上初始化过程，插件元数据有相应的值
    if (loader.fileName().isEmpty()) {
        qCDebug(pluginLog) << "Cannot open file";
        return false;
    }

    // 读取插件元数据
    if (!readMetaData(loader.metaData()))
        return false;

    // 如果是系统识别的插件，元数据解析有误也会变为Read状态，
    // 但是同时也会设置错误信息
    state = PluginSpec::Read;
    return true;
}

void PluginSpecPrivate::setEnabledBySettings(bool value)
{
    enabledBySettings = value;
}

void PluginSpecPrivate::setEnabledByDefault(bool value)
{
    enabledByDefault = value;
}

void PluginSpecPrivate::setForceEnabled(bool value)
{
    forceEnabled = value;
    if (value)
        forceDisabled = false;
}

void PluginSpecPrivate::setForceDisabled(bool value)
{
    if (value)
        forceEnabled = false;
    forceDisabled = value;
}

/*!
    \internal
*/
bool PluginSpecPrivate::reportError(const QString &err)
{
    errorString = err;
    hasError = true;
    return true;
}

static inline QString msgValueMissing(const char *key)
{
    return QCoreApplication::translate("PluginSpec", "\"%1\" is missing").arg(QLatin1String(key));
}

static inline QString msgValueIsNotAString(const char *key)
{
    return QCoreApplication::translate("PluginSpec", "Value for key \"%1\" is not a string")
            .arg(QLatin1String(key));
}

static inline QString msgValueIsNotABool(const char *key)
{
    return QCoreApplication::translate("PluginSpec", "Value for key \"%1\" is not a bool")
            .arg(QLatin1String(key));
}

static inline QString msgValueIsNotAObjectArray(const char *key)
{
    return QCoreApplication::translate("PluginSpec", "Value for key \"%1\" is not an array of objects")
            .arg(QLatin1String(key));
}

static inline QString msgValueIsNotAMultilineString(const char *key)
{
    return QCoreApplication::translate("PluginSpec", "Value for key \"%1\" is not a string and not an array of strings")
            .arg(QLatin1String(key));
}

static inline QString msgInvalidFormat(const char *key, const QString &content)
{
    return QCoreApplication::translate("PluginSpec", "Value \"%2\" for key \"%1\" has invalid format")
            .arg(QLatin1String(key), content);
}

/*!
    \internal
*/
bool PluginSpecPrivate::readMetaData(const QJsonObject &pluginMetaData)
{
    qCDebug(pluginLog) << "MetaData:" << QJsonDocument(pluginMetaData).toJson();
    QJsonValue value;
    value = pluginMetaData.value(QLatin1String("IID"));
    // 1. 检查IID
    if (!value.isString()) {
        qCDebug(pluginLog) << "Not a plugin (no string IID found)";
        return false;
    }
    // 不是本系统所识别的插件
    if (value.toString() != PluginManager::pluginIID()) {
        qCDebug(pluginLog) << "Plugin ignored (IID does not match)";
        return false;
    }

    // 2. 检查制作插件时嵌入的元数据
    value = pluginMetaData.value(QLatin1String(PLUGIN_METADATA));
    // 如果不存在合法的元数据，则报告错误信息并返回true
    // 即该插件代表本系统所识别的插件，但是元数据有误
    if (!value.isObject())
        return reportError(tr("Plugin meta data not found"));
    // 更新插件元数据
    metaData = value.toObject();

    // 3. 检测插件名
    value = metaData.value(QLatin1String(PLUGIN_NAME));
    if (value.isUndefined())
        return reportError(msgValueMissing(PLUGIN_NAME));
    if (!value.isString())
        return reportError(msgValueIsNotAString(PLUGIN_NAME));
    name = value.toString();

    // 4. 检测插件版本
    value = metaData.value(QLatin1String(PLUGIN_VERSION));
    if (value.isUndefined())
        return reportError(msgValueMissing(PLUGIN_VERSION));
    if (!value.isString())
        return reportError(msgValueIsNotAString(PLUGIN_VERSION));
    version = value.toString();
    if (!isValidVersion(version))
        return reportError(msgInvalidFormat(PLUGIN_VERSION, version));

    // 5. 检测插件兼容版本
    value = metaData.value(QLatin1String(PLUGIN_COMPATVERSION));
    if (!value.isUndefined() && !value.isString())
        return reportError(msgValueIsNotAString(PLUGIN_COMPATVERSION));
    // 如果未定义兼容版本则采用插件版本
    compatVersion = value.toString(version);
    if (!value.isUndefined() && !isValidVersion(compatVersion))
        return reportError(msgInvalidFormat(PLUGIN_COMPATVERSION, compatVersion));

    // 6. 检测插件是否必须
    value = metaData.value(QLatin1String(PLUGIN_REQUIRED));
    if (!value.isUndefined() && !value.isBool())
        return reportError(msgValueIsNotABool(PLUGIN_REQUIRED));
    // 如果未定义则默认为false
    required = value.toBool(false);
    qCDebug(pluginLog) << "required =" << required;

    // 7. 检测插件是否为实验性质
    value = metaData.value(QLatin1String(PLUGIN_EXPERIMENTAL));
    if (!value.isUndefined() && !value.isBool())
        return reportError(msgValueIsNotABool(PLUGIN_EXPERIMENTAL));
    // 如果未定义则默认为false
    experimental = value.toBool(false);
    qCDebug(pluginLog) << "experimental =" << experimental;

    // 8. 检测插件是否默认禁用
    value = metaData.value(QLatin1String(PLUGIN_DISABLED_BY_DEFAULT));
    if (!value.isUndefined() && !value.isBool())
        return reportError(msgValueIsNotABool(PLUGIN_DISABLED_BY_DEFAULT));
    enabledByDefault = !value.toBool(false);
    qCDebug(pluginLog) << "enabledByDefault =" << enabledByDefault;

    // 如果是实验性质的插件则默认禁用
    if (experimental)
        enabledByDefault = false;
    enabledBySettings = enabledByDefault;

    // 9. 检测插件供应商
    value = metaData.value(QLatin1String(VENDOR));
    if (!value.isUndefined() && !value.isString())
        return reportError(msgValueIsNotAString(VENDOR));
    vendor = value.toString();

    // 10. 检测插件版权
    value = metaData.value(QLatin1String(COPYRIGHT));
    if (!value.isUndefined() && !value.isString())
        return reportError(msgValueIsNotAString(COPYRIGHT));
    copyright = value.toString();

    // 11. 检测插件描述
    value = metaData.value(QLatin1String(DESCRIPTION));
    if (!value.isUndefined() && !Utils::readMultiLineString(value, &description))
        return reportError(msgValueIsNotAString(DESCRIPTION));

    // 12. 检测插件URL
    value = metaData.value(QLatin1String(URL));
    if (!value.isUndefined() && !value.isString())
        return reportError(msgValueIsNotAString(URL));
    url = value.toString();

    // 13. 检测插件类别
    value = metaData.value(QLatin1String(CATEGORY));
    if (!value.isUndefined() && !value.isString())
        return reportError(msgValueIsNotAString(CATEGORY));
    category = value.toString();

    // 14. 检测插件许可证
    value = metaData.value(QLatin1String(LICENSE));
    if (!value.isUndefined() && !Utils::readMultiLineString(value, &license))
        return reportError(msgValueIsNotAMultilineString(LICENSE));

    // 15. 检测插件平台
    value = metaData.value(QLatin1String(PLATFORM));
    if (!value.isUndefined() && !value.isString())
        return reportError(msgValueIsNotAString(PLATFORM));
    const QString platformSpec = value.toString().trimmed();
    if (!platformSpec.isEmpty()) {
        platformSpecification.setPattern(platformSpec);
        if (!platformSpecification.isValid())
            return reportError(tr("Invalid platform specification \"%1\": %2")
                               .arg(platformSpec, platformSpecification.errorString()));
    }

    // 16. 检测插件依赖项(重头戏)
    value = metaData.value(QLatin1String(DEPENDENCIES));
    if (!value.isUndefined() && !value.isArray())
        return reportError(msgValueIsNotAObjectArray(DEPENDENCIES));
    // 如果定义了依赖项并且是一个数组
    if (!value.isUndefined()) {
        const QJsonArray array = value.toArray();
        for (const QJsonValue &v : array) {
            // 如果不是对象数组则报错
            if (!v.isObject())
                return reportError(msgValueIsNotAObjectArray(DEPENDENCIES));
            QJsonObject dependencyObject = v.toObject();
            PluginDependency dep;
            // 检测依赖项名字
            value = dependencyObject.value(QLatin1String(DEPENDENCY_NAME));
            if (value.isUndefined())
                return reportError(tr("Dependency: %1").arg(msgValueMissing(DEPENDENCY_NAME)));
            if (!value.isString())
                return reportError(tr("Dependency: %1").arg(msgValueIsNotAString(DEPENDENCY_NAME)));
            dep.name = value.toString();
            // 检测依赖项版本
            value = dependencyObject.value(QLatin1String(DEPENDENCY_VERSION));
            if (!value.isUndefined() && !value.isString())
                return reportError(tr("Dependency: %1").arg(msgValueIsNotAString(DEPENDENCY_VERSION)));
            dep.version = value.toString();
            if (!isValidVersion(dep.version))
                return reportError(tr("Dependency: %1").arg(msgInvalidFormat(DEPENDENCY_VERSION,
                                                                             dep.version)));
            // 检测依赖项类型
            dep.type = PluginDependency::Required;
            value = dependencyObject.value(QLatin1String(DEPENDENCY_TYPE));
            if (!value.isUndefined() && !value.isString())
                return reportError(tr("Dependency: %1").arg(msgValueIsNotAString(DEPENDENCY_TYPE)));
            // 如果依赖项类型未定义，则默认为Required
            if (!value.isUndefined()) {
                const QString typeValue = value.toString();
                if (typeValue.toLower() == QLatin1String(DEPENDENCY_TYPE_HARD)) {
                    dep.type = PluginDependency::Required;
                } else if (typeValue.toLower() == QLatin1String(DEPENDENCY_TYPE_SOFT)) {
                    dep.type = PluginDependency::Optional;
                } else if (typeValue.toLower() == QLatin1String(DEPENDENCY_TYPE_TEST)) {
                    dep.type = PluginDependency::Test;
                } else {
                    return reportError(tr("Dependency: \"%1\" must be \"%2\" or \"%3\" (is \"%4\").")
                                       .arg(QLatin1String(DEPENDENCY_TYPE),
                                            QLatin1String(DEPENDENCY_TYPE_HARD),
                                            QLatin1String(DEPENDENCY_TYPE_SOFT),
                                            typeValue));
                }
            }
            dependencies.append(dep);
        }
    }

    // 17. 检测插件参数
    value = metaData.value(QLatin1String(ARGUMENTS));
    if (!value.isUndefined() && !value.isArray())
        return reportError(msgValueIsNotAObjectArray(ARGUMENTS));
    if (!value.isUndefined()) {
        const QJsonArray array = value.toArray();
        for (const QJsonValue &v : array) {
            if (!v.isObject())
                return reportError(msgValueIsNotAObjectArray(ARGUMENTS));
            QJsonObject argumentObject = v.toObject();
            PluginArgumentDescription arg;
            value = argumentObject.value(QLatin1String(ARGUMENT_NAME));
            if (value.isUndefined())
                return reportError(tr("Argument: %1").arg(msgValueMissing(ARGUMENT_NAME)));
            if (!value.isString())
                return reportError(tr("Argument: %1").arg(msgValueIsNotAString(ARGUMENT_NAME)));
            arg.name = value.toString();
            if (arg.name.isEmpty())
                return reportError(tr("Argument: \"%1\" is empty").arg(QLatin1String(ARGUMENT_NAME)));
            value = argumentObject.value(QLatin1String(ARGUMENT_DESCRIPTION));
            if (!value.isUndefined() && !value.isString())
                return reportError(tr("Argument: %1").arg(msgValueIsNotAString(ARGUMENT_DESCRIPTION)));
            arg.description = value.toString();
            value = argumentObject.value(QLatin1String(ARGUMENT_PARAMETER));
            if (!value.isUndefined() && !value.isString())
                return reportError(tr("Argument: %1").arg(msgValueIsNotAString(ARGUMENT_PARAMETER)));
            arg.parameter = value.toString();
            argumentDescriptions.append(arg);
            qCDebug(pluginLog) << "Argument:" << arg.name << "Parameter:" << arg.parameter
                               << "Description:" << arg.description;
        }
    }

    return true;
}

/*!
    \internal
*/
bool PluginSpecPrivate::provides(const QString &pluginName, const QString &pluginVersion) const
{
    /*插件名是否匹配，忽略大小写*/
    if (QString::compare(pluginName, name, Qt::CaseInsensitive) != 0)
        return false;
    /*当满足compatVersion <= pluginVersion <= version条件时，返回true；否则，返回false*/
    return (versionCompare(version, pluginVersion) >= 0) && (versionCompare(compatVersion, pluginVersion) <= 0);
}

/*!
    \internal
*/
const QRegularExpression &PluginSpecPrivate::versionRegExp()
{
    /*插件版本的正则表达式*/
    static const QRegularExpression reg("^([0-9]+)(?:[.]([0-9]+))?(?:[.]([0-9]+))?(?:_([0-9]+))?$");
    return reg;
}
/*!
    \internal
*/
bool PluginSpecPrivate::isValidVersion(const QString &version)
{
    /*判断给定的version参数是否是有效的插件版本格式*/
    return versionRegExp().match(version).hasMatch();
}

/*!
    \internal
*/
int PluginSpecPrivate::versionCompare(const QString &version1, const QString &version2)
{
    /*解析插件版本*/
    const QRegularExpressionMatch match1 = versionRegExp().match(version1);
    const QRegularExpressionMatch match2 = versionRegExp().match(version2);
    /*如果不是有效的插件版本格式，则返回0，即认为两者匹配，即无效格式版本被认为与任何版本匹配*/
    if (!match1.hasMatch())
        return 0;
    if (!match2.hasMatch())
        return 0;
    /*插件版本的格式为x.y.z_n，其中x、y、z和n是非负整数*/
    for (int i = 0; i < 4; ++i) {
        const int number1 = match1.captured(i + 1).toInt();
        const int number2 = match2.captured(i + 1).toInt();
        /*version1 < version2*/
        if (number1 < number2)
            return -1;
        /*version1 > version2*/
        if (number1 > number2)
            return 1;
    }
    return 0;
}

/*!
    \internal
*/
bool PluginSpecPrivate::resolveDependencies(const QVector<PluginSpec *> &specs)
{
    /*解析条件检查*/
    /*如果在解析前有错误发生，如不能读取插件元数据，则返回false*/
    if (hasError)
        return false;
    /*如果依赖项已经找到，则退回到Read状态重新解析*/
    if (state == PluginSpec::Resolved)
        state = PluginSpec::Read; // Go back, so we just re-resolve the dependencies.
    /*如果不是Read状态，则报错*/
    if (state != PluginSpec::Read) {
        errorString = QCoreApplication::translate("PluginSpec", "Resolving dependencies failed because state != Read");
        hasError = true;
        return false;
    }
    /*正式解析过程*/
    QHash<PluginDependency, PluginSpec *> resolvedDependencies;
    /*遍历依赖项列表*/
    for (const PluginDependency &dependency : qAsConst(dependencies)) {
        /*遍历已注册插件列表，查找是否有匹配的项*/
        PluginSpec * const found = Utils::findOrDefault(specs, [&dependency](PluginSpec *spec) {
            return spec->provides(dependency.name, dependency.version);
        });
        /*如果未找到依赖项*/
        if (!found) {
            /*并且依赖项类型为Required，则报错*/
            if (dependency.type == PluginDependency::Required) {
                hasError = true;
                if (!errorString.isEmpty())
                    errorString.append(QLatin1Char('\n'));
                errorString.append(QCoreApplication::translate("PluginSpec", "Could not resolve dependency '%1(%2)'")
                    .arg(dependency.name).arg(dependency.version));
            }
            // 如果依赖项类型不为Requested，则忽略
            continue;
        }
        /*如果找到了依赖项，则记录*/
        resolvedDependencies.insert(dependency, found);
    }
    /*如果有一个依赖项未找到，则解析失败，返回false*/
    if (hasError)
        return false;

    /*如果成功找到所有依赖项，则保存解析结果，并转移状态，返回true*/
    /*解析的是直接依赖项，解析结果中包含了全部Requested依赖项和部分Optional依赖项*/
    dependencySpecs = resolvedDependencies;

    state = PluginSpec::Resolved;

    return true;
}

// returns the plugins that it actually indirectly enabled
QVector<PluginSpec *> PluginSpecPrivate::enableDependenciesIndirectly(bool enableTestDependencies)
{
    /*如果不启用该插件，则什么都不做*/
    if (!q->isEffectivelyEnabled()) // plugin not enabled, nothing to do
        return {};
    QVector<PluginSpec *> enabled;
    /*遍历该插件的依赖项*/
    for (auto it = dependencySpecs.cbegin(), end = dependencySpecs.cend(); it != end; ++it) {
        // 可以认为除Required的依赖项都不用启用
        if (it.key().type != PluginDependency::Required
                && (!enableTestDependencies || it.key().type != PluginDependency::Test))
            continue;
        PluginSpec *dependencySpec = it.value();
        // 如果依赖项已经是启动时加载了，则不做处理
        if (!dependencySpec->isEffectivelyEnabled()) {
            dependencySpec->d->enabledIndirectly = true;
            enabled << dependencySpec;
        }
    }
    return enabled;
}

/*!
    \internal
*/
bool PluginSpecPrivate::loadLibrary()
{
    // 如果该插件有错误发生，则退出并返回false
    if (hasError)
        return false;
    // 如果插件不为Resolved状态，则报错
    if (state != PluginSpec::Resolved) {
        // 如果该插件已加载，则直接返回true
        if (state == PluginSpec::Loaded)
            return true;
        errorString = QCoreApplication::translate("PluginSpec", "Loading the library failed because state != Resolved");
        hasError = true;
        return false;
    }
    // 插件加载失败，则报错
    if (!loader.load()) {
        hasError = true;
        errorString = QDir::toNativeSeparators(filePath)
            + QString::fromLatin1(": ") + loader.errorString();
        return false;
    }
    // 判断是否为系统插件，即派生自IPlugin
    auto *pluginObject = qobject_cast<IPlugin*>(loader.instance());
    if (!pluginObject) {
        hasError = true;
        errorString = QCoreApplication::translate("PluginSpec", "Plugin is not valid (does not derive from IPlugin)");
        loader.unload();
        return false;
    }
    state = PluginSpec::Loaded;
    plugin = pluginObject;
    plugin->d->pluginSpec = q;
    return true;
}

/*!
    \internal
*/
bool PluginSpecPrivate::initializePlugin()
{
    if (hasError)
        return false;
    if (state != PluginSpec::Loaded) {
        if (state == PluginSpec::Initialized)
            return true;
        errorString = QCoreApplication::translate("PluginSpec", "Initializing the plugin failed because state != Loaded");
        hasError = true;
        return false;
    }
    if (!plugin) {
        errorString = QCoreApplication::translate("PluginSpec", "Internal error: have no plugin instance to initialize");
        hasError = true;
        return false;
    }
    QString err;
    if (!plugin->initialize(arguments, &err)) {
        errorString = QCoreApplication::translate("PluginSpec", "Plugin initialization failed: %1").arg(err);
        hasError = true;
        return false;
    }
    state = PluginSpec::Initialized;
    return true;
}

/*!
    \internal
*/
bool PluginSpecPrivate::initializeExtensions()
{
    if (hasError)
        return false;
    if (state != PluginSpec::Initialized) {
        if (state == PluginSpec::Running)
            return true;
        errorString = QCoreApplication::translate("PluginSpec", "Cannot perform extensionsInitialized because state != Initialized");
        hasError = true;
        return false;
    }
    if (!plugin) {
        errorString = QCoreApplication::translate("PluginSpec", "Internal error: have no plugin instance to perform extensionsInitialized");
        hasError = true;
        return false;
    }
    plugin->extensionsInitialized();
    state = PluginSpec::Running;
    return true;
}

/*!
    \internal
*/
bool PluginSpecPrivate::delayedInitialize()
{
    if (hasError)
        return false;
    if (state != PluginSpec::Running)
        return false;
    if (!plugin) {
        errorString = QCoreApplication::translate("PluginSpec", "Internal error: have no plugin instance to perform delayedInitialize");
        hasError = true;
        return false;
    }
    return plugin->delayedInitialize();
}

/*!
    \internal
*/
IPlugin::ShutdownFlag PluginSpecPrivate::stop()
{
    if (!plugin)
        return IPlugin::SynchronousShutdown;
    state = PluginSpec::Stopped;
    return plugin->aboutToShutdown();
}

/*!
    \internal
*/
void PluginSpecPrivate::kill()
{
    if (!plugin)
        return;
    delete plugin;
    plugin = nullptr;
    state = PluginSpec::Deleted;
}
