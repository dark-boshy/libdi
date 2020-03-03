#pragma once

namespace libdi
{

class IRegistrationBuilder
{
public:

    virtual ~IRegistrationBuilder() {};

    virtual void InstancePerScope() = 0;

    virtual void InstancePerDependency() = 0;
};

}
