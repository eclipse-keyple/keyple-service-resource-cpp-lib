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

#include "CardResource.h"

/* Keyple Core Util */
#include "KeypleAssert.h"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using namespace keyple::core::util;

CardResource::CardResource(std::shared_ptr<CardReader> reader, std::shared_ptr<SmartCard> smartCard)
: mReader(reader), mSmartCard(smartCard)
{
    Assert::getInstance().notNull(reader, "reader").notNull(smartCard, "smartCard");
}

std::shared_ptr<CardReader> CardResource::getReader() const
{
    return mReader;
}

std::shared_ptr<SmartCard> CardResource::getSmartCard() const 
{
    return mSmartCard;
}

}
}
}
}
