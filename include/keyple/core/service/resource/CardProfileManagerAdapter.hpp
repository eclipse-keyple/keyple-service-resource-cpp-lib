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

#include "keyple/core/service/resource/CardResourceAdapter.hpp"
#include "keyple/core/service/resource/CardResourceProfileConfigurator.hpp"
#include "keyple/core/service/resource/CardResourceServiceAdapter.hpp"
#include "keyple/core/service/resource/CardResourceServiceConfiguratorAdapter.hpp"
#include "keyple/core/service/resource/ReaderManagerAdapter.hpp"
#include "keyple/core/util/cpp/Logger.hpp"
#include "keyple/core/util/cpp/LoggerFactory.hpp"
#include "keyple/core/util/cpp/Pattern.hpp"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using keyple::core::util::cpp::Logger;
using keyple::core::util::cpp::LoggerFactory;
using keyple::core::util::cpp::Pattern;

/**
 * Manager of a card profile.
 *
 * <p>It contains the profile configuration and associated card resources.
 *
 * @since 2.0.0
 */
class KEYPLESERVICERESOURCE_API CardProfileManagerAdapter final {
public:
    /**
     * Creates a new card profile manager using the provided card profile and
     * initializes all available card resources.
     *
     * @param cardProfile The associated card profile.
     * @param globalConfiguration The global configuration of the service.
     * @since 2.0.0
     */
    CardProfileManagerAdapter(
        std::shared_ptr<CardResourceProfileConfigurator> cardProfile,
        std::shared_ptr<CardResourceServiceConfiguratorAdapter> 
            globalConfiguration);

    /**
     * Removes the provided card resource from the profile manager if it is
     * present.
     *
     * @param cardResource The card resource to remove.
     * @since 2.0.0
     */
    void 
    removeCardResource(std::shared_ptr<CardResource> cardResource);

    /**
     * Invoked when a new reader is connected.<br>
     * If the associated plugin is referenced on the card profile, then tries to
     * initialize a card resource if the reader is accepted.
     *
     * @param readerManager The reader manager to use.
     * @since 2.0.0
     */
    void 
    onReaderConnected(std::shared_ptr<ReaderManagerAdapter> readerManager);

    /**
     * Invoked when a new card is inserted.<br>
     * The behaviour is the same as if a reader was connected.
     *
     * @param readerManager The reader manager to use.
     * @since 2.0.0
     */
    void 
    onCardInserted(std::shared_ptr<ReaderManagerAdapter> readerManager);

    /**
     * Tries to get a card resource and locks the associated reader.<br>
     * Applies the configured allocation strategy by looping, pausing, ordering
     * resources.
     *
     * @return Null if there is no card resource available.
     * @since 2.0.0
     */
    std::shared_ptr<CardResource> 
    getCardResource();

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger =
        LoggerFactory::getLogger(typeid(CardProfileManagerAdapter));

    /**
     * The associated card profile
     */
    std::shared_ptr<CardResourceProfileConfigurator> mCardProfile;

    /**
     * The global configuration of the card resource service
     */
    std::shared_ptr<CardResourceServiceConfiguratorAdapter> 
        mGlobalConfiguration;

    /**
     * The unique instance of the card resource service
     */
    std::shared_ptr<CardResourceServiceAdapter> mService;

    /**
     * The ordered list of "regular" plugins to use
     */
    std::vector<std::shared_ptr<Plugin>> mPlugins;

    /**
     * The ordered list of "pool" plugins to use
     */
    std::vector<std::shared_ptr<PoolPlugin>> mPoolPlugins;

    /**
     * The current available card resources associated with "regular" plugins
     */
    std::vector<std::shared_ptr<CardResourceAdapter>> mCardResources;

    /**
     * The filter on the reader name if set
     */
    std::shared_ptr<Pattern> mReaderNameRegexPattern;

    /**
     * Initializes card resources using the plugins configured on the card
     * profile.
     */
    void
    initializeCardResourcesUsingProfilePlugins();

    /**
     * Initializes card resources using the plugins configured on the card
     * resource service.
     */
    void
    initializeCardResourcesUsingDefaultPlugins();

    /**
     * Initializes all available card resources by analysing all readers of the
     * provided "regular" plugin.
     *
     * @param plugin The "regular" plugin to analyse.
     */
    void
    initializeCardResources(std::shared_ptr<Plugin> plugin);

    /**
     * Tries to initialize a card resource for the provided reader manager only
     * if the reader is accepted by the profile.
     *
     * <p>If the reader is accepted, then activates the provided reader manager
     * if it is not already activated.
     *
     * @param readerManager The reader manager to use.
     */
    void
    initializeCardResource(std::shared_ptr<ReaderManagerAdapter> readerManager);

    /**
     * Checks if the provided reader is accepted using the filter on the name.
     *
     * @param reader The reader to check.
     * @return True if it is accepted.
     */
    bool
    isReaderAccepted(std::shared_ptr<CardReader> reader);

    /**
     * Make a pause if the provided card resource is null and a blocking
     * allocation mode is requested.
     *
     * @param cardResource The founded card resource or null if not found.
     */
    void
    pauseIfNeeded(std::shared_ptr<CardResource> cardResource);

    /**
     * Tries to get a card resource searching in "regular" and "pool" plugins.
     *
     * @return Null if there is no card resource available.
     */
    std::shared_ptr<CardResource>
    getRegularOrPoolCardResource();

    /**
     * Tries to get a card resource searching in all "regular" plugins.
     *
     * <p>If a card resource is no more usable, then removes it from the
     * service.
     *
     * @return Null if there is no card resource available.
     */
    std::shared_ptr<CardResource>
    getRegularCardResource();

    /**
     * Updates the order of the created card resources according to the
     * configured strategy.
     *
     * @param cardResourceIndex The current card resource index of the
     *        available card resource founded.
     */
    void
    updateCardResourcesOrder(const int cardResourceIndex);

    /**
     * Tries to get a card resource searching in all "pool" plugins.
     *
     * @return Null if there is no card resource available.
     */
    std::shared_ptr<CardResource>
    getPoolCardResource();

    /**
     * 
     */
    static void
    releaseReaderSilently(
        std::shared_ptr<PoolPlugin> poolPlugin, 
        std::shared_ptr<CardReader> reader);
};

} /* namespace resource */
} /* namespace service */
} /* namespace core */
} /* namespace keyple */
