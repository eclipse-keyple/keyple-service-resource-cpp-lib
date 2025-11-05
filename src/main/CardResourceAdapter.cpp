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

namespace keyple {
namespace core {
namespace service {
namespace resource {

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

} /* namespace resource */
} /* namespace service */
} /* namespace core */
} /* namespace keyple */   
 