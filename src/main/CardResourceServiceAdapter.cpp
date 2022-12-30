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

#include "CardResourceServiceAdapter.h"

#include <sstream>

/* Keyple Core Util */
#include "IllegalStateException.h"
#include "KeypleAssert.h"
#include "System.h"

/* Keyple Service Resource */
#include "CardProfileManagerAdapter.h"

/* Keyple Core Service */
#include "ObservablePlugin.h"
#include "SmartCardServiceProvider.h"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using namespace keyple::core::service;
using namespace keyple::core::util;
using namespace keyple::core::util::cpp;
using namespace keyple::core::util::cpp::exception;

std::shared_ptr<CardResourceServiceAdapter> CardResourceServiceAdapter::mInstance;

std::shared_ptr<CardResourceServiceAdapter> CardResourceServiceAdapter::getInstance()
{
    if (mInstance == nullptr) {
        mInstance = std::make_shared<CardResourceServiceAdapter>();
    }

    return mInstance;
}

const std::string CardResourceServiceAdapter::getCardResourceInfo(
    const std::shared_ptr<CardResource> cardResource)
{
    if (cardResource != nullptr) {
        std::stringstream ss;
        ss << "card resource ("
           << HexUtil::toHex(System::identityHashCode(cardResource))
           << ") - reader '"
           << cardResource->getReader()->getName()
           << "' ("
           << HexUtil::toHex(System::identityHashCode(cardResource->getReader()))
           << ") - smart card ("
           << HexUtil::toHex(System::identityHashCode(cardResource->getSmartCard()))
            << ")";

        return ss.str();
    }

    return "";
}

std::shared_ptr<ReaderManagerAdapter> CardResourceServiceAdapter::getReaderManager(
    const std::shared_ptr<CardReader> reader) const
{
    const auto r = std::dynamic_pointer_cast<Reader>(reader);
    if (r == nullptr) {
        return nullptr;
    }

    const auto it = mReaderToReaderManagerMap.find(r);
    if (it != mReaderToReaderManagerMap.end()) {
        return it->second;
    }

    return nullptr;
}

void CardResourceServiceAdapter::registerPoolCardResource(
    std::shared_ptr<CardResource> cardResource, std::shared_ptr<PoolPlugin> poolPlugin)
{
    mCardResourceToPoolPluginMap.insert({cardResource, poolPlugin});
}

void CardResourceServiceAdapter::configure(
    std::shared_ptr<CardResourceServiceConfiguratorAdapter> configurator)
{
    mLogger->info("Applying a new configuration...\n");

    if (mIsStarted) {
        stop();
        mConfigurator = configurator;
        start();
    } else {
        mConfigurator = configurator;
    }

    mLogger->info("New configuration applied\n");
}

std::shared_ptr<CardResourceServiceConfigurator> CardResourceServiceAdapter::getConfigurator() const
{
    return std::make_shared<CardResourceServiceConfiguratorAdapter>();
}

void CardResourceServiceAdapter::start()
{
    if (mConfigurator == nullptr) {
        throw IllegalStateException("The card resource service is not configured.");
    }

    if (mIsStarted) {
        stop();
    }

    mLogger->info("Starting...\n");

    initializeReaderManagers();
    initializeCardProfileManagers();
    removeUnusedReaderManagers();
    startMonitoring();
    mIsStarted = true;

    mLogger->info("Started\n");
}

void CardResourceServiceAdapter::stop()
{
    mIsStarted = false;

    stopMonitoring();

    mReaderToReaderManagerMap.clear();
    mCardProfileNameToCardProfileManagerMap.clear();
    mCardResourceToPoolPluginMap.clear();
    mPluginToObservableReadersMap.clear();

    mLogger->info("Stopped\n");
}

std::shared_ptr<CardResource> CardResourceServiceAdapter::getCardResource(
    const std::string& cardResourceProfileName) const
{
    mLogger->debug("Searching a card resource for profile '%'...\n", cardResourceProfileName);

    if (!mIsStarted) {
        throw IllegalStateException("The card resource service is not started.");
    }

    Assert::getInstance().notEmpty(cardResourceProfileName, "cardResourceProfileName");

    std::shared_ptr<CardProfileManagerAdapter> cardProfileManager = nullptr;
    const auto& it = mCardProfileNameToCardProfileManagerMap.find(cardResourceProfileName);
    if (it != mCardProfileNameToCardProfileManagerMap.end()) {
        cardProfileManager = it->second;
    }

    Assert::getInstance().notNull(cardProfileManager, "cardResourceProfileName");

    std::shared_ptr<CardResource> cardResource = cardProfileManager->getCardResource();

    mLogger->debug("Found : %\n", getCardResourceInfo(cardResource));

    return cardResource;
}

void CardResourceServiceAdapter::releaseCardResource(std::shared_ptr<CardResource> cardResource)
{
    mLogger->debug("Releasing %...\n", getCardResourceInfo(cardResource));

    if (!mIsStarted) {
        throw IllegalStateException("The card resource service is not started.");
    }

    Assert::getInstance().notNull(cardResource, "cardResource");

    /* For regular or pool plugin ? */
    std::shared_ptr<ReaderManagerAdapter> readerManager = nullptr;

    const auto reader = cardResource->getReader();
    if (reader == nullptr) {
        throw IllegalArgumentException("Invalid reader");
    }

    const auto it = mReaderToReaderManagerMap.find(reader);
    if (it != mReaderToReaderManagerMap.end()) {
        readerManager = it->second;
        readerManager->unlock();
    } else {
        std::shared_ptr<PoolPlugin> poolPlugin = nullptr;
        const auto itt = mCardResourceToPoolPluginMap.find(cardResource);
        if (itt != mCardResourceToPoolPluginMap.end()) {
            poolPlugin = itt->second;
            mCardResourceToPoolPluginMap.erase(cardResource);
            poolPlugin->releaseReader(reader);
        }
    }

    mLogger->debug("Card resource released\n");
}

void CardResourceServiceAdapter::removeCardResource(std::shared_ptr<CardResource> cardResource)
{
    mLogger->debug("Removing %...\n", getCardResourceInfo(cardResource));

    releaseCardResource(cardResource);

    /* For regular plugin ? */
    std::shared_ptr<ReaderManagerAdapter> readerManager = nullptr;

    const auto r = std::dynamic_pointer_cast<Reader>(cardResource->getReader());
    if (r == nullptr) {
        throw IllegalArgumentException("Invalid reader");
    }

    const auto it = mReaderToReaderManagerMap.find(r);
    if (it != mReaderToReaderManagerMap.end()) {
        readerManager = it->second;
        readerManager->removeCardResource(cardResource);

        for (auto& pair : mCardProfileNameToCardProfileManagerMap) {
            pair.second->removeCardResource(cardResource);
        }
    }

    mLogger->debug("Card resource removed\n");
}

void CardResourceServiceAdapter::onPluginEvent(const std::shared_ptr<PluginEvent> pluginEvent)
{
    if (!mIsStarted) {
        return;
    }

    std::shared_ptr<Plugin> plugin =
        SmartCardServiceProvider::getService()->getPlugin(pluginEvent->getPluginName());

    if (pluginEvent->getType() == PluginEvent::Type::READER_CONNECTED) {
        for (const std::string& readerName : pluginEvent->getReaderNames()) {

            /* Get the new reader from the plugin because it is not yet registered in the service */
            std::shared_ptr<CardReader> reader = plugin->getReader(readerName);
            if (reader != nullptr) {
                const std::lock_guard<std::mutex> lock(mMutex);
                onReaderConnected(reader, plugin);
            }
        }
    } else {
        for (const std::string& readerName : pluginEvent->getReaderNames()) {

            /*
             * Get the reader back from the service because it is no longer registered in the
             * plugin.
             */
            std::shared_ptr<CardReader> reader = getReader(readerName);
            if (reader != nullptr) {
                /* The reader is registered in the service */
                const std::lock_guard<std::mutex> lock(mMutex);
                onReaderDisconnected(reader, plugin);
            }
        }
    }
}

void CardResourceServiceAdapter::onReaderEvent(const std::shared_ptr<CardReaderEvent> readerEvent)
{
    if (!mIsStarted) {
        return;
    }

    std::shared_ptr<CardReader> reader = getReader(readerEvent->getReaderName());
    if (reader != nullptr) {

        /* The reader is registered in the service */
        const std::lock_guard<std::mutex> lock(mMutex);
        const auto it = mReaderToReaderManagerMap.find(reader);
        if (it != mReaderToReaderManagerMap.end()) {
            onReaderEvent(readerEvent, it->second);
        }
    }
}

void CardResourceServiceAdapter::initializeReaderManagers()
{
    for (const auto& plugin : mConfigurator->getPlugins()) {
        for (const auto& reader : plugin->getReaders()) {
            registerReader(reader, plugin);
        }
    }
}

std::shared_ptr<ReaderManagerAdapter> CardResourceServiceAdapter::registerReader(
    std::shared_ptr<CardReader> reader, std::shared_ptr<Plugin> plugin)
{
    /* Get the reader configurator if a monitoring is requested for this reader */
    std::shared_ptr<ReaderConfiguratorSpi> readerConfiguratorSpi = nullptr;
    for (const auto& configuredPlugin : mConfigurator->getConfiguredPlugins()) {
        if (configuredPlugin->getPlugin() == plugin) {
            readerConfiguratorSpi = configuredPlugin->getReaderConfiguratorSpi();
            break;
        }
    }

    auto readerManager =
        std::make_shared<ReaderManagerAdapter>(reader,
                                               plugin,
                                               readerConfiguratorSpi,
                                               mConfigurator->getUsageTimeoutMillis());

    mReaderToReaderManagerMap.insert({reader, readerManager});

    const auto observable = std::dynamic_pointer_cast<ObservableCardReader>(reader);
    if (observable) {
        const auto it = mPluginToObservableReadersMap.find(plugin);
        if (it != mPluginToObservableReadersMap.end()) {
            /* Add element to existing vector */
            it->second.push_back(observable);
        } else {
            /* Create a new vector and add an entry to map */
            std::vector<std::shared_ptr<ObservableCardReader>> usedObservableReaders;
            usedObservableReaders.push_back(observable);
            mPluginToObservableReadersMap.insert({plugin, usedObservableReaders});
        }
    }

    return readerManager;
}

void CardResourceServiceAdapter::initializeCardProfileManagers()
{
    for (const auto& profile : mConfigurator->getCardResourceProfileConfigurators()) {
        mCardProfileNameToCardProfileManagerMap.insert({
            profile->getProfileName(),
            std::make_shared<CardProfileManagerAdapter>(profile, mConfigurator)});
    }
}

void CardResourceServiceAdapter::removeUnusedReaderManagers()
{
    std::vector<std::shared_ptr<ReaderManagerAdapter>> readerManagers;
    for (const auto& pair : mReaderToReaderManagerMap) {
        readerManagers.push_back(pair.second);
    }

    for (const auto& readerManager : readerManagers) {
        if (!readerManager->isActive()) {
            unregisterReader(readerManager->getReader(), readerManager->getPlugin());
        }
    }
}

void CardResourceServiceAdapter::unregisterReader(std::shared_ptr<CardReader> reader,
                                                  std::shared_ptr<Plugin> plugin)
{
    mReaderToReaderManagerMap.erase(reader);

    const auto it = mPluginToObservableReadersMap.find(plugin);
    auto observableCardReader = std::dynamic_pointer_cast<ObservableCardReader>(reader);

    if (it != mPluginToObservableReadersMap.end() && observableCardReader != nullptr) {
        observableCardReader->removeObserver(shared_from_this());
        it->second.erase(std::remove(it->second.begin(),
                                     it->second.end(),
                                     observableCardReader),
                         it->second.end());
    }
}

void CardResourceServiceAdapter::startMonitoring()
{
    for (auto& configuredPlugin : mConfigurator->getConfiguredPlugins()) {

        auto observable = std::dynamic_pointer_cast<ObservablePlugin>(configuredPlugin->getPlugin());
        if (configuredPlugin->isWithPluginMonitoring() && observable != nullptr) {
            mLogger->info("Start the monitoring of plugin '%'\n",
                          configuredPlugin->getPlugin()->getName());
            startPluginObservation(configuredPlugin);
        }

        const auto it = mPluginToObservableReadersMap.find(configuredPlugin->getPlugin());
        if (configuredPlugin->isWithReaderMonitoring() &&
            it != mPluginToObservableReadersMap.end()) {

            for (auto& reader : it->second) {
                mLogger->info("Start the monitoring of reader '%'\n", reader->getName());
                startReaderObservation(reader, configuredPlugin);
            }
        }
    }
}

void CardResourceServiceAdapter::stopMonitoring()
{
    for (auto& configuredPlugin : mConfigurator->getConfiguredPlugins()) {

        auto observable = std::dynamic_pointer_cast<ObservablePlugin>(configuredPlugin->getPlugin());
        if (configuredPlugin->isWithPluginMonitoring() && observable != nullptr) {
            mLogger->info("Stop the monitoring of plugin '%'\n",
                          configuredPlugin->getPlugin()->getName());
            std::dynamic_pointer_cast<ObservablePlugin>(configuredPlugin->getPlugin())
                ->removeObserver(shared_from_this());
        }

        const auto it = mPluginToObservableReadersMap.find(configuredPlugin->getPlugin());
        if (configuredPlugin->isWithReaderMonitoring() &&
            it != mPluginToObservableReadersMap.end()) {

            for (auto& reader : it->second) {
                mLogger->info("Stop the monitoring of reader '%'\n", reader->getName());
                reader->removeObserver(shared_from_this());
            }
        }
    }
}

std::shared_ptr<CardReader> CardResourceServiceAdapter::getReader(const std::string& readerName)
    const
{
    for (const auto& pair : mReaderToReaderManagerMap) {
        if (pair.first->getName() == readerName) {
            return pair.first;
        }
    }

    return nullptr;
}

void CardResourceServiceAdapter::onReaderConnected(std::shared_ptr<CardReader> reader,
                                                   std::shared_ptr<Plugin> plugin)
{
    std::shared_ptr<ReaderManagerAdapter> readerManager = registerReader(reader, plugin);
    for (auto pair : mCardProfileNameToCardProfileManagerMap) {
        pair.second->onReaderConnected(readerManager);
    }

    if (readerManager->isActive()) {
        startMonitoring(reader, plugin);
    } else {
        unregisterReader(reader, plugin);
    }
}

void CardResourceServiceAdapter::startMonitoring(std::shared_ptr<CardReader> reader,
                                                 std::shared_ptr<Plugin> plugin)
{
    auto observable = std::dynamic_pointer_cast<ObservableCardReader>(reader);
    if (observable != nullptr) {
        for (auto& configuredPlugin : mConfigurator->getConfiguredPlugins()) {
            if (configuredPlugin->getPlugin() == plugin &&
                configuredPlugin->isWithReaderMonitoring()) {
                mLogger->info("Start the monitoring of reader '%'\n", reader->getName());
                startReaderObservation(observable, configuredPlugin);
            }
        }
    }
}

void CardResourceServiceAdapter::startPluginObservation(
    std::shared_ptr<ConfiguredPlugin> configuredPlugin)
{
    auto observablePlugin =
        std::dynamic_pointer_cast<ObservablePlugin>(configuredPlugin->getPlugin());
    observablePlugin->setPluginObservationExceptionHandler(
        configuredPlugin->getPluginObservationExceptionHandlerSpi());
    observablePlugin->addObserver(shared_from_this());
}

void CardResourceServiceAdapter::startReaderObservation(
    std::shared_ptr<ObservableCardReader> observableReader,
    std::shared_ptr<ConfiguredPlugin> configuredPlugin)
{
    observableReader->setReaderObservationExceptionHandler(
        configuredPlugin->getReaderObservationExceptionHandlerSpi());
    observableReader->addObserver(shared_from_this());
    observableReader->startCardDetection(ObservableCardReader::DetectionMode::REPEATING);
}

void CardResourceServiceAdapter::onReaderDisconnected(std::shared_ptr<CardReader> reader,
                                                      std::shared_ptr<Plugin> plugin)
{
    const auto it = mReaderToReaderManagerMap.find(reader);
    if (it != mReaderToReaderManagerMap.end()) {
        mLogger->debug("Remove disconnected reader '%' and all associated card resources\n",
                       reader->getName());

        onCardRemoved(it->second);
        unregisterReader(reader, plugin);
    }
}

void CardResourceServiceAdapter::onReaderEvent(const std::shared_ptr<CardReaderEvent> readerEvent,
                                               std::shared_ptr<ReaderManagerAdapter> readerManager)
{
    if (readerEvent->getType() == CardReaderEvent::Type::CARD_INSERTED ||
        readerEvent->getType() == CardReaderEvent::Type::CARD_MATCHED) {
        mLogger->debug("Create new card resources associated with reader '%' matching the new " \
                       "card inserted\n",
                       readerManager->getReader()->getName());

        onCardInserted(readerManager);

    } else {
        mLogger->debug("Remove all card resources associated with reader '%' caused by a card " \
                       "removal or reader unregistration\n",
                       readerManager->getReader()->getName());

        onCardRemoved(readerManager);
    }
}

void CardResourceServiceAdapter::onCardInserted(std::shared_ptr<ReaderManagerAdapter> readerManager)
{
    for (auto& pair : mCardProfileNameToCardProfileManagerMap) {
        pair.second->onCardInserted(readerManager);
    }
}

void CardResourceServiceAdapter::onCardRemoved(std::shared_ptr<ReaderManagerAdapter> readerManager)
{
    const std::vector<std::shared_ptr<CardResource>>& cardResourcesToRemove =
        readerManager->getCardResources();

    for (const auto& cardResource : cardResourcesToRemove) {
        removeCardResource(cardResource);
    }
}

}
}
}
}
