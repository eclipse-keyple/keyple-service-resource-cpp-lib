/**************************************************************************************************
 * Copyright (c) 2022 Calypso Networks Association https://calypsonet.org/                        *
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

#include <map>
#include <memory>
#include <mutex>
#include <string>

/* Keyple Service Resource */
#include "CardResource.h"
#include "CardResourceService.h"
#include "CardResourceServiceConfiguratorAdapter.h"
#include "ReaderManagerAdapter.h"

/* Keyple Core Service */
#include "Plugin.h"
#include "PluginObserverSpi.h"
#include "PoolPlugin.h"
#include "Reader.h"

/* Keyple Core Util */
#include "HexUtil.h"
#include "LoggerFactory.h"

/* Calypsonet Terminal Reader */
#include "CardReader.h"
#include "CardReaderObserverSpi.h"
#include "ObservableCardReader.h"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using namespace calypsonet::terminal::reader;
using namespace calypsonet::terminal::reader::spi;
using namespace keyple::core::service;
using namespace keyple::core::service::spi;
using namespace keyple::core::util;
using namespace keyple::core::util::cpp;

using ConfiguredPlugin = PluginsConfigurator::ConfiguredPlugin;

class CardProfileManagerAdapter;

/**
 * (package-private)<br>
 * Implementation of {@link CardResourceService}.
 *
 * @since 2.0.0
 */
class CardResourceServiceAdapter final
: public CardResourceService,
  public PluginObserverSpi,
  public CardReaderObserverSpi,
  public std::enable_shared_from_this<CardResourceServiceAdapter> {
public:
    /**
     * (package-private)<br>
     * Gets the unique instance.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    static std::shared_ptr<CardResourceServiceAdapter> getInstance();

    /**
     * (package-private)<br>
     * Gets a string representation of the provided card resource.
     *
     * @param cardResource The card resource.
     * @return Null if the provided card resource is null.
     * @since 2.0.0
     */
    static const std::string getCardResourceInfo(const std::shared_ptr<CardResource> cardResource);

    /**
     * (package-private)<br>
     * Gets the reader manager associated to the provided reader.
     *
     * @param reader The associated reader.
     * @return Null if there is no reader manager associated.
     * @since 2.0.0
     */
    std::shared_ptr<ReaderManagerAdapter> getReaderManager(const std::shared_ptr<CardReader> reader)
        const;

    /**
     * (package-private)<br>
     * Associates a card resource to a "pool" plugin.
     *
     * @param cardResource The card resource to register.
     * @param poolPlugin The associated pool plugin.
     * @since 2.0.0
     */
    void registerPoolCardResource(std::shared_ptr<CardResource> cardResource,
                                  std::shared_ptr<PoolPlugin> poolPlugin);

    /**
     * (package-private)<br>
     * Configures the card resource service.
     *
     * <p>If service is started, then stops the service, applies the configuration and starts the
     * service.
     *
     * <p>If not, then only applies the configuration.
     *
     * @since 2.0.0
     */
    void configure(std::shared_ptr<CardResourceServiceConfiguratorAdapter> configurator);

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    std::shared_ptr<CardResourceServiceConfigurator> getConfigurator() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void start() override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void stop() override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    std::shared_ptr<CardResource> getCardResource(const std::string& cardResourceProfileName) const
        override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void releaseCardResource(std::shared_ptr<CardResource> cardResource) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void removeCardResource(std::shared_ptr<CardResource> cardResource) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void onPluginEvent(const std::shared_ptr<PluginEvent> pluginEvent) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void onReaderEvent(const std::shared_ptr<CardReaderEvent> readerEvent) override;

private:
    /**
     *
     */
    static std::shared_ptr<CardResourceServiceAdapter> mInstance;

    /**
     *
     */
    const std::unique_ptr<Logger> mLogger =
        LoggerFactory::getLogger(typeid(CardResourceServiceAdapter));

    /**
     * Map an accepted reader of a "regular" plugin to a reader manager
     */
    std::map<std::shared_ptr<CardReader>, std::shared_ptr<ReaderManagerAdapter>>
        mReaderToReaderManagerMap;

    /**
     * Map a configured card profile name to a card profile manager
     */
    std::map<std::string, std::shared_ptr<CardProfileManagerAdapter>>
        mCardProfileNameToCardProfileManagerMap;

    /**
     * Map a card resource to a "pool plugin".<br>
     * A card resource associated to a "pool plugin" is only present in this map for the time of its
     * use and is not referenced by any card profile manager.
     */
    std::map<std::shared_ptr<CardResource>, std::shared_ptr<PoolPlugin>>
        mCardResourceToPoolPluginMap;

    /**
     * Map a "regular" plugin to its accepted observable readers referenced by at least one card
     * profile manager.<br>
     * This map is useful to observe only the accepted readers in case of a card monitoring request.
     */
    std::map<std::shared_ptr<Plugin>, std::vector<std::shared_ptr<ObservableCardReader>>>
        mPluginToObservableReadersMap;

    /**
     * The current configuration
     */
    std::shared_ptr<CardResourceServiceConfiguratorAdapter> mConfigurator;

    /**
     * The current status of the card resource service
     */
    bool mIsStarted;

    /**
     *
     */
    std::mutex mMutex;

    /**
     * (private)<br>
     * Initializes a reader manager for each reader of each configured "regular" plugin.
     */
    void initializeReaderManagers();

    /**
     * (private)<br>
     * Creates and registers a reader manager associated to the provided reader and its associated
     * plugin.<br>
     * If the provided reader is observable, then add it to the map of used observable readers.
     *
     * @param reader The reader to register.
     * @param plugin The associated plugin.
     * @return A not null reference.
     */
    std::shared_ptr<ReaderManagerAdapter> registerReader(std::shared_ptr<CardReader> reader,
                                                         std::shared_ptr<Plugin> plugin);

    /**
     * (private)<br>
     * Creates and registers a card profile manager for each configured card profile and creates all
     * available card resources.
     */
    void initializeCardProfileManagers();

    /**
     * (private)<br>
     * Removes all reader managers whose reader is not accepted by any card profile manager and
     * unregisters their associated readers.
     */
    void removeUnusedReaderManagers();

    /**
     * (private)<br>
     * Removes the registered reader manager associated to the provided reader and stops the
     * observation of the reader if the reader is observable and the observation started.
     *
     * @param reader The reader to unregister.
     * @param plugin The associated plugin.
     */
    void unregisterReader(std::shared_ptr<CardReader> reader, std::shared_ptr<Plugin> plugin);

    /**
     * (private)<br>
     * Starts the observation of observable plugins and/or observable readers if requested.<br>
     * The observation of the readers is performed only for those accepted by at least one card
     * profile manager.
     */
    void startMonitoring();

    /**
     * (private)<br>
     * Stops the observation of all observable plugins and observable readers configured.
     */
    void stopMonitoring();

    /**
     * (private)<br>
     * Gets the reader having the provided name if it is registered.
     *
     * @param readerName The name of the reader.
     * @return Null if the reader is not or no longer registered.
     */
    std::shared_ptr<CardReader> getReader(const std::string& readerName) const;

    /**
     * (private)<br>
     * Invoked when a new reader is connected.<br>
     * Notifies all card profile managers about the new available reader.<br>
     * If the new reader is accepted by at least one card profile manager, then a new reader manager
     * is registered to the service.
     *
     * @param reader The new reader.
     * @param plugin The associated plugin.
     */
    void onReaderConnected(std::shared_ptr<CardReader> reader, std::shared_ptr<Plugin> plugin);

    /**
     * (private)<br>
     * Starts the observation of the provided reader only if it is observable, if the monitoring is
     * requested for the provided plugin and if the reader is accepted by at least one card profile
     * manager.
     *
     * @param reader The reader to observe.
     * @param plugin The associated plugin.
     */
    void startMonitoring(std::shared_ptr<CardReader> reader, std::shared_ptr<Plugin> plugin);

    /**
     * (private)<br>
     * Starts the observation of the "regular" plugin.
     *
     * @param configuredPlugin The associated configuration.
     */
    void startPluginObservation(std::shared_ptr<ConfiguredPlugin> configuredPlugin);

    /**
     * (private)<br>
     * Starts the observation of the reader associated to a "regular" plugin.
     *
     * @param observableReader The observable reader to observe.
     * @param configuredPlugin The associated configuration.
     */
    void startReaderObservation(std::shared_ptr<ObservableCardReader> observableReader,
                                std::shared_ptr<ConfiguredPlugin> configuredPlugin);

    /**
     * (private)<br>
     * Invoked when an accepted reader is no more available because it was disconnected or
     * unregistered.<br>
     * Removes its reader manager and all associated created card resources from all card profile
     * managers.
     *
     * @param reader The disconnected reader.
     * @param plugin The associated plugin.
     */
    void onReaderDisconnected(std::shared_ptr<CardReader> reader, std::shared_ptr<Plugin> plugin);

    /**
     * (private)<br>
     * Invoked when a card is inserted, removed or the associated reader unregistered.<br>
     *
     * @param readerEvent The reader event.
     * @param readerManager The reader manager associated to the reader.
     */
    void onReaderEvent(const std::shared_ptr<CardReaderEvent> readerEvent,
                       std::shared_ptr<ReaderManagerAdapter> readerManager);

    /**
     * (private)<br>
     * Invoked when a card is inserted on a reader.<br>
     * Notifies all card profile managers about the insertion of the card.<br>
     * Each card profile manager interested by the card reader will try to create a card resource.
     *
     * @param readerManager The associated reader manager.
     */
    void onCardInserted(std::shared_ptr<ReaderManagerAdapter> readerManager);

    /**
     * (private)<br>
     * Invoked when a card is removed or the associated reader unregistered.<br>
     * Removes all created card resources associated to the reader.
     *
     * @param readerManager The associated reader manager.
     */
    void onCardRemoved(std::shared_ptr<ReaderManagerAdapter> readerManager);
};

}
}
}
}
