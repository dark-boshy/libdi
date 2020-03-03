#include "pch.h"

#include "libdi/container.h"
#include "libdi/diexceptions.h"

using namespace libdi;

namespace
{
struct RegistrationBuilder : IRegistrationBuilder
{
    RegistrationBuilder(Internal::CreationParameter& param)
        :creationParemeter(param) {}


    ~RegistrationBuilder()
    {
        creationParemeter.instantiationType = instanceType;
    }

private:
    Internal::CreationParameter& creationParemeter;
    InstanceType instanceType = InstanceType::InstancePerScope;

    // Inherited via IRegistrationBuilder
    virtual void InstancePerScope() override
    {
        instanceType = InstanceType::InstancePerScope;
    }
    virtual void InstancePerDependency() override
    {
        instanceType = InstanceType::InstancePerRequest;
    }
};

}

std::shared_ptr<IComponent> Container::CreateInstanceInternal(const std::type_info & type)
{
    std::string typeName = type.name();

    auto IsCreatedIterator = createdObjects.find(typeName);

    if (IsCreatedIterator != end(createdObjects))
    {
        return IsCreatedIterator->second;
    }

    if (constructingTypes[typeName] == true)
    {
        throw CircularException(typeName);
    }

    std::shared_ptr<IComponent> component;

    try
    {
        auto constructorInterator = creationParemeters.find(typeName);

        if (constructorInterator == creationParemeters.end())
        {
            throw TypeNotFoundException(typeName);
        }

        constructingTypes[typeName] = true;


        auto func = constructorInterator->second.constructor;

        component = func();

        if (constructorInterator->second.instantiationType == InstanceType::InstancePerScope)
        {
            createdObjects[typeName] = component;
        }
    }
    catch (CircularException& e)
    {
        constructingTypes[typeName] = false;
        throw CircularException(e.get(), typeName);
    }

    constructingTypes[typeName] = false;

    return component;
}

std::unique_ptr<IRegistrationBuilder> Container::CreateBuilderInternal(std::string&& typeName)
{
    auto& entry = creationParemeters[typeName];
    return std::make_unique<RegistrationBuilder>(entry);
}
