#include "PublicCoreEnvironmentProvider.h"

ReferencePublicCoreEnvironmentProvider::ReferencePublicCoreEnvironmentProvider(const CoreEnvironment& env):
    t_env(env)
{
}

const CoreEnvironment& ReferencePublicCoreEnvironmentProvider::get() const
{
    return t_env;
}
