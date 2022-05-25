# *************************************************************************************************
# Copyright (c) 2021 Calypso Networks Association https://calypsonet.org/                         *
#                                                                                                 *
# See the NOTICE file(s) distributed with this work for additional information regarding          *
# copyright ownership.                                                                            *
#                                                                                                 *
# This program and the accompanying materials are made available under the terms of the Eclipse   *
# Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0                   *
#                                                                                                 *
# SPDX-License-Identifier: EPL-2.0                                                                *
# *************************************************************************************************/

SET(LIBRARY_NAME keypleserviceresourcecpplib)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DKEYPLESERVICERESOURCE_EXPORT")

SET(CALYPSONET_CARD_DIR    "../../../calypsonet-terminal-card-cpp-api")
SET(CALYPSONET_READER_DIR  "../../../calypsonet-terminal-reader-cpp-api")
SET(KEYPLE_COMMON_DIR      "../../../keyple-common-cpp-api")
SET(KEYPLE_PLUGIN_DIR      "../../../keyple-plugin-cpp-api")
SET(KEYPLE_SERVICE_DIR     "../../../keyple-service-cpp-lib")
SET(KEYPLE_UTIL_DIR        "../../../keyple-util-cpp-lib")

SET(KEYPLE_SERVICE_LIB     "keypleservicecpplib")
SET(KEYPLE_UTIL_LIB        "keypleutilcpplib")

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/spi

    ${CALYPSONET_CARD_DIR}/src/main
    ${CALYPSONET_CARD_DIR}/src/main/spi

    ${CALYPSONET_READER_DIR}/src/main
    ${CALYPSONET_READER_DIR}/src/main/selection
    ${CALYPSONET_READER_DIR}/src/main/selection/spi
    ${CALYPSONET_READER_DIR}/src/main/spi

    ${KEYPLE_COMMON_DIR}/src/main

    ${KEYPLE_PLUGIN_DIR}/src/main
    ${KEYPLE_PLUGIN_DIR}/src/main/spi
    ${KEYPLE_PLUGIN_DIR}/src/main/spi/reader
    ${KEYPLE_PLUGIN_DIR}/src/main/spi/reader/observable
    ${KEYPLE_PLUGIN_DIR}/src/main/spi/reader/observable/state/insertion
    ${KEYPLE_PLUGIN_DIR}/src/main/spi/reader/observable/state/processing
    ${KEYPLE_PLUGIN_DIR}/src/main/spi/reader/observable/state/removal

    ${KEYPLE_SERVICE_DIR}/src/main
    ${KEYPLE_SERVICE_DIR}/src/main/cpp
    ${KEYPLE_SERVICE_DIR}/src/main/spi

    ${KEYPLE_UTIL_DIR}/src/main
    ${KEYPLE_UTIL_DIR}/src/main/cpp
    ${KEYPLE_UTIL_DIR}/src/main/cpp/exception
)

ADD_LIBRARY(
    ${LIBRARY_NAME}

    ${LIBRARY_TYPE}

    ${CMAKE_CURRENT_SOURCE_DIR}/CardProfileManagerAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardResource.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardResourceProfileConfigurator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardResourceServiceAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardResourceServiceConfiguratorAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardResourceServiceProvider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PluginsConfigurator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PoolPluginsConfigurator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ReaderManagerAdapter.cpp
)

TARGET_LINK_LIBRARIES(${LIBRARY_NAME} ${KEYPLE_UTIL_LIB} ${KEYPLE_SERVICE_LIB})
