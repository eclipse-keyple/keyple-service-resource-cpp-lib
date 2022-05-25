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

#include "CardResourceServiceConfiguratorAdapter.h"

/* Keyple Core Util */
#include "Arrays.h"
#include "IllegalStateException.h"
#include "KeypleAssert.h"

/* Keyple Service Resource */
#include "CardResourceServiceAdapter.h"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using namespace keyple::core::util;
using namespace keyple::core::util::cpp;
using namespace keyple::core::util::cpp::exception;

CardResourceServiceConfiguratorAdapter::CardResourceServiceConfiguratorAdapter()
: mIsBlockingAllocationMode(false) {}

CardResourceServiceConfigurator& CardResourceServiceConfiguratorAdapter::withPlugins(
    std::shared_ptr<PluginsConfigurator> pluginsConfigurator)
{
    Assert::getInstance().notNull(pluginsConfigurator, "pluginsConfigurator");

    /* C++: Java checks against nullity here... */
    if (!mPlugins.empty()) {
        throw IllegalStateException("Plugins already configured.");
    }

    mPlugins = pluginsConfigurator->getPlugins();
    mConfiguredPlugins = pluginsConfigurator->getConfiguredPlugins();
    mAllocationStrategy = pluginsConfigurator->getAllocationStrategy();
    mUsageTimeoutMillis = pluginsConfigurator->getUsageTimeoutMillis();

    return *this;
}

CardResourceServiceConfigurator& CardResourceServiceConfiguratorAdapter::withPoolPlugins(
    std::shared_ptr<PoolPluginsConfigurator> poolPluginsConfigurator)
{
    Assert::getInstance().notNull(poolPluginsConfigurator, "poolPluginsConfigurator");

    /* C++: Java checks against nullity here... */
    if (!mPoolPlugins.empty()) {
        throw IllegalStateException("Pool plugins already configured.");
    }

    mPoolPlugins = poolPluginsConfigurator->getPoolPlugins();
    mUsePoolFirst = poolPluginsConfigurator->isUsePoolFirst();

    return *this;
}

CardResourceServiceConfigurator& CardResourceServiceConfiguratorAdapter::withCardResourceProfiles(
    const std::vector<std::shared_ptr<CardResourceProfileConfigurator>>&
        cardResourceProfileConfigurators)
{
    if (!mCardResourceProfileConfigurators.empty()) {
        throw IllegalStateException("Card resource profiles already configured.");
    }

    for (const auto& configurator : cardResourceProfileConfigurators) {
        Assert::getInstance().notNull(configurator, "cardResourceProfileConfigurator");
        mCardResourceProfileConfigurators.push_back(configurator);
    }

    return *this;
}

CardResourceServiceConfigurator& CardResourceServiceConfiguratorAdapter::withBlockingAllocationMode(
    const int cycleDurationMillis, const int timeoutMillis)
{
    Assert::getInstance().greaterOrEqual(cycleDurationMillis, 1, "cycleDurationMillis")
                         .greaterOrEqual(timeoutMillis, 1, "timeoutMillis");

    if (mIsBlockingAllocationMode) {
        throw IllegalStateException("Allocation mode already configured.");
    }

    mIsBlockingAllocationMode = true;
    mCycleDurationMillis = cycleDurationMillis;
    mTimeoutMillis = timeoutMillis;

    return *this;
}

void CardResourceServiceConfiguratorAdapter::configure()
{
    /*
     * Configure default values
     *
     * C++: can't have null vector, this step is not required (?)
     * FIXME (?)
     */
    // if (plugins == null) {
    // plugins = Collections.emptyList();
    // }
    // if (configuredPlugins == null) {
    // configuredPlugins = Collections.emptyList();
    // }
    // if (poolPlugins == null) {
    // poolPlugins = Collections.emptyList();
    // }

    /* Check global plugins (regular + pool) */
    std::vector<std::shared_ptr<Plugin>> allPlugins;
    Arrays::addAll(allPlugins, mPlugins);
    Arrays::addAll(allPlugins, mPoolPlugins);

    if (allPlugins.empty()) {
        throw IllegalStateException("No plugin configured.");
    }

    /* Check card resource profiles */
    if (mCardResourceProfileConfigurators.empty()) {
        throw IllegalStateException("No card resource profile configured.");
    }

    /* Check card resource profiles names and plugins */
    std::vector<std::string> profileNames;
    for (const auto& profile : mCardResourceProfileConfigurators) {
        /* Check name */
        if (std::find(profileNames.begin(), profileNames.end(), profile->getProfileName()) !=
                profileNames.end()) {
            throw IllegalStateException("Some card resource profiles are configured with the same" \
                                        " profile name.");
        }

        profileNames.push_back(profile->getProfileName());

        /* Check plugins */
        if (!Arrays::containsAll(allPlugins, profile->getPlugins())) {
            throw IllegalStateException("Some card resource profiles specify plugins which are " \
                                        "not configured in the global list.");
        }
    }

    /* Remove plugins not used by a least one card profile */
    const std::vector<std::shared_ptr<Plugin>> usedPlugins = computeUsedPlugins(allPlugins);

    if (usedPlugins.size() != allPlugins.size()) {
        std::vector<std::shared_ptr<Plugin>> unusedPlugins = allPlugins;
        Arrays::removeAll(unusedPlugins, usedPlugins);
        Arrays::removeAll(mPlugins, unusedPlugins);
        Arrays::removeAll(mConfiguredPlugins, getConfiguredPlugins(unusedPlugins));
        Arrays::removeAll(mPoolPlugins, extractPoolPlugins(unusedPlugins));
    }

    /* Apply the configuration */
    CardResourceServiceAdapter::getInstance()->configure(shared_from_this());
}

const std::vector<std::shared_ptr<Plugin>> CardResourceServiceConfiguratorAdapter::computeUsedPlugins(
    const std::vector<std::shared_ptr<Plugin>>& configuredPlugins) const
{
    std::vector<std::shared_ptr<Plugin>> usedPlugins;

    for (const auto& profile : mCardResourceProfileConfigurators) {
        if (!profile->getPlugins().empty()) {
            Arrays::addAll(usedPlugins, profile->getPlugins());
        } else {
            return configuredPlugins;
        }
    }

    return usedPlugins;
}

const std::vector<std::shared_ptr<Plugin>>& CardResourceServiceConfiguratorAdapter::getPlugins()
    const
{
    return mPlugins;
}

const std::vector<std::shared_ptr<ConfiguredPlugin>>&
    CardResourceServiceConfiguratorAdapter::getConfiguredPlugins() const
{
    return mConfiguredPlugins;
}

AllocationStrategy CardResourceServiceConfiguratorAdapter::getAllocationStrategy() const
{
    return mAllocationStrategy;
}

int CardResourceServiceConfiguratorAdapter::getUsageTimeoutMillis() const
{
    return mUsageTimeoutMillis;
}

const std::vector<std::shared_ptr<PoolPlugin>>& CardResourceServiceConfiguratorAdapter::getPoolPlugins()
    const
{
    return mPoolPlugins;
}

bool CardResourceServiceConfiguratorAdapter::isUsePoolFirst() const
{
    return mUsePoolFirst;
}

const std::vector<std::shared_ptr<CardResourceProfileConfigurator>>&
    CardResourceServiceConfiguratorAdapter::getCardResourceProfileConfigurators() const
{
    return mCardResourceProfileConfigurators;
}

bool CardResourceServiceConfiguratorAdapter::isBlockingAllocationMode() const
{
    return mIsBlockingAllocationMode;
}

int CardResourceServiceConfiguratorAdapter::getCycleDurationMillis() const
{
    return mCycleDurationMillis;
}

int CardResourceServiceConfiguratorAdapter::getTimeoutMillis() const
{
    return mTimeoutMillis;
}

const std::vector<std::shared_ptr<PoolPlugin>>
    CardResourceServiceConfiguratorAdapter::extractPoolPlugins(
        const std::vector<std::shared_ptr<Plugin>>& plugins) const
{
    std::vector<std::shared_ptr<PoolPlugin>> results;

    for (const auto& plugin : plugins) {
        const auto pool = std::dynamic_pointer_cast<PoolPlugin>(plugin);
        if (pool != nullptr) {
            results.push_back(pool);
        }
    }

    return results;
}

const std::vector<std::shared_ptr<ConfiguredPlugin>>
    CardResourceServiceConfiguratorAdapter::getConfiguredPlugins(
        const std::vector<std::shared_ptr<Plugin>>& plugins) const
{
    std::vector<std::shared_ptr<ConfiguredPlugin>> results;

    for (const auto& configuredRegularPlugin : mConfiguredPlugins) {
        if (Arrays::contains(plugins, configuredRegularPlugin->getPlugin())) {
            results.push_back(configuredRegularPlugin);
        }
    }

    return results;
}

}
}
}
}
