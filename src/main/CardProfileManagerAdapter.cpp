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

#include "keyple/core/service/resource/CardProfileManagerAdapter.hpp"

#include <algorithm>
#include <random>

#include "keyple/core/service/Plugin.hpp"
#include "keyple/core/service/SmartCardServiceProvider.hpp"
#include "keyple/core/service/resource/CardResourceServiceAdapter.hpp"
#include "keyple/core/util/cpp/Arrays.hpp"
#include "keyple/core/util/cpp/System.hpp"
#include "keyple/core/util/cpp/Thread.hpp"
#include "keyple/core/util/cpp/exception/IllegalStateException.hpp"
#include "keyple/core/util/cpp/exception/InterruptedException.hpp"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using keyple::core::service::Plugin;
using keyple::core::service::SmartCardServiceProvider;
using keyple::core::service::resource::CardResourceServiceAdapter;
using keyple::core::util::cpp::Arrays;
using keyple::core::util::cpp::System;
using keyple::core::util::cpp::Thread;
using keyple::core::util::cpp::exception::IllegalStateException;
using keyple::core::util::cpp::exception::InterruptedException;

using AllocationStrategy = PluginsConfigurator::AllocationStrategy;

CardProfileManagerAdapter::CardProfileManagerAdapter(
    std::shared_ptr<CardResourceProfileConfigurator> cardProfile,
    std::shared_ptr<CardResourceServiceConfiguratorAdapter> globalConfiguration)
: mCardProfile(cardProfile)
, mGlobalConfiguration(globalConfiguration)
, mService(CardResourceServiceAdapter::getInstance())
{
    /* Prepare filter on reader name if requested */
    if (cardProfile->getReaderNameRegex() != "") {
        mReaderNameRegexPattern
            = Pattern::compile(cardProfile->getReaderNameRegex());
    } else {
        mReaderNameRegexPattern = nullptr;
    }

    /* Initialize all available card resources */
    if (cardProfile->getPlugins().empty()) {
        initializeCardResourcesUsingDefaultPlugins();
    } else {
        initializeCardResourcesUsingProfilePlugins();
    }
}

void
CardProfileManagerAdapter::removeCardResource(
    std::shared_ptr<CardResource> cardResource)
{
    const auto it
        = std::find(mCardResources.begin(), mCardResources.end(), cardResource);

    if (it != mCardResources.end()) {
        mCardResources.erase(it);
        mLogger->debug(
            "Remove % from profile [%]\n",
            CardResourceServiceAdapter::getCardResourceInfo(cardResource),
            mCardProfile->getProfileName());
    }
}

void
CardProfileManagerAdapter::onReaderConnected(
    std::shared_ptr<ReaderManagerAdapter> readerManager)
{
    if (mCardProfile->getPlugins().empty()) {
        initializeCardResource(readerManager);
    } else {
        const auto& profilePlugins = mCardProfile->getPlugins();
        const auto it = std::find_if(
            profilePlugins.begin(),
            profilePlugins.end(),
            [readerManager](const std::shared_ptr<Plugin>& profilePlugin) {
                return (profilePlugin == readerManager->getPlugin());
            });

        if (it != profilePlugins.end()) {
            initializeCardResource(readerManager);
            return;
        }
    }
}

void
CardProfileManagerAdapter::onCardInserted(
    std::shared_ptr<ReaderManagerAdapter> readerManager)
{
    onReaderConnected(readerManager);
}

std::shared_ptr<CardResource>
CardProfileManagerAdapter::getCardResource()
{
    std::shared_ptr<CardResource> cardResource = nullptr;
    uint64_t maxTime = System::currentTimeMillis()
                       + mGlobalConfiguration->getTimeoutMillis();

    do {
        if (mPlugins.empty()) {
            cardResource = getPoolCardResource();
        } else {
            if (mPoolPlugins.empty()) {
                cardResource = getRegularCardResource();
            } else {
                cardResource = getRegularOrPoolCardResource();
            }
        }

        pauseIfNeeded(cardResource);

    } while (cardResource == nullptr
             && mGlobalConfiguration->isBlockingAllocationMode()
             && System::currentTimeMillis() <= maxTime);

    return cardResource;
}

void
CardProfileManagerAdapter::initializeCardResourcesUsingProfilePlugins()
{
    for (const auto& plugin : mCardProfile->getPlugins()) {
        const auto poolPlugin = std::dynamic_pointer_cast<PoolPlugin>(plugin);
        if (poolPlugin) {
            mPoolPlugins.push_back(poolPlugin);
        } else {
            mPlugins.push_back(plugin);
            initializeCardResources(plugin);
        }
    }
}

void
CardProfileManagerAdapter::initializeCardResourcesUsingDefaultPlugins()
{
    const auto& poolPlugins = mGlobalConfiguration->getPoolPlugins();
    mPoolPlugins.insert(
        std::end(mPoolPlugins), std::begin(poolPlugins), std::end(poolPlugins));

    for (const auto& plugin : mGlobalConfiguration->getPlugins()) {
        mPlugins.push_back(plugin);
        initializeCardResources(plugin);
    }
}

void
CardProfileManagerAdapter::initializeCardResources(
    std::shared_ptr<Plugin> plugin)
{
    for (const auto& reader : plugin->getReaders()) {
        std::shared_ptr<ReaderManagerAdapter> readerManager
            = mService->getReaderManager(reader);
        initializeCardResource(readerManager);
    }
}

void
CardProfileManagerAdapter::initializeCardResource(
    std::shared_ptr<ReaderManagerAdapter> readerManager)
{
    if (isReaderAccepted(readerManager->getReader())) {
        readerManager->activate();

        std::shared_ptr<CardResourceAdapter> cardResource
            = readerManager->matches(
                mCardProfile->getCardResourceProfileExtension());

        /*
         * The returned card resource may already be present in the current list
         * if the service starts with an observable reader in which a card has
         * been inserted.
         */
        if (cardResource != nullptr) {
            if (!Arrays::contains(mCardResources, cardResource)) {
                mCardResources.push_back(cardResource);
                mLogger->info(
                    "Add % to profile [%]\n",
                    CardResourceServiceAdapter::getCardResourceInfo(
                        cardResource),
                    mCardProfile->getProfileName());
            } else {
                mLogger->info(
                    "% already present in profile [%]\n",
                    CardResourceServiceAdapter::getCardResourceInfo(
                        cardResource),
                    mCardProfile->getProfileName());
            }
        }
    }
}

bool
CardProfileManagerAdapter::isReaderAccepted(std::shared_ptr<CardReader> reader)
{
    return mReaderNameRegexPattern == nullptr
           || mReaderNameRegexPattern->matcher(reader->getName())->matches();
}

void
CardProfileManagerAdapter::pauseIfNeeded(
    std::shared_ptr<CardResource> cardResource)
{
    if (cardResource == nullptr
        && mGlobalConfiguration->isBlockingAllocationMode()) {
        try {
            Thread::sleep(mGlobalConfiguration->getCycleDurationMillis());
        } catch (const InterruptedException& e) {
            mLogger->error("Unexpected sleep interruption", e);
            // FIXME: Thread::currentThread().interrupt();
        }
    }
}

std::shared_ptr<CardResource>
CardProfileManagerAdapter::getRegularOrPoolCardResource()
{
    std::shared_ptr<CardResource> cardResource = nullptr;

    if (mGlobalConfiguration->isUsePoolFirst()) {
        cardResource = getPoolCardResource();
        if (cardResource == nullptr) {
            cardResource = getRegularCardResource();
        }
    } else {
        cardResource = getRegularCardResource();
        if (cardResource == nullptr) {
            cardResource = getPoolCardResource();
        }
    }

    return cardResource;
}

std::shared_ptr<CardResource>
CardProfileManagerAdapter::getRegularCardResource()
{
    std::shared_ptr<CardResource> result = nullptr;
    std::vector<std::shared_ptr<CardResource>> unusableCardResources;

    for (const auto& cardResource : mCardResources) {
        std::shared_ptr<CardReader> reader = cardResource->getReader();
        // FIXME synchronized (reader) {
        std::shared_ptr<ReaderManagerAdapter> readerManager
            = mService->getReaderManager(reader);
        if (readerManager != nullptr) {
            try {
                if (readerManager->lock(
                        cardResource,
                        mCardProfile->getCardResourceProfileExtension())) {
                    int cardResourceIndex
                        = Arrays::indexOf(mCardResources, cardResource);
                    updateCardResourcesOrder(cardResourceIndex);
                    result = cardResource;
                    break;
                }
            } catch (const IllegalStateException& e) {
                (void)e;
                unusableCardResources.push_back(cardResource);
            }
        } else {
            unusableCardResources.push_back(cardResource);
        }
        //}
    }

    /* Remove unusable card resources identified */
    for (const auto& cardResource : unusableCardResources) {
        mService->removeCardResource(cardResource);
    }

    return result;
}

void
CardProfileManagerAdapter::updateCardResourcesOrder(const int cardResourceIndex)
{
    if (mGlobalConfiguration->getAllocationStrategy()
        == AllocationStrategy::CYCLIC) {
        std::rotate(
            mCardResources.begin(),
            mCardResources.begin() - cardResourceIndex - 1,
            mCardResources.end());

    } else if (
        mGlobalConfiguration->getAllocationStrategy()
        == AllocationStrategy::RANDOM) {
        auto rng = std::default_random_engine {};
        std::shuffle(std::begin(mCardResources), std::end(mCardResources), rng);
    }
}

std::shared_ptr<CardResource>
CardProfileManagerAdapter::getPoolCardResource()
{
    std::shared_ptr<CardResourceProfileExtension> cardProfileExtension
        = mCardProfile->getCardResourceProfileExtension();

    for (const std::shared_ptr<PoolPlugin>& poolPlugin : mPoolPlugins) {
        try {
            std::shared_ptr<CardReader> reader = poolPlugin->allocateReader(
                mCardProfile->getReaderGroupReference());

            if (reader != nullptr) {
                std::shared_ptr<SmartCard> selectedSmartCard
                    = poolPlugin->getSelectedSmartCard(reader);

                std::shared_ptr<SmartCard> smartCard
                    = selectedSmartCard != nullptr
                          ? cardProfileExtension->matches(selectedSmartCard)
                          : cardProfileExtension->matches(
                              reader,
                              SmartCardServiceProvider::getService()
                                  ->getReaderApiFactory());

                if (smartCard != nullptr) {
                    std::shared_ptr<KeypleReaderExtension> readerExtension
                        = poolPlugin->getReaderExtension(
                            typeid(KeypleReaderExtension), reader->getName());

                    auto cardResource = std::make_shared<CardResourceAdapter>(
                        reader, readerExtension, smartCard);

                    mService->registerPoolCardResource(
                        cardResource, poolPlugin);

                    return cardResource;
                }
            } else {
                releaseReaderSilently(poolPlugin, reader);
            }

        } catch (const KeyplePluginException&) {
            /* Continue */
        }
    }

    return nullptr;
}

void
CardProfileManagerAdapter::releaseReaderSilently(
    std::shared_ptr<PoolPlugin> poolPlugin, std::shared_ptr<CardReader> reader)
{
    try {
        poolPlugin->releaseReader(reader);
    } catch (const Exception&) {
        /* NOP £*/
    }
}

} /* namespace resource */
} /* namespace service */
} /* namespace core */
} /* namespace keyple */
