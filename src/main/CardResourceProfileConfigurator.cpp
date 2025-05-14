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

#include "keyple/core/service/resource/CardResourceProfileConfigurator.hpp"

#include "keyple/core/util/KeypleAssert.hpp"
#include "keyple/core/util/cpp/Pattern.hpp"
#include "keyple/core/util/cpp/exception/IllegalArgumentException.hpp"
#include "keyple/core/util/cpp/exception/IllegalStateException.hpp"
#include "keyple/core/util/cpp/exception/PatternSyntaxException.hpp"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using keyple::core::util::Assert;
using keyple::core::util::cpp::Pattern;
using keyple::core::util::cpp::exception::IllegalArgumentException;
using keyple::core::util::cpp::exception::IllegalStateException;
using keyple::core::util::cpp::exception::PatternSyntaxException;

using Builder = CardResourceProfileConfigurator::Builder;

/* CARD RESOURCE PROFILE CONFIGURATOR --------------------------------------- */

CardResourceProfileConfigurator::CardResourceProfileConfigurator(
    Builder* builder)
: mProfileName(builder->mProfileName)
, mCardResourceProfileExtension(builder->mCardResourceProfileExtension)
, mPlugins(builder->mPlugins)
, mReaderNameRegex(builder->mReaderNameRegex)
, mReaderGroupReference(builder->mReaderGroupReference)
{
    /* Deleted builder here. It's been allocated with new */
    delete builder;
}

const std::string&
CardResourceProfileConfigurator::getProfileName() const
{
    return mProfileName;
}

std::shared_ptr<CardResourceProfileExtension>
CardResourceProfileConfigurator::getCardResourceProfileExtension() const
{
    return mCardResourceProfileExtension;
}

const std::vector<std::shared_ptr<Plugin>>&
CardResourceProfileConfigurator::getPlugins() const
{
    return mPlugins;
}

const std::string&
CardResourceProfileConfigurator::getReaderNameRegex() const
{
    return mReaderNameRegex;
}

const std::string&
CardResourceProfileConfigurator::getReaderGroupReference() const
{
    return mReaderGroupReference;
}

Builder*
CardResourceProfileConfigurator::builder(
    const std::string& profileName,
    std::shared_ptr<CardResourceProfileExtension> cardResourceProfileExtension)
{
    return new Builder(profileName, cardResourceProfileExtension);
}

/* BUILDER ------------------------------------------------------------------ */

Builder::Builder(
    const std::string& profileName,
    std::shared_ptr<CardResourceProfileExtension> cardResourceProfileExtension)
: mProfileName(profileName)
, mCardResourceProfileExtension(cardResourceProfileExtension)
, mReaderNameRegex("")
, mReaderGroupReference("")
{
    Assert::getInstance().notNull(
        cardResourceProfileExtension, "cardResourceProfileExtension");
}

Builder&
Builder::withPlugins(const std::vector<std::shared_ptr<Plugin>>& plugins)
{
    for (const auto& plugin : plugins) {
        Assert::getInstance().notNull(plugin, "plugin");
        mPlugins.push_back(plugin);
    }

    return *this;
}

Builder&
Builder::withReaderNameRegex(const std::string& readerNameRegex)
{
    Assert::getInstance().notEmpty(readerNameRegex, "readerNameRegex");

    if (mReaderNameRegex != "") {
        throw IllegalStateException("Reader name regex has already been set");
    }

    try {
        Pattern::compile(readerNameRegex);
    } catch (const PatternSyntaxException&) {
        throw IllegalArgumentException(
            "Invalid regular expression: " + readerNameRegex);
    }

    mReaderNameRegex = readerNameRegex;

    return *this;
}

Builder&
Builder::withReaderGroupReference(const std::string& readerGroupReference)
{
    Assert::getInstance().notEmpty(
        readerGroupReference, "readerGroupReference");

    if (mReaderGroupReference != "") {
        throw IllegalStateException(
            "Reader group reference has already been set");
    }

    mReaderGroupReference = readerGroupReference;

    return *this;
}

std::shared_ptr<CardResourceProfileConfigurator>
Builder::build()
{
    return std::make_shared<CardResourceProfileConfigurator>(this);
}

} /* namespace resource */
} /* namespace service */
} /* namespace core */
} /* namespace keyple */
