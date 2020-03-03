#pragma once

#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>

#include "diexceptions.h"
#include "icomponent.h"
#include "iscope.h"
#include "iregistrationbuilder.h"

namespace libdi
{

namespace Internal
{
using inner_t = std::shared_ptr<IComponent>;
using constructor_t = std::function<inner_t(void)>;

struct CreationParameter
{
    constructor_t constructor;
    InstanceType instantiationType;
};
}

class Container : public IScope
{
    using inner_t = Internal::inner_t;
    using constructor_t = Internal::constructor_t;

public:
    // Inherited via IScope
    virtual std::shared_ptr<IComponent> CreateInstanceInternal(const std::type_info & type) override;

    template <class T, class U,
        typename std::enable_if<std::is_base_of<IComponent, typename T>::value, typename T>::type* = nullptr,
        typename std::enable_if<std::is_base_of<typename T, typename U>::value, typename T>::type* = nullptr
    >
        std::unique_ptr<IRegistrationBuilder> RegisterType()
    {
        if constexpr (std::is_constructible<U>::value)
        {
            std::function < std::shared_ptr<T>() > constructor = [=]()
            {
                return std::make_shared<U>();
            };
            return Register<T>(constructor);
        }
        else if constexpr (std::is_constructible<U, Container&>::value)
        {
            std::function < std::shared_ptr<T>() > constructor = [=]()
            {
                return std::make_shared<U>(*this);
            };
            return Register<T>(constructor);
        }
        else
        {
            static_assert(false, "Supports only constructors without parameters and constructors with single parameter of type Container&");
        }
    }

    template <class T, typename std::enable_if<std::is_base_of<IComponent, typename T>::value, typename T>::type* = nullptr>
    std::unique_ptr<IRegistrationBuilder> Register(std::function<std::shared_ptr<T>()> constructor)
    {
        const std::type_info& typeInfo = typeid(T);

        std::string typeName = typeInfo.name();

        auto register_it = creationParemeters.find(typeName);

        if (register_it != creationParemeters.end())
        {
            throw TypeAlreadyRegistredException(typeName);
        }
        creationParemeters[typeName] = { constructor, InstanceType::InstancePerScope };

        return CreateBuilderInternal(std::move(typeName));
    }

    template <class T, class U, typename std::enable_if<std::is_base_of<IComponent, typename T>::value, typename T>::type* = nullptr>
    void RegisterInstance(typename std::shared_ptr<U> instance)
    {
        const std::type_info& typeInfo = typeid(T);

        std::string typeName = typeInfo.name();

        auto register_it = creationParemeters.find(typeName);

        if (register_it != creationParemeters.end())
        {
            throw TypeAlreadyRegistredException(typeName);
        }
        creationParemeters[typeName] = { constructor_t(), InstanceType::InstancePerScope };
        createdObjects[typeName] = instance;
    }

private:
    std::unique_ptr<IRegistrationBuilder> CreateBuilderInternal(std::string&& typeName);

    std::unordered_map < std::string, Internal::CreationParameter > creationParemeters;
    std::unordered_map<std::string, inner_t> createdObjects;
    std::unordered_map<std::string, bool> constructingTypes;
};

}
