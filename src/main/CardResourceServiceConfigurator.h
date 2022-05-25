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

#pragma once

#include <memory>

/* Keyple Service Resource */
#include "CardResourceProfileConfigurator.h"
#include "PluginsConfigurator.h"
#include "PoolPluginsConfigurator.h"

namespace keyple {
namespace core {
namespace service {
namespace resource {

/**
 * Configurator of the card resource service.
 *
 * <p>The configuration consists in a sequence of steps including:
 *
 * <ul>
 *   <li>Assignment of plugins to be used with or without automatic refresh.
 *   <li>Selection of strategies and parameters of card allocation.
 *   <li>Creation of card profiles.
 * </ul>
 *
 * @since 2.0.0
 */
class CardResourceServiceConfigurator {
public:
    /**
     * 
     */
    virtual ~CardResourceServiceConfigurator() = default;

    /**
     * Configures the card resource service with one or more {@link Plugin} or ObservablePlugin.
     *
     * @param pluginsConfigurator The plugins configurator to use.
     * @return The current configurator instance.
     * @throw IllegalArgumentException If the provided plugins configurator is null.
     * @throw IllegalStateException If this step has already been performed.
     * @since 2.0.0
     */
    virtual CardResourceServiceConfigurator& withPlugins(
        std::shared_ptr<PluginsConfigurator> pluginsConfigurator) = 0;

    /**
     * Configures the card resource service with one or more {@link PoolPlugin}.
     *
     * @param poolPluginsConfigurator The pool plugins configurator to use.
     * @return The current configurator instance.
     * @throw IllegalArgumentException If the provided pool plugins configurator is null.
     * @throw IllegalStateException If this step has already been performed.
     * @since 2.0.0
     */
    virtual CardResourceServiceConfigurator& withPoolPlugins(
        std::shared_ptr<PoolPluginsConfigurator> poolPluginsConfigurator) = 0;

    /**
     * Configures the card resource service with one or more card resource profiles.
     *
     * @param cardResourceProfileConfigurators The collection of card resources profiles to use.
     * @return The current configurator instance.
     * @throw IllegalArgumentException If the provided configurators are null.
     * @throw IllegalStateException If this step has already been performed.
     * @since 2.0.0
     */
    virtual CardResourceServiceConfigurator& withCardResourceProfiles(
        const std::vector<std::shared_ptr<CardResourceProfileConfigurator>>& 
            cardResourceProfileConfigurators) = 0;

    /**
     * Configures the card resource service to use a blocking allocation mode with the provided
     * timing parameters used during the allocation process.
     *
     * <p>By default, the card resource service is configured with a <b>non-blocking</b> allocation
     * mode.
     *
     * @param cycleDurationMillis The cycle duration (in milliseconds) is the time between two
     *        attempts to find an available card.
     * @param timeoutMillis The timeout (in milliseconds) is the maximum amount of time the
     *        allocation method will attempt to find an available card.
     * @return The current configurator instance.
     * @throw IllegalArgumentException If one of the provided values is less or equal to 0.
     * @throw IllegalStateException If this step has already been performed.
     * @since 2.0.0
     */
    virtual CardResourceServiceConfigurator& withBlockingAllocationMode(
        const int cycleDurationMillis, const int timeoutMillis) = 0;

    /**
     * Finalizes the configuration of the card resource service.
     *
     * <p>If the service is already started, the new configuration is applied immediately.<br>
     * Any previous configuration will be overwritten.
     *
     * <p>If some global configured plugins are not used by any card resource profile, then they are
     * automatically removed from the configuration.
     *
     * @throw IllegalStateException
     *     <ul>
     *       <li>If no "plugin" or "pool plugin" is configured.
     *       <li>If no card resource profile is configured.
     *       <li>If some card resource profiles are configured with the same profile name.
     *       <li>If some card resource profiles specify plugins which are not configured in the
     *           global list.
     *     </ul>
     *
     * @since 2.0.0
     */
    virtual void configure() = 0;
};

}
}
}
}
