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

#include "keyple/core/service/resource/CardResourceService.hpp"
#include "keyple/core/service/resource/KeypleServiceResourceExport.hpp"

namespace keyple {
namespace core {
namespace service {
namespace resource {

/**
 * Provider of the CardResourceService.
 *
 * @since 2.0.0
 */
class KEYPLESERVICERESOURCE_API CardResourceServiceProvider final {
public:
    /**
     * Gets the unique instance of CardResourceService.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    static std::shared_ptr<CardResourceService> getService();

private:
    /**
     * Private constructor
     */
    CardResourceServiceProvider();
};

}
}
}
}
