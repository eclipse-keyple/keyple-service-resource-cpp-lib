#******************************************************************************
#* Copyright (c) 2025 Calypso Networks Association https://calypsonet.org/    *
#*                                                                            *
#* See the NOTICE file(s) distributed with this work for additional           *
#* information regarding copyright ownership.                                 *
#*                                                                            *
#* This program and the accompanying materials are made available under the   *
#* terms of the Eclipse Public License 2.0 which is available at              *
#* http://www.eclipse.org/legal/epl-2.0                                       *
#*                                                                            *
#* SPDX-License-Identifier: EPL-2.0                                           *
#******************************************************************************/

SET(LIBRARY_NAME keypleserviceresourcecpplib)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DKEYPLESERVICERESOURCE_EXPORT")

INCLUDE("CMakeLists.txt.keyple-service")

ADD_LIBRARY(

    ${LIBRARY_NAME}

    ${LIBRARY_TYPE}

    ${CMAKE_CURRENT_SOURCE_DIR}/CardProfileManagerAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardResourceAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardResourceProfileConfigurator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardResourceServiceAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardResourceServiceConfiguratorAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardResourceServiceProvider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PluginsConfigurator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PoolPluginsConfigurator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ReaderManagerAdapter.cpp
    )

TARGET_INCLUDE_DIRECTORIES(

    ${LIBRARY_NAME}

    PUBLIC

    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

TARGET_LINK_LIBRARIES(

    ${LIBRARY_NAME}

    PUBLIC

    Keyple::Service
)

ADD_LIBRARY(

    Keyple::Service::Resource

    ALIAS

    ${LIBRARY_NAME}
)
