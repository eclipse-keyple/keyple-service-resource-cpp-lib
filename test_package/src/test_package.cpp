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

#include <iostream>

#include "keyple/core/service/resource/CardResourceServiceProvider.hpp"

int
main()
{
    /* Calls into the actual compiled library (not just a header), proving
     * the binary resolved and linked correctly through Conan/CMake. */
    const auto service = keyple::core::service::resource::CardResourceServiceProvider::getService();

    std::cout << "Keyple::Service::Resource package resolved and linked correctly ("
              << (service != nullptr ? "service obtained" : "service unavailable") << ")" << std::endl;
    return service != nullptr ? 0 : 1;
}
