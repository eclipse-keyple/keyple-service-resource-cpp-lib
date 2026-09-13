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

#include "keyple/core/service/resource/CardResourceAdapter.hpp"

#include <ostream>

#include "keyple/core/util/HexUtil.hpp"
#include "keyple/core/util/cpp/System.hpp"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using keyple::core::util::HexUtil;
using keyple::core::util::cpp::System;

CardResourceAdapter::CardResourceAdapter(
  const std::shared_ptr<CardReader> reader,
  const std::shared_ptr<KeypleReaderExtension> readerExtension,
  const std::shared_ptr<SmartCard> smartCard)
: mReader(reader)
, mReaderExtension(readerExtension)
, mSmartCard(smartCard)
{
}

std::shared_ptr<CardReader> CardResourceAdapter::getReader() const
{
    return mReader;
}

std::shared_ptr<KeypleReaderExtension>
CardResourceAdapter::getReaderExtension() const
{
    return mReaderExtension;
}

std::shared_ptr<SmartCard>
CardResourceAdapter::getSmartCard() const
{
    return mSmartCard;
}

std::ostream&
operator<<(std::ostream& os, const CardResourceAdapter& cra)
{
    os << "CARD_RESOURCE_ADAPTER: {"
       << "READER_NAME: " << cra.mReader->getName() << ", "
       << "READER_HASH_CODE: "
          << HexUtil::toHex(System::identityHashCode(cra.mReader)) << ", "
       << "SMART_CARD_HASH_CODE: "
          << HexUtil::toHex(System::identityHashCode(cra.mSmartCard)) << ", "
       << "}";

    return os;
}

/**
 *
 */
std::ostream&
operator<<(
    std::ostream& os, const std::shared_ptr<CardResourceAdapter> cra)
{
    if (cra == nullptr) {
        os << "CARD_RESOURCE_ADAPTER: null";
    } else {
        os << "CARD_RESOURCE_ADAPTER: {"
           << "HASH_CODE: " << HexUtil::toHex(System::identityHashCode(cra)) << ", "
           << "READER_NAME: " << cra->mReader->getName() << ", "
           << "READER_HASH_CODE: "
               << HexUtil::toHex(System::identityHashCode(cra->mReader)) << ", "
           << "SMART_CARD_HASH_CODE: "
               << HexUtil::toHex(System::identityHashCode(cra->mSmartCard)) << ", "
           << "}";
    }

    return os;
}

} /* namespace resource */
} /* namespace service */
} /* namespace core */
} /* namespace keyple */
