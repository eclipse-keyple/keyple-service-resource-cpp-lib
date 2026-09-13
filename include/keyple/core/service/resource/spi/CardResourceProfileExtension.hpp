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

#pragma once

#include "keypop/reader/CardReader.hpp"
#include "keypop/reader/ReaderApiFactory.hpp"
#include "keypop/reader/selection/CardSelectionManager.hpp"
#include "keypop/reader/selection/spi/SmartCard.hpp"

namespace keyple {
namespace core {
namespace service {
namespace resource {
namespace spi {

using keypop::reader::CardReader;
using keypop::reader::ReaderApiFactory;
using keypop::reader::selection::CardSelectionManager;
using keypop::reader::selection::spi::SmartCard;

/**
 * Provides means to check if a reader contains a card that matches a given
 * profile.
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
     * Checks if a card is inserted in the provided reader, selects it,
     * evaluates its profile and potentially executes any necessary commands.
     *
     * @param reader The reader in which the card is supposed to be inserted.
     * @param readerApiFactory A instance of readerApiFactory.
     * @return A SmartCard or null if no card is inserted or if its profile does
     * not match the associated data.
     * @since 2.0.0
     */
    virtual std::shared_ptr<SmartCard> 
    matches(
        std::shared_ptr<CardReader> reader,
        std::shared_ptr<ReaderApiFactory> readerApiFactory)
        = 0;

    /**
     * Checks if the provided selected smart card is as expected by the profile.
     *
     * @param smartCard The selected smart card.
     * @return The provided {@link SmartCard} or null if its profile does not
     *         match the associated data.
     * @since 3.1.0
     */
    virtual std::shared_ptr<SmartCard> 
    matches(const std::shared_ptr<SmartCard> smartCard) = 0;
};

} /* namespace spi */
} /* namespace resource */
} /* namespace service */
} /* namespace core */
} /* namespace keyple */
