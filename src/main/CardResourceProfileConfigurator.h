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

/* Keyple Service Resource */
#include "CardResourceProfileExtension.h"
#include "KeypleServiceResourceExport.h"

/* Keyple Core Service */
#include "Plugin.h"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using namespace keyple::core::service;
using namespace keyple::core::service::resource::spi;

/**
 * Configurator of a card resource profile.
 *
 * @since 2.0.0
 */
class KEYPLESERVICERESOURCE_API CardResourceProfileConfigurator final {
public:
    /**
     * Builder of CardResourceProfileConfigurator.
     *
     * @since 2.0.0
     */
    class KEYPLESERVICERESOURCE_API Builder {
    public:
        /**
         *
         */
        friend CardResourceProfileConfigurator;

        /**
         * Restricts the scope of the search during the allocation process to the provided plugins.
         *
         * <p>If this setter is not invoked, all global configured plugins will be used as search
         * domain during the allocation process.
         *
         * <p><u>Note:</u> The order of the plugins is important because it will be kept during the
         * allocation process, but the pool plugins allocation strategy is defined by
         * PoolPluginsConfigurator.
         *
         * @param plugins An ordered list of plugins.
         * @return The current builder instance.
         * @throw IllegalArgumentException If one or more plugin are null or empty.
         * @throw IllegalStateException If one or more plugins are not previously configured
         *        (performed by the method CardResourceServiceConfigurator::configure()).
         * @since 2.0.0
         */
        Builder& withPlugins(const std::vector<std::shared_ptr<Plugin>>& plugins);

        /**
         * Sets a filter targeting all card readers having a name matching the provided regular
         * expression.
         *
         * <p>This filter is useful for readers associated to "regular" plugins only.
         *
         * @param readerNameRegex A regular expression.
         * @return The current builder instance.
         * @throw IllegalArgumentException If the readerNameRegex is null, empty or invalid.
         * @throw IllegalStateException If the filter has already been set.
         * @since 2.0.0
         */
        Builder& withReaderNameRegex(const std::string& readerNameRegex);

        /**
         * Sets a filter to target all card having the provided specific reader group reference.
         *
         * <p>* This filter is useful for readers associated to "pool" plugins only.
         *
         * @param readerGroupReference A reader group reference.
         * @return The current builder instance.
         * @throw IllegalArgumentException If the readerGroupReference is null or empty.
         * @throw IllegalStateException If the filter has already been set.
         * @since 2.0.0
         */
        Builder& withReaderGroupReference(const std::string& readerGroupReference);

        /**
         * Creates a new instance of {@link CardResourceProfileConfigurator} using the current
         * configuration.
         *
         * @return A new instance.
         * @since 2.0.0
         */
        std::shared_ptr<CardResourceProfileConfigurator> build();

    private:
        /**
         *
         */
        std::string mProfileName;

        /**
         *
         */
        std::shared_ptr<CardResourceProfileExtension> mCardResourceProfileExtension;

        /**
         *
         */
        std::vector<std::shared_ptr<Plugin>> mPlugins;

        /**
         *
         */
        std::string mReaderNameRegex;

        /**
         *
         */
        std::string mReaderGroupReference;

        /**
         *
         */
        Builder(const std::string& profileName,
                std::shared_ptr<CardResourceProfileExtension> cardResourceProfileExtension);
    };

    /**
     * (package-private)<br>
     * Gets the name of the profile.
     *
     * @return A not empty string.
     * @since 2.0.0
     */
    const std::string& getProfileName() const;

    /**
     * (package-private)<br>
     * Gets the card resource profile extension.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    std::shared_ptr<CardResourceProfileExtension> getCardResourceProfileExtension() const;

    /**
     * (package-private)<br>
     * Gets the list of plugins configured for the profile.<br>
     * If empty, then global configured plugins must be used.
     *
     * @return A not null collection.
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<Plugin>>& getPlugins() const;

    /**
     * (package-private)<br>
     * Gets the filter on the reader name as a regex value.<br>
     * This filter is useful for readers associated to "regular" plugins only.
     *
     * @return Null if no filter is set.
     * @since 2.0.0
     */
    const std::string& getReaderNameRegex() const;

    /**
     * (package-private)<br>
     * Gets the filter on the reader group reference.<br>
     * This filter is useful for readers associated to "pool" plugins only.
     *
     * @return Null if no filter is set.
     * @since 2.0.0
     */
    const std::string& getReaderGroupReference() const;

    /**
     * Gets the configurator's builder to use in order to create a new instance of a card resource
     * profile with the provided name and a card resource profile extension to handle specific card
     * operations to be performed at allocation time.
     *
     * @param profileName The name of the profile (must be unique).
     * @param cardResourceProfileExtension The associated specific extension able to select a card.
     * @return A not null reference.
     * @throw IllegalArgumentException If the name or the card profile extension is null or empty.
     * @throw IllegalStateException If the name is already in use (performed by the method
     *     CardResourceServiceConfigurator::configure()).
     * @since 2.0.0
     */
    static Builder* builder(
        const std::string& profileName,
        std::shared_ptr<CardResourceProfileExtension> cardResourceProfileExtension);

    /**
     * C++: should be private but prevent the use of make_shared from Builder class.
     */
    CardResourceProfileConfigurator(Builder* builder);

private:
    /**
     *
     */
    const std::string mProfileName;

    /**
     *
     */
    const std::shared_ptr<CardResourceProfileExtension> mCardResourceProfileExtension;

    /**
     *
     */
    const std::vector<std::shared_ptr<Plugin>> mPlugins;

    /**
     *
     */
    const std::string mReaderNameRegex;

    /**
     *
     */
    const std::string mReaderGroupReference;
};

}
}
}
}
