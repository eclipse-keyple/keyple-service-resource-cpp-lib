/**************************************************************************************************
 * Copyright (c) 2021 Calypso Networks Association https://calypsonet.org/                        *
 *                                                                                                *
 * See the NOTICE file(s) distributed with this work for additional information regarding         *
 * copyright ownership.                                                                           *
 *                                                                                                *
 * This program and the accompanying materials are made available under the terms of the Eclipse  *
 * Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0                  *
 *                                                                                                *
 * SPDX-License-Identifier: EPL-2.0                                                               *
 **************************************************************************************************/

#include "PoolPluginsConfigurator.h"

/* Keyple Core Util */
#include "Arrays.h"
#include "IllegalStateException.h"
#include "KeypleAssert.h"

namespace keyple {
namespace core {
namespace service {
namespace resource {

using namespace keyple::core::util;
using namespace keyple::core::util::cpp;
using namespace keyple::core::util::cpp::exception;

using Builder = PoolPluginsConfigurator::Builder;

/* BUILDER -------------------------------------------------------------------------------------- */

Builder& PoolPluginsConfigurator::Builder::usePoolFirst()
{
    if (mUsePoolFirstConfigured == true) {
        throw IllegalStateException("Pool plugins priority already configured.");
    }

    mUsePoolFirst = true;
    mUsePoolFirstConfigured = true;

    return *this;
}

Builder& PoolPluginsConfigurator::Builder::addPoolPlugin(std::shared_ptr<PoolPlugin> poolPlugin)
{
    Assert::getInstance().notNull(poolPlugin, "poolPlugin");

    if (Arrays::contains(mPoolPlugins, poolPlugin)) {
        throw IllegalStateException("Pool plugin already configured.");
    }

    mPoolPlugins.push_back(poolPlugin);
    
    return *this;
}

std::shared_ptr<PoolPluginsConfigurator> PoolPluginsConfigurator::Builder::build() 
{
    if (mPoolPlugins.empty()) {
        throw IllegalStateException("No pool plugin was configured.");
    }

    if (mUsePoolFirstConfigured == false) {
        mUsePoolFirst = false;
    }
 
    return std::make_shared<PoolPluginsConfigurator>(this);
}

PoolPluginsConfigurator::Builder::Builder() : mUsePoolFirstConfigured(false) {}

/* POOL PLUGINS CONFIGURATOR -------------------------------------------------------------------- */

bool PoolPluginsConfigurator::isUsePoolFirst() const
{
    return mUsePoolFirst;
}

const std::vector<std::shared_ptr<PoolPlugin>>& PoolPluginsConfigurator::getPoolPlugins() const
{
    return mPoolPlugins;
}

Builder* PoolPluginsConfigurator::builder() 
{
    return new Builder();
}

PoolPluginsConfigurator::PoolPluginsConfigurator(const Builder* builder)
: mUsePoolFirst(builder->mUsePoolFirst), mPoolPlugins(builder->mPoolPlugins)
{
    /* Deleted builder here. It's been allocated with new */
    delete builder;
}

}
}
}
}
