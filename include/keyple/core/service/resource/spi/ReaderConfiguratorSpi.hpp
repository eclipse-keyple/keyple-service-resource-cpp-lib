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

#include "keypop/reader/CardReader.hpp"

namespace keyple {
namespace core {
namespace service {
namespace resource {
namespace spi {

using keypop::reader::CardReader;

/**
 * Reader configurator used to set up a new card reader connected with its
 * specific settings.
 *
 * <p>Note: since it depends on the type of reader, only the application
 * developer knows what settings to apply to the readers implemented by the Card
 * Resource Service in order for them to be fully operational.
 *
 * @since 2.0.0
 */
class ReaderConfiguratorSpi {
public:
    /**
     * Invoked when a new card reader is connected and accepted by at least one
     * card resource profile.
     *
     * <p>The setup is required for some specific readers and must be done
     * first.
     *
     * @param reader The reader to set up.
     * @since 2.0.0
     */
    virtual void 
    setupReader(std::shared_ptr<CardReader> reader) = 0;
};

} /* namespace spi */
} /* namespace resource */
} /* namespace service */
} /* namespace core */
} /* namespace keyple */
