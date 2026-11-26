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

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(KeypleServiceResourceCppLib
        VERSION 3.1.1
        LANGUAGES C CXX)

SET(CMAKE_MACOSX_RPATH 1)
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compilers
SET(CMAKE_C_COMPILER_WORKS 1)
SET(CMAKE_CXX_COMPILER_WORKS 1)

# Compiler warnings (compiler/OS selection itself is now handled by the
# Conan profile in use).
IF(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    ADD_COMPILE_OPTIONS(-Wall -Wextra -Wdeprecated -Wno-overloaded-virtual -pedantic -pedantic-errors -Werror)
ELSEIF(MSVC)
    ADD_COMPILE_OPTIONS(/W4 /WX /wd4250 /wd4251 /wd4275 /EHsc)
ENDIF()

# Set common output directory
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add projects
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/src)
