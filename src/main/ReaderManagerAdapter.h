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

#include <memory>

/* Calypsonet Terminal Reader */
#include "CardReader.h"

/* Keyple Core Util */
#include "LoggerFactory.h"

/* Keyple Core Service */
#include "CardResourceProfileExtension.h"
#include "Plugin.h"

/* Keyple Service Resource */
#include "CardResource.h"
#include "ReaderConfiguratorSpi.h"



namespace keyple {
namespace core {
namespace service {
namespace resource {

using namespace calypsonet::terminal::reader;
using namespace keyple::core::service;
using namespace keyple::core::service::resource::spi;
using namespace keyple::core::util::cpp;

/**
 * (package-private)<br>
 * Manager of a reader associated to a "regular" plugin.
 *
 * <p>It contains all associated created card resources and manages concurrent access to the
 * reader's card resources so that only one card resource can be used at a time.
 *
 * @since 2.0.0
 */
class ReaderManagerAdapter final {
public:
    /**
     * (package-private)<br>
     * Creates a new reader manager not active by default.
     *
     * @param reader The associated reader.
     * @param plugin The associated plugin.
     * @param readerConfiguratorSpi The reader configurator to use.
     * @param usageTimeoutMillis The max usage duration of a card resource before it will be
     *        automatically release.
     * @since 2.0.0
     */
    ReaderManagerAdapter(std::shared_ptr<CardReader> reader,
                         std::shared_ptr<Plugin> plugin,
                         std::shared_ptr<ReaderConfiguratorSpi> readerConfiguratorSpi,
                         const int usageTimeoutMillis);

    /**
     * (package-private)<br>
     * Gets the associated reader.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    std::shared_ptr<CardReader> getReader() const;

    /**
     * (package-private)<br>
     * Gets the associated plugin.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    std::shared_ptr<Plugin> getPlugin() const;

    /**
     * (package-private)<br>
     * Gets a view of the current created card resources.
     *
     * @return An empty collection if there's no card resources.
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<CardResource>>& getCardResources() const;

    /**
     * (package-private)<br>
     * Indicates if the associated reader is accepted by at least one card profile manager.
     *
     * @return True if the reader manager is active.
     * @since 2.0.0
     */
    bool isActive() const;

    /**
     * (package-private)<br>
     * Activates the reader manager and setup the reader if needed.
     *
     * @since 2.0.0
     */
    void activate();

    /**
     * (package-private)<br>
     * Gets a new or an existing card resource if the current inserted card matches with the
     * provided card resource profile extension.
     *
     * <p>If the card matches, then updates the current selected card resource.
     *
     * <p>In any case, invoking this method unlocks the reader due to the use of the card selection
     * manager by the extension during the match process.
     *
     * @param extension The card resource profile extension to use for matching.
     * @return Null if the inserted card does not match with the provided profile extension.
     * @since 2.0.0
     */
    std::shared_ptr<CardResource> matches(std::shared_ptr<CardResourceProfileExtension> extension);

    /**
     * (package-private)<br>
     * Tries to lock the provided card resource if the reader is not busy.
     *
     * <p>If the provided card resource is not the current selected one, then tries to select it using
     * the provided card resource profile extension.
     *
     * @param cardResource The card resource to lock.
     * @param extension The card resource profile extension to use in case if a new selection is
     *        needed.
     * @return True if the card resource is locked.
     * @throw IllegalStateException If a new selection has been made and the current card does not
     *        match the provided profile extension or is not the same smart card than the provided
     *        one.
     * @since 2.0.0
     */
    bool lock(std::shared_ptr<CardResource> cardResource,
              std::shared_ptr<CardResourceProfileExtension> extension);

    /**
     * (package-private)<br>
     * Free the reader.
     *
     * @since 2.0.0
     */
    void unlock();

    /**
     * (package-private)<br>
     * Removes the provided card resource.
     *
     * @param cardResource The card resource to remove.
     * @since 2.0.0
     */
    void removeCardResource(std::shared_ptr<CardResource> cardResource);

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger = LoggerFactory::getLogger(typeid(ReaderManagerAdapter));

    /**
     * The associated reader
     */
    std::shared_ptr<CardReader> mReader;

    /**
     * The associated plugin
     */
    std::shared_ptr<Plugin> mPlugin;

    /**
     * Collection of all created card resources
     */
    std::vector<std::shared_ptr<CardResource>> mCardResources;

    /**
     * The reader configurator, not null if the monitoring is activated for the associated reader
     */
    std::shared_ptr<ReaderConfiguratorSpi> mReaderConfiguratorSpi;

    /**
     * The max usage duration of a card resource before it will be automatically release
     */
    int mUsageTimeoutMillis;

    /**
     * Indicates the time after which the reader will be automatically unlocked if a new lock is
     * requested.
     */
    uint64_t mLockMaxTimeMillis;

    /**
     * Current selected card resource
     */
    std::shared_ptr<CardResource> mSelectedCardResource;

    /**
     * Indicates if a card resource is actually in use
     */
    bool mIsBusy;

    /**
     * Indicates if the associated reader is accepted by at least one card profile manager
     */
    bool mIsActive;

    /**
     * (private)<br>
     * Gets an existing card resource having the same smart card than the provided one, or creates a
     * new one if not.
     *
     * @param smartCard The associated smart card.
     * @return A not null reference.
     */
    std::shared_ptr<CardResource> getOrCreateCardResource(std::shared_ptr<SmartCard> smartCard);

    /**
     * (private)<br>
     * Checks if the provided Smart Cards are identical.
     *
     * @param s1 Smart Card 1
     * @param s2 Smart Card 2
     * @return True if they are identical.
     */
    bool areEquals(const std::shared_ptr<SmartCard> s1, const std::shared_ptr<SmartCard> s2) const;
};

}
}
}
}
