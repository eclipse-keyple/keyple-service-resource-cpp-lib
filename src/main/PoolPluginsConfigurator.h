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

/* Keyple Core Service */
#include "PoolPlugin.h"

/* Keyple Service Resource */
#include "KeypleServiceResourceExport.h"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using namespace keyple::core::service;

/**
 * Configurator of all pool plugins to associate to the card resource service.
 *
 * @since 2.0.0
 */
class KEYPLESERVICERESOURCE_API PoolPluginsConfigurator final {
public:
    /**
     * Builder of PoolPluginsConfigurator.
     *
     * @since 2.0.0
     */
    class KEYPLESERVICERESOURCE_API Builder {
    public:
        /**
         * 
         */
        friend class PoolPluginsConfigurator;
        
        /**
         * Configures the card resource service to search for available cards in pool plugins before
         * regular plugins.
         *
         * <p>Default value: pool last
         *
         * @return The current builder instance.
         * @throw IllegalStateException If the setting has already been configured.
         * @since 2.0.0
         */
        Builder& usePoolFirst();

        /**
         * Adds a PoolPlugin to the default list of all card profiles.
         *
         * <p><u>Note:</u> The order of the plugins is important because it will be kept during the
         * allocation process unless redefined by card profiles.
         *
         * @param poolPlugin The pool plugin to add.
         * @return The current builder instance.
         * @throw IllegalArgumentException If the provided pool plugin is null.
         * @throw IllegalStateException If the pool plugin has already been configured.
         * @since 2.0.0
         */
        Builder& addPoolPlugin(std::shared_ptr<PoolPlugin> poolPlugin);

        /**
         * Creates a new instance of {@link PoolPluginsConfigurator} using the current configuration.
         *
         * @return A new instance.
         * @throws IllegalStateException If no pool plugin has been configured.
         * @since 2.0.0
         */
        std::shared_ptr<PoolPluginsConfigurator> build();

    private:
        /**
         * 
         */
        bool mUsePoolFirst;

        /**
         * C++ addon
         */
        bool mUsePoolFirstConfigured;
        
        /**
         * 
         */
        std::vector<std::shared_ptr<PoolPlugin>> mPoolPlugins;

        /**
         * 
         */
        Builder();
    };

    /**
     * (package-private)<br>
     *
     * @return True if pool plugins must be used prior to "regular" plugins.
     * @since 2.0.0
     */
    bool isUsePoolFirst() const;

    /**
     * (package-private)<br>
     * Gets the list of all configured "pool" plugins.
     *
     * @return A not empty list.
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<PoolPlugin>>& getPoolPlugins() const;

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
    PoolPluginsConfigurator(const Builder* builder);

private:
    /**
     * 
     */
    const bool mUsePoolFirst;

    /**
     * 
     */
    const std::vector<std::shared_ptr<PoolPlugin>> mPoolPlugins;
};

}
}
}
}
