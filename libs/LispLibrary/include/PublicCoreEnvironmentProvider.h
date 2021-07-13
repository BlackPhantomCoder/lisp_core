#pragma once
#include "CoreEnv.h"

class IPublicCoreEnvironmentProvider
{
public:
	virtual ~IPublicCoreEnvironmentProvider() = default;
	virtual const CoreEnvironment& get() const = 0;
};

class ReferencePublicCoreEnvironmentProvider :public IPublicCoreEnvironmentProvider {
public:
	ReferencePublicCoreEnvironmentProvider(const CoreEnvironment& env);
	virtual const CoreEnvironment& get() const override;
private:
	const CoreEnvironment& t_env;
};
