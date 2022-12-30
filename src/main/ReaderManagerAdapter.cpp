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

#include "ReaderManagerAdapter.h"

/* Keyple Core Util */
#include "Arrays.h"
#include "IllegalStateException.h"
#include "System.h"

/* Keyple Core Service */
#include "SmartCardServiceProvider.h"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using namespace keyple::core::service;
using namespace keyple::core::util::cpp;
using namespace keyple::core::util::cpp::exception;

ReaderManagerAdapter::ReaderManagerAdapter(
   std::shared_ptr<CardReader> reader,
   std::shared_ptr<Plugin> plugin,
   std::shared_ptr<ReaderConfiguratorSpi> readerConfiguratorSpi,
   const int usageTimeoutMillis)
: mReader(reader),
  mPlugin(plugin),
  mReaderConfiguratorSpi(readerConfiguratorSpi),
  mUsageTimeoutMillis(usageTimeoutMillis),
  mSelectedCardResource(nullptr),
  mIsBusy(false),
  mIsActive(false) {}

std::shared_ptr<CardReader> ReaderManagerAdapter::getReader() const
{
    return mReader;
}

std::shared_ptr<Plugin> ReaderManagerAdapter::getPlugin() const
{
    return mPlugin;
}

const std::vector<std::shared_ptr<CardResource>>& ReaderManagerAdapter::getCardResources() const
{
    return mCardResources;
}

bool ReaderManagerAdapter::isActive() const
{
    return mIsActive;
}

void ReaderManagerAdapter::activate()
{
    if (!mIsActive) {
        mReaderConfiguratorSpi->setupReader(mReader);
    }

    mIsActive = true;
}

std::shared_ptr<CardResource> ReaderManagerAdapter::matches(
    std::shared_ptr<CardResourceProfileExtension> extension)
{
    std::shared_ptr<CardResource> cardResource = nullptr;
    std::shared_ptr<SmartCard> smartCard =
        extension->matches(mReader,
                           SmartCardServiceProvider::getService()->createCardSelectionManager());

    if (smartCard != nullptr) {
        cardResource = getOrCreateCardResource(smartCard);
        mSelectedCardResource = cardResource;
    }

    unlock();

    return cardResource;
}

bool ReaderManagerAdapter::lock(std::shared_ptr<CardResource> cardResource,
                                std::shared_ptr<CardResourceProfileExtension> extension)
{
    if (mIsBusy) {
        if (static_cast<uint64_t>(System::currentTimeMillis()) < mLockMaxTimeMillis) {
            return false;
        }

        mLogger->warn("Reader '%' automatically unlocked due to a usage duration over than % " \
                       "milliseconds\n",
                       mReader->getName(),
                       mUsageTimeoutMillis);
    }

    if (mSelectedCardResource != cardResource) {
        std::shared_ptr<SmartCard> smartCard =
            extension->matches(
                mReader,
                SmartCardServiceProvider::getService()->createCardSelectionManager());

        if (!areEquals(cardResource->getSmartCard(), smartCard)) {
            mSelectedCardResource = nullptr;
            throw IllegalStateException("No card is inserted or its profile does not match the " \
                                        "associated data.");
        }

        mSelectedCardResource = cardResource;
    }

    mLockMaxTimeMillis = System::currentTimeMillis() + mUsageTimeoutMillis;
    mIsBusy = true;

    return true;
}

void ReaderManagerAdapter::unlock()
{
    mIsBusy = false;
}

void ReaderManagerAdapter::removeCardResource(std::shared_ptr<CardResource> cardResource)
{
    Arrays::remove(mCardResources, cardResource);
    if (mSelectedCardResource == cardResource) {
        mSelectedCardResource = nullptr;
    }
}

std::shared_ptr<CardResource> ReaderManagerAdapter::getOrCreateCardResource(
    std::shared_ptr<SmartCard> smartCard)
{
    /* Check if an identical card resource is already created */
    for (const auto& cardResource : mCardResources) {
        if (areEquals(cardResource->getSmartCard(), smartCard)) {
            return cardResource;
        }
    }

    /* If none, then create a new one */
    auto cardResource = std::make_shared<CardResource>(mReader, smartCard);
    mCardResources.push_back(cardResource);

    return cardResource;
}

bool ReaderManagerAdapter::areEquals(const std::shared_ptr<SmartCard> s1,
                                     const std::shared_ptr<SmartCard> s2) const
{
    if (s1 == s2) {
       return true;
    }

    if (s1 == nullptr || s2 == nullptr) {
        return false;
    }

    bool hasSamePowerOnData = (s1->getPowerOnData() == "" && s2->getPowerOnData() == "") ||
                              (s1->getPowerOnData() != "" &&
                               s1->getPowerOnData() == s2->getPowerOnData());

    bool hasSameFci = Arrays::equals(s1->getSelectApplicationResponse(),
                                     s2->getSelectApplicationResponse());

    return hasSamePowerOnData && hasSameFci;
}

}
}
}
}
