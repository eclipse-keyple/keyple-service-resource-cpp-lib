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

#include "keyple/core/service/resource/ReaderManagerAdapter.hpp"

#include "keyple/core/service/SmartCardServiceProvider.hpp"
#include "keyple/core/service/resource/CardResourceAdapter.hpp"
#include "keyple/core/util/cpp/Arrays.hpp"
#include "keyple/core/util/cpp/System.hpp"
#include "keyple/core/util/cpp/exception/IllegalStateException.hpp"
#include "keypop/reader/selection/spi/IsoSmartCard.hpp"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using keyple::core::service::SmartCardServiceProvider;
using keyple::core::service::resource::CardResourceAdapter;
using keyple::core::util::cpp::Arrays;
using keyple::core::util::cpp::System;
using keyple::core::util::cpp::exception::IllegalStateException;
using keypop::reader::selection::spi::IsoSmartCard;

ReaderManagerAdapter::ReaderManagerAdapter(
    std::shared_ptr<CardReader> reader,
    std::shared_ptr<Plugin> plugin,
    std::shared_ptr<ReaderConfiguratorSpi> readerConfiguratorSpi,
    const int usageTimeoutMillis)
: mReader(reader)
, mReaderExtension(plugin->getReaderExtension(
      typeid(KeypleReaderExtension), reader->getName()))
, mPlugin(plugin)
, mReaderConfiguratorSpi(readerConfiguratorSpi)
, mUsageTimeoutMillis(usageTimeoutMillis)
, mSelectedCardResource(nullptr)
, mIsBusy(false)
, mIsActive(false)
{
}

std::shared_ptr<CardReader>
ReaderManagerAdapter::getReader() const
{
    return mReader;
}

std::shared_ptr<Plugin>
ReaderManagerAdapter::getPlugin() const
{
    return mPlugin;
}

const std::vector<std::shared_ptr<CardResourceAdapter>>&
ReaderManagerAdapter::getCardResources() const
{
    return mCardResources;
}

bool
ReaderManagerAdapter::isActive() const
{
    return mIsActive;
}

void
ReaderManagerAdapter::activate()
{
    if (!mIsActive) {
        mReaderConfiguratorSpi->setupReader(mReader);
    }

    mIsActive = true;
}

std::shared_ptr<CardResourceAdapter>
ReaderManagerAdapter::matches(
    std::shared_ptr<CardResourceProfileExtension> extension)
{
    std::shared_ptr<CardResourceAdapter> cardResource = nullptr;
    std::shared_ptr<SmartCard> smartCard = extension->matches(
        mReader, SmartCardServiceProvider::getService()->getReaderApiFactory());

    if (smartCard != nullptr) {
        cardResource = getOrCreateCardResource(smartCard);
        mSelectedCardResource = cardResource;
    }

    unlock();

    return cardResource;
}

bool
ReaderManagerAdapter::lock(
    std::shared_ptr<CardResource> cardResource,
    std::shared_ptr<CardResourceProfileExtension> extension)
{
    if (mIsBusy) {
        if (mUsageTimeoutMillis == 0
            || static_cast<uint64_t>(System::currentTimeMillis())
                   < mLockMaxTimeMillis) {
            return false;
        }

        mLogger->warn(
            "Reader [%] automatically unlocked due to a usage duration over "
            "than % milliseconds\n",
            mReader->getName(),
            mUsageTimeoutMillis);
    }

    if (mSelectedCardResource != cardResource) {
        std::shared_ptr<SmartCard> smartCard = extension->matches(
            mReader,
            SmartCardServiceProvider::getService()->getReaderApiFactory());

        if (!areEquals(cardResource->getSmartCard(), smartCard)) {
            mSelectedCardResource = nullptr;
            throw IllegalStateException(
                "No card is inserted or its profile does not match the "
                "associated data.");
        }

        mSelectedCardResource = cardResource;
    }

    mLockMaxTimeMillis = System::currentTimeMillis() + mUsageTimeoutMillis;
    mIsBusy = true;

    return true;
}

void
ReaderManagerAdapter::unlock()
{
    mIsBusy = false;
}

void
ReaderManagerAdapter::removeCardResource(
    std::shared_ptr<CardResource> cardResource)
{
    Arrays::remove(
        mCardResources,
        std::dynamic_pointer_cast<CardResourceAdapter>(cardResource));

    if (mSelectedCardResource == cardResource) {
        mSelectedCardResource = nullptr;
    }
}

std::shared_ptr<CardResourceAdapter>
ReaderManagerAdapter::getOrCreateCardResource(
    std::shared_ptr<SmartCard> smartCard)
{
    /* Check if an identical card resource is already created */
    for (const auto& cardResource : mCardResources) {
        if (areEquals(cardResource->getSmartCard(), smartCard)) {
            return cardResource;
        }
    }

    /* If none, then create a new one */
    auto cardResource = std::make_shared<CardResourceAdapter>(
        mReader, mReaderExtension, smartCard);
    mCardResources.push_back(cardResource);

    return cardResource;
}

bool
ReaderManagerAdapter::areEquals(
    const std::shared_ptr<SmartCard> s1, const std::shared_ptr<SmartCard> s2)
{
    if (s1 == s2) {
        return true;
    }

    if (s1 == nullptr || s2 == nullptr) {
        return false;
    }

    bool hasSamePowerOnData
        = (s1->getPowerOnData() == "" && s2->getPowerOnData() == "")
          || (s1->getPowerOnData() != ""
              && s1->getPowerOnData() == s2->getPowerOnData());

    bool hasSameFci;
    const auto _s1 = std::dynamic_pointer_cast<IsoSmartCard>(s1);
    const auto _s2 = std::dynamic_pointer_cast<IsoSmartCard>(s2);
    if (_s1 && _s2) {
        hasSameFci = Arrays::equals(
            _s1->getSelectApplicationResponse(),
            _s2->getSelectApplicationResponse());
    } else {
        hasSameFci = true;
    }

    return hasSamePowerOnData && hasSameFci;
}

} /* namespace resource */
} /* namespace service */
} /* namespace core */
} /* namespace keyple */
