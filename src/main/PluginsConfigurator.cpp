/**************************************************************************************************
 * Copyright (c) 2021 Calypso Networks Association https://calypsonet.org/                        *
 *                                                                                                *
 * See the NOTICE file(s) distributed with this work for additional information regarding         *
 * copyright ownership.                                                                           *
 *                                                                                                *
 * This program and the accompanying materials are made available under the terms of the Eclipse  *
 * Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0                  *
 *                                                                                                *
 * SPDX-License-Identifier: EPL-2.0                                                               *
 **************************************************************************************************/

#include "PluginsConfigurator.h"

/* Keyple Core Util */
#include "Arrays.h"
#include "IllegalArgumentException.h"
#include "IllegalStateException.h"
#include "KeypleAssert.h"

/* Keyple Core Service */
#include "PoolPlugin.h"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using namespace keyple::core::service;
using namespace keyple::core::util;
using namespace keyple::core::util::cpp;
using namespace keyple::core::util::cpp::exception;

using AllocationStrategy = PluginsConfigurator::AllocationStrategy;
using ConfiguredPlugin = PluginsConfigurator::ConfiguredPlugin;

/* BUILDER -------------------------------------------------------------------------------------- */

PluginsConfigurator::Builder::Builder()
: mAllocationStrategyConfigured(false), 
  mUsageTimeoutMillisConfigured(false),
  mUsageTimeoutMillis(0),
  mAllocationStrategy(AllocationStrategy::FIRST) {}

PluginsConfigurator::Builder& PluginsConfigurator::Builder::withAllocationStrategy(
    const AllocationStrategy allocationStrategy)
{
    if (mAllocationStrategyConfigured == true) {
        throw IllegalStateException("Allocation strategy already configured.");
    }

    mAllocationStrategy = allocationStrategy;
    mAllocationStrategyConfigured = true;

    return *this;
}

PluginsConfigurator::Builder& PluginsConfigurator::Builder::withUsageTimeout(
    const int usageTimeoutMillis)
{
    Assert::getInstance().greaterOrEqual(usageTimeoutMillis, 1, "usageTimeoutMillis");

    if (mUsageTimeoutMillisConfigured == true) {
        throw IllegalStateException("Usage timeout already configured.");
    }

    mUsageTimeoutMillis = usageTimeoutMillis;
    mUsageTimeoutMillisConfigured = true;

    return *this;
}

PluginsConfigurator::Builder& PluginsConfigurator::Builder::addPlugin(
    std::shared_ptr<Plugin> plugin, std::shared_ptr<ReaderConfiguratorSpi> readerConfiguratorSpi)
{
    return addPluginWithMonitoring(plugin, readerConfiguratorSpi, nullptr, nullptr);
}

PluginsConfigurator::Builder& PluginsConfigurator::Builder::addPluginWithMonitoring(
    std::shared_ptr<Plugin> plugin,
    std::shared_ptr<ReaderConfiguratorSpi> readerConfiguratorSpi,
    std::shared_ptr<PluginObservationExceptionHandlerSpi> pluginObservationExceptionHandlerSpi,
    std::shared_ptr<CardReaderObservationExceptionHandlerSpi> readerObservationExceptionHandlerSpi)
{
    Assert::getInstance().notNull(plugin, "plugin")
                         .notNull(readerConfiguratorSpi, "readerConfiguratorSpi");

    const auto pool = std::dynamic_pointer_cast<PoolPlugin>(plugin);
    if (pool != nullptr) {
        throw IllegalArgumentException("Plugin must be an instance of Plugin or ObservablePlugin");
    }

    if (Arrays::contains(mPlugins, plugin)) {
        throw IllegalStateException("Plugin already configured.");
    }

    mPlugins.push_back(plugin);
    mConfiguredPlugins.push_back(
        std::make_shared<ConfiguredPlugin>(plugin,
                                           readerConfiguratorSpi,
                                           pluginObservationExceptionHandlerSpi,
                                           readerObservationExceptionHandlerSpi));

    return *this;
}

std::shared_ptr<PluginsConfigurator> PluginsConfigurator::Builder::build()
{
    if (mPlugins.empty()) {
        throw IllegalStateException("No plugin was configured.");
    }

    if (mAllocationStrategyConfigured == false) {
        mAllocationStrategy = AllocationStrategy::FIRST;
        mAllocationStrategyConfigured = true;
    }

    if (mUsageTimeoutMillisConfigured == false) {
        /* Infinite */
        mUsageTimeoutMillis = 0;
        mUsageTimeoutMillisConfigured = true;
    }

    return std::make_shared<PluginsConfigurator>(this);
}

/* CONFIGURED PLUGIN ---------------------------------------------------------------------------- */

PluginsConfigurator::ConfiguredPlugin::ConfiguredPlugin(
  std::shared_ptr<Plugin> plugin,
  std::shared_ptr<ReaderConfiguratorSpi> readerConfiguratorSpi,
  std::shared_ptr<PluginObservationExceptionHandlerSpi>  pluginObservationExceptionHandlerSpi,
  std::shared_ptr<CardReaderObservationExceptionHandlerSpi> readerObservationExceptionHandlerSpi)
: mPlugin(plugin), 
  mReaderConfiguratorSpi(readerConfiguratorSpi),
  mWithPluginMonitoring(false),
  mWithReaderMonitoring(false),
  mPluginObservationExceptionHandlerSpi(nullptr),
  mReaderObservationExceptionHandlerSpi(nullptr)
{
    if (pluginObservationExceptionHandlerSpi != nullptr) {
        mWithPluginMonitoring = true;
        mPluginObservationExceptionHandlerSpi = pluginObservationExceptionHandlerSpi;
    }

    if (readerObservationExceptionHandlerSpi != nullptr) {
        mWithReaderMonitoring = true;
        mReaderObservationExceptionHandlerSpi = readerObservationExceptionHandlerSpi;
    }
}

std::shared_ptr<Plugin> PluginsConfigurator::ConfiguredPlugin::getPlugin() const
{
    return mPlugin;
}

std::shared_ptr<ReaderConfiguratorSpi>
    PluginsConfigurator::ConfiguredPlugin::getReaderConfiguratorSpi() const
{
    return mReaderConfiguratorSpi;
}

bool PluginsConfigurator::ConfiguredPlugin::isWithPluginMonitoring() const
{
    return mWithPluginMonitoring;
}

std::shared_ptr<PluginObservationExceptionHandlerSpi>
    PluginsConfigurator::ConfiguredPlugin::getPluginObservationExceptionHandlerSpi() const
{
    return mPluginObservationExceptionHandlerSpi;
}

bool PluginsConfigurator::ConfiguredPlugin::isWithReaderMonitoring() const
{
    return mWithReaderMonitoring;
}

std::shared_ptr<CardReaderObservationExceptionHandlerSpi>
    PluginsConfigurator::ConfiguredPlugin::getReaderObservationExceptionHandlerSpi() const
{
    return mReaderObservationExceptionHandlerSpi;
}

/* PLUGINS CONFIGURATOR ------------------------------------------------------------------------- */

AllocationStrategy PluginsConfigurator::getAllocationStrategy() const
{
    return mAllocationStrategy;
}

int PluginsConfigurator::getUsageTimeoutMillis() const
{
    return mUsageTimeoutMillis;
}

const std::vector<std::shared_ptr<Plugin>>& PluginsConfigurator::getPlugins() const
{
    return mPlugins;
}

const std::vector<std::shared_ptr<ConfiguredPlugin>>& PluginsConfigurator::getConfiguredPlugins()
    const
{
    return mConfiguredPlugins;
}

PluginsConfigurator::Builder* PluginsConfigurator::builder()
{
    return new Builder();
}

PluginsConfigurator::PluginsConfigurator(PluginsConfigurator::Builder* builder)
: mAllocationStrategy(builder->mAllocationStrategy),
  mUsageTimeoutMillis(builder->mUsageTimeoutMillis),
  mPlugins(builder->mPlugins),
  mConfiguredPlugins(builder->mConfiguredPlugins)
{
    /* Deleted builder here. It's been allocated with new */
    delete builder;
}

}
}
}
}
