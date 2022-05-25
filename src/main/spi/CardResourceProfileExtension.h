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

/* Calypsonet Terminal Reader */
#include "CardReader.h"
#include "CardSelectionManager.h"
#include "SmartCard.h"

namespace keyple {
namespace core {
namespace service {
namespace resource {
namespace spi {

using namespace calypsonet::terminal::reader;
using namespace calypsonet::terminal::reader::selection;
using namespace calypsonet::terminal::reader::selection::spi;

/**
 * Provides means to check if a reader contains a card that matches a given profile.
 *
 * @since 2.0.0
 */
class CardResourceProfileExtension {
public:
    /**
     * 
     */
    virtual ~CardResourceProfileExtension() = default;
    
    /**
     * Checks if a card is inserted in the provided reader, selects it, evaluates its profile and
     * potentially executes any necessary commands.
     *
     * @param reader The reader in which the card is supposed to be inserted.
     * @param cardSelectionManager A instance of CardSelectionManager.
     * @return A SmartCard or null if no card is inserted or if its profile does not match
     *         the associated data.
     * @since 2.0.0
     */
    virtual std::shared_ptr<SmartCard> matches(
        std::shared_ptr<CardReader> reader, 
        std::shared_ptr<CardSelectionManager> cardSelectionManager) = 0;
};

}
}
}
}
}
