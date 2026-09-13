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

#include "keyple/core/common/KeypleReaderExtension.hpp"
#include "keyple/core/service/resource/KeypleServiceResourceExport.hpp"
#include "keypop/reader/CardReader.hpp"
#include "keypop/reader/selection/spi/SmartCard.hpp"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using keyple::core::common::KeypleReaderExtension;
using keypop::reader::CardReader;
using keypop::reader::selection::spi::SmartCard;

/**
 * This POJO contains a smart card and its associated card reader.
 *
 * @since 2.0.0
 */
class CardResource {
public:
    /**
     * Returns the reader
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    virtual std::shared_ptr<CardReader>
    getReader() const = 0;

    /**
     * Returns the Keyple reader's extension.
     *
     * @return A not null reference.
     * @since 2.1.0
     */
    virtual std::shared_ptr<KeypleReaderExtension>
    getReaderExtension() const = 0;

    /**
     * Returns the smart card image.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    virtual std::shared_ptr<SmartCard>
    getSmartCard() const = 0;
};

} /* namespace resource */
} /* namespace service */
} /* namespace core */
} /* namespace keyple */
