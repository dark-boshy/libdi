#pragma once

#include <type_traits>

#include "IComponent.h"

namespace libdi
{

enum class InstanceType
{
    InstancePerScope,
    InstancePerRequest,
};

class IScope
{
public:
    virtual ~IScope() = 0 {};

public:
    template <class T, typename std::enable_if<std::is_base_of<IComponent, typename T>::value, typename T>::type* = nullptr>
    std::shared_ptr<typename T> Resolve()
    {
        const std::type_info& type = typeid(T);

        auto component = CreateInstanceInternal(type);

        return std::static_pointer_cast<T>(component);
    }

protected:

    virtual std::shared_ptr<IComponent> CreateInstanceInternal(const std::type_info& type) = 0;
};

}
