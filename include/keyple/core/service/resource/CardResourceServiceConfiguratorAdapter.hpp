/******************************************************************************
 * Copyright (c) 2025 Calypso Networks Association https://calypsonet.org/    *
 *                                                                            *
 * See the NOTICE file(s) distributed with this work for additional           *
 * information regarding copyright ownership.                                 *
 *                                                                            *
 * This program and the accompanying materials are made available under the   *
 * terms of the Eclipse Public License 2.0 which is available at              *
 * http://www.eclipse.org/legal/epl-2.0                                       *
 *                                                                            *
 * SPDX-License-Identifier: EPL-2.0                                           *
 ******************************************************************************/

#pragma once

#include <memory>
#include <vector>

/* Keyple Core Service */
#include "keyple/core/service/Plugin.hpp"
#include "keyple/core/service/PoolPlugin.hpp"
#include "keyple/core/service/resource/CardResourceProfileConfigurator.hpp"
#include "keyple/core/service/resource/CardResourceServiceConfigurator.hpp"
#include "keyple/core/service/resource/PluginsConfigurator.hpp"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using keyple::core::service::Plugin;
using keyple::core::service::PoolPlugin;

using AllocationStrategy = PluginsConfigurator::AllocationStrategy;
using ConfiguredPlugin = PluginsConfigurator::ConfiguredPlugin;

/**
 * Implementation of CardResourceServiceConfigurator.
 *
 * @since 2.0.0
 */
class CardResourceServiceConfiguratorAdapter final
: public CardResourceServiceConfigurator,
  public std::enable_shared_from_this<CardResourceServiceConfiguratorAdapter> {
public:
    /**
     * Constructor.
     *
     * @since 2.0.0
     */
    CardResourceServiceConfiguratorAdapter();

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    CardResourceServiceConfigurator&
    withPlugins(std::shared_ptr<PluginsConfigurator> pluginsConfigurator) 
        override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    CardResourceServiceConfigurator&
    withPoolPlugins(
        std::shared_ptr<PoolPluginsConfigurator> poolPluginsConfigurator)
        override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    CardResourceServiceConfigurator&
    withCardResourceProfiles(
        const std::vector<std::shared_ptr<CardResourceProfileConfigurator>>&
            cardResourceProfileConfigurators) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    CardResourceServiceConfigurator& 
    withBlockingAllocationMode(
        const int cycleDurationMillis, const int timeoutMillis) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void 
    configure() override;

    /**
     * Computes the collection of the plugins used by at least one card profile.
     *
     * @return A not null collection.
     */
    const std::vector<std::shared_ptr<Plugin>>
    computeUsedPlugins(
        const std::vector<std::shared_ptr<Plugin>>& configuredPlugins) const;

    /**
     *
     * @return A not null list.
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<Plugin>>& 
    getPlugins() const;

    /**
     *
     * @return A not null list.
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<ConfiguredPlugin>>&
    getConfiguredPlugins() const;

    /**
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    AllocationStrategy 
    getAllocationStrategy() const;

    /**
     *
     * @return 0 if no usage timeout is set.
     * @since 2.0.0
     */
    int 
    getUsageTimeoutMillis() const;

    /**
     *
     * @return A not null list.
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<PoolPlugin>>& 
    getPoolPlugins() const;

    /**
     *
     * @return True if pool plugins are prior to regular plugins.
     * @since 2.0.0
     */
    bool 
    isUsePoolFirst() const;

    /**
     * Gets the configurations of all configured card resource profiles.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<CardResourceProfileConfigurator>>&
    getCardResourceProfileConfigurators() const;

    /**
     *
     * @return A not null boolean.
     * @since 2.0.0
     */
    bool 
    isBlockingAllocationMode() const;

    /**
     *
     * @return A positive int.
     * @since 2.0.0
     */
    int 
    getCycleDurationMillis() const;

    /**
     *
     * @return A positive int.
     * @since 2.0.0
     */
    int 
    getTimeoutMillis() const;

private:
    /**
     * Regular plugins
     */
    std::vector<std::shared_ptr<Plugin>> mPlugins;

    /**
     *
     */
    std::vector<std::shared_ptr<ConfiguredPlugin>> mConfiguredPlugins;

    /**
     *
     */
    AllocationStrategy mAllocationStrategy;

    /**
     *
     */
    int mUsageTimeoutMillis;

    /**
     * Pool plugins
     */
    std::vector<std::shared_ptr<PoolPlugin>> mPoolPlugins;

    /**
     *
     */
    bool mUsePoolFirst;

    /**
     * Card resource profiles configurators
     */
    std::vector<std::shared_ptr<CardResourceProfileConfigurator>>
        mCardResourceProfileConfigurators;

    /**
     * Global
     */
    bool mIsBlockingAllocationMode;

    /**
     *
     */
    int mCycleDurationMillis;

    /**
     *
     */
    int mTimeoutMillis;

    /**
     * Extracts all PoolPlugin from a collection of Plugin.
     *
     * @param plugins The origin collection.
     * @return A not null list
     */
    static const std::vector<std::shared_ptr<PoolPlugin>> 
    extractPoolPlugins(
        const std::vector<std::shared_ptr<Plugin>>& plugins);

    /**
     * Gets all ConfiguredPlugin associated to a plugin contained in the
     * provided collection.
     *
     * @param plugins The reference collection.
     * @return A not null collection.
     */
    const std::vector<std::shared_ptr<ConfiguredPlugin>> 
    getConfiguredPlugins(const std::vector<std::shared_ptr<Plugin>>& plugins) 
        const;
};

} /* namespace resource */
} /* namespace service */
} /* namespace core */
} /* namespace keyple */
