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

#include <memory>

#include "keyple/core/service/resource/CardResource.hpp"
#include "keypop/reader/CardReader.hpp"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using keypop::reader::CardReader;

/**
 * Adapter of CardResource.
 *
 * @since 2.1.0
 */
class CardResourceAdapter final : public CardResource {
public:  
    /**
     * Creates new instance.
     *
     * @param reader The card reader.
     * @param readerExtension The Keyple reader's extension.
     * @param smartCard The smart card image.
     * @since 2.1.0
     */
    CardResourceAdapter(
        const std::shared_ptr<CardReader> reader, 
        const std::shared_ptr<KeypleReaderExtension> readerExtension, 
        const std::shared_ptr<SmartCard> smartCard);
  
    /**
     * {@inheritDoc}
     *
     * @since 2.1.0
     */
    std::shared_ptr<CardReader>
    getReader() const override;
  
    /**
     * {@inheritDoc}
     *
     * @since 2.1.0
     */
    std::shared_ptr<KeypleReaderExtension>
    getReaderExtension() const override;
  
    /**
     * {@inheritDoc}
     *
     * @since 2.1.0
     */
    std::shared_ptr<SmartCard>
    getSmartCard() const override;

private:
    /**
     * 
     */
    const std::shared_ptr<CardReader> mReader;

    /**
     * 
     */
    const std::shared_ptr<KeypleReaderExtension> mReaderExtension;

    /**
     * 
     */
    const std::shared_ptr<SmartCard> mSmartCard;
};

} /* namespace resource */
} /* namespace service */
} /* namespace core */
} /* namespace keyple */
