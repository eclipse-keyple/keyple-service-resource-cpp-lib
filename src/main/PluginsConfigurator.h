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
#include <vector>

/* Calypsonet Terminal Reader */
#include "CardReaderObservationExceptionHandlerSpi.h"

/* Keyple Core Service */
#include "Plugin.h"
#include "PluginObservationExceptionHandlerSpi.h"

/* Keyple Service Resource */
#include "KeypleServiceResourceExport.h"
#include "ReaderConfiguratorSpi.h"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using namespace calypsonet::terminal::reader::spi;
using namespace keyple::core::service;
using namespace keyple::core::service::spi;
using namespace keyple::core::service::resource::spi;

/**
 * Configurator of all plugins to associate to the card resource service.
 *
 * @since 2.0.0
 */
class KEYPLESERVICERESOURCE_API PluginsConfigurator final {
public:
    /**
     * Enumeration of all card resource service allocation strategies.
     *
     * @since 2.0.0
     */
    enum class AllocationStrategy {
        /**
         * Configures the card resource service to provide the first available card when a card
         * allocation is made.
         *
         * @since 2.0.0
         */
        FIRST,

        /**
         * Configures the card resource service to provide available cards on a cyclical basis to
         * avoid always providing the same card.
         *
         * @since 2.0.0
         */
        CYCLIC,

        /**
         * Configures the card resource service to provide available cards randomly to avoid always
         * providing the same card.
         *
         * @since 2.0.0
         */
        RANDOM
    };

    /**
     * (package-private)<br>
     * This POJO contains a plugin and the parameters that have been associated with it.
     *
     * @since 2.0.0
     */
    class ConfiguredPlugin {
    public:
        /**
         * (package-private)<br>
         * Constructor.
         *
         * @param plugin The plugin.
         * @param readerConfiguratorSpi The reader configurator to use.
         * @param pluginObservationExceptionHandlerSpi The plugin exception handler to use.
         * @param readerObservationExceptionHandlerSpi The reader exception handler to use.
         * @since 2.0.0
         */
        ConfiguredPlugin(
            std::shared_ptr<Plugin> plugin,
            std::shared_ptr<ReaderConfiguratorSpi> readerConfiguratorSpi,
            std::shared_ptr<PluginObservationExceptionHandlerSpi>
                pluginObservationExceptionHandlerSpi,
            std::shared_ptr<CardReaderObservationExceptionHandlerSpi>
                readerObservationExceptionHandlerSpi);

        /**
         * (package-private)<br>
         *
         * @return A not null {@link Plugin} reference.
         * @since 2.0.0
         */
        std::shared_ptr<Plugin> getPlugin() const;

        /**
         * (package-private)<br>
         *
         * @return A not null {@link ReaderConfiguratorSpi} reference if reader monitoring is
         *         requested.
         * @since 2.0.0
         */
        std::shared_ptr<ReaderConfiguratorSpi> getReaderConfiguratorSpi() const;

        /**
         * (package-private)<br>
         *
         * @return True if the reader monitoring is required.
         * @since 2.0.0
         */
        bool isWithPluginMonitoring() const;

        /**
         * (package-private)<br>
         *
         * @return A not null {@link PluginObservationExceptionHandlerSpi} reference if reader
         *         monitoring is requested.
         * @since 2.0.0
         */
        std::shared_ptr<PluginObservationExceptionHandlerSpi>
             getPluginObservationExceptionHandlerSpi() const;

        /**
         * (package-private)<br>
         *
         * @return True if the card monitoring is required.
         * @since 2.0.0
         */
        bool isWithReaderMonitoring() const;

        /**
         * (package-private)<br>
         *
         * @return A not null CardReaderObservationExceptionHandlerSpi reference if card
         *         monitoring is requested.
         * @since 2.0.0
         */
        std::shared_ptr<CardReaderObservationExceptionHandlerSpi>
            getReaderObservationExceptionHandlerSpi() const;

    private:
        /**
         *
         */
        std::shared_ptr<Plugin> mPlugin;

        /**
         *
         */
        std::shared_ptr<ReaderConfiguratorSpi> mReaderConfiguratorSpi;

        /**
         *
         */
        bool mWithPluginMonitoring;

        /**
         *
         */
        std::shared_ptr<PluginObservationExceptionHandlerSpi> mPluginObservationExceptionHandlerSpi;

        /**
         *
         */
        bool mWithReaderMonitoring;

        /**
         *
         */
        std::shared_ptr<CardReaderObservationExceptionHandlerSpi>
            mReaderObservationExceptionHandlerSpi;
    };

    /**
     * Builder of PluginsConfigurator.
     *
     * @since 2.0.0
     */
    class KEYPLESERVICERESOURCE_API Builder {
    public:
        /**
         *
         */
        friend class PluginsConfigurator;

        /**
         * Specifies the allocation strategy to perform when a card resource is requested.
         *
         * <p>Default value: AllocationStrategy::FIRST
         *
         * @param allocationStrategy The AllocationStrategy to use.
         * @return The current builder instance.
         * @throw IllegalArgumentException If the provided strategy is null.
         * @throw IllegalStateException If the strategy has already been configured.
         * @since 2.0.0
         */
        Builder& withAllocationStrategy(const AllocationStrategy allocationStrategy);

        /**
         * Specifies the timeout to use after that an allocated card resource can be automatically
         * reallocated by card resource service to a new thread if requested.
         *
         * <p>Default value: infinite
         *
         * @param usageTimeoutMillis The max usage duration of a card resource (in milliseconds).
         * @return The current builder instance.
         * @throw IllegalArgumentException If the provided value is less or equal to 0.
         * @throw IllegalStateException If the timeout has already been configured.
         * @since 2.0.0
         */
        Builder& withUsageTimeout(const int usageTimeoutMillis);

        /**
         * Adds a {@link Plugin} or {@link ObservablePlugin} to the default list of all card
         * profiles.
         *
         * <p><u>Note:</u> The order of the plugins is important because it will be kept during the
         * allocation process unless redefined by card profiles.
         *
         * @param plugin The plugin to add.
         * @param readerConfiguratorSpi The reader configurator to use when a reader is connected
         *        and accepted by at leas one card resource profile.
         * @return The current builder instance.
         * @throws IllegalArgumentException If the provided plugin or reader configurator is null or
         *         if the plugin is not an instance of Plugin or ObservablePlugin.
         * @throw IllegalStateException If the plugin has already been configured.
         * @since 2.0.0
         */
        Builder& addPlugin(std::shared_ptr<Plugin> plugin,
                           std::shared_ptr<ReaderConfiguratorSpi> readerConfiguratorSpi);

        /**
         * Adds a {@link Plugin} or {@link ObservablePlugin} to the default list of all card
         * profiles with background auto monitoring of reader connections/disconnections and/or card
         * insertions/removals.
         *
         * <p><u>Note:</u> The order of the plugins is important because it will be kept during the
         * allocation process unless redefined by card profiles.
         *
         * <p>The plugin or readers must be observable for the monitoring operations to have an
         * effect.
         *
         * @param plugin The plugin to add.
         * @param readerConfiguratorSpi The reader configurator to use when a reader is connected
         *        and accepted by at leas one card resource profile.
         * @param pluginObservationExceptionHandlerSpi If not null, then activates the monitoring of
         *     the plugin and specifies the exception handler to use in case of error occurs during
         *     the asynchronous observation process.
         * @param readerObservationExceptionHandlerSpi If not null, then activates the monitoring of
         *     the readers and specifies the exception handler to use in case of error occurs during
         *     the asynchronous observation process.
         * @return The current builder instance.
         * @throw IllegalArgumentException If the provided plugin or reader configurator is null or
         *        if the plugin is not an instance of Plugin or ObservablePlugin.
         * @throw IllegalStateException If the plugin has already been configured.
         * @since 2.0.0
         */
        Builder& addPluginWithMonitoring(
            std::shared_ptr<Plugin> plugin,
            std::shared_ptr<ReaderConfiguratorSpi> readerConfiguratorSpi,
            std::shared_ptr<PluginObservationExceptionHandlerSpi>
                pluginObservationExceptionHandlerSpi,
            std::shared_ptr<CardReaderObservationExceptionHandlerSpi>
                readerObservationExceptionHandlerSpi);

        /**
         * Creates a new instance of {@link PluginsConfigurator} using the current configuration.
         *
         * @return A new instance.
         * @throw IllegalStateException If no plugin has been configured.
         * @since 2.0.0
         */
        std::shared_ptr<PluginsConfigurator> build();

    private:
        /**
         *
         */
        AllocationStrategy mAllocationStrategy;

        /**
         * C++: addon
         */
        bool mAllocationStrategyConfigured;

        /**
         *
         */
        int mUsageTimeoutMillis;

        /**
         *
         */
        bool mUsageTimeoutMillisConfigured;

        /**
         *
         */
        std::vector<std::shared_ptr<Plugin>> mPlugins;

        /**
         *
         */
        std::vector<std::shared_ptr<ConfiguredPlugin>> mConfiguredPlugins;

        /**
         *
         */
        Builder();
    };

    /**
     * (package-private)<br>
     * Gets the selected card resource allocation strategy.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    AllocationStrategy getAllocationStrategy() const;

    /**
     * (package-private)<br>
     * Gets the configured usage timeout.
     *
     * @return 0 if no timeout is set.
     * @since 2.0.0
     */
    int getUsageTimeoutMillis() const;

    /**
     * (package-private)<br>
     * Gets the list of all configured "regular" plugins.
     *
     * @return A not empty list.
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<Plugin>>& getPlugins() const;

    /**
     * (package-private)<br>
     * Gets the list of all configured "regular" plugins with their associated configuration.
     *
     * @return A not empty collection.
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<ConfiguredPlugin>>& getConfiguredPlugins() const;

    /**
     * Gets the configurator's builder to use in order to create a new instance.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    static Builder* builder();

    /**
     * C++: should be private but prevent the use of make_shared from Builder class.
     */
    PluginsConfigurator(Builder* builder);

private:
    /**
     *
     */
    const AllocationStrategy mAllocationStrategy;

    /**
     *
     */
    const int mUsageTimeoutMillis;

    /**
     *
     */
    const std::vector<std::shared_ptr<Plugin>> mPlugins;

    /**
     *
     */
    const std::vector<std::shared_ptr<ConfiguredPlugin>> mConfiguredPlugins;
};

}
}
}
}
