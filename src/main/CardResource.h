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

/* Calypsonet Terminal Reader */
#include "CardReader.h"
#include "SmartCard.h"

/* Keyple Service Resource */
#include "KeypleServiceResourceExport.h"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using namespace calypsonet::terminal::reader;
using namespace calypsonet::terminal::reader::selection::spi;

/**
 * This POJO contains a SmartCard and its associated CardReader.
 *
 * @since 2.0.0
 */
class KEYPLESERVICERESOURCE_API CardResource final {
public:
    /**
     * Creates an instance of {@link CardResource}.
     *
     * @param reader The {@link CardReader}.
     * @param smartCard The {@link SmartCard}.
     * @since 2.0.0
     */
    CardResource(std::shared_ptr<CardReader> reader, std::shared_ptr<SmartCard> smartCard);

    /**
     * Gets the reader
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    std::shared_ptr<CardReader> getReader() const;

    /**
     * Gets the SmartCard.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    std::shared_ptr<SmartCard> getSmartCard() const;

private:
    /**
     * 
     */
    std::shared_ptr<CardReader> mReader;

    /**
     * 
     */
    std::shared_ptr<SmartCard> mSmartCard;
};

}
}
}
}
