#pragma once

#include <exception>
#include <vector>
#include <string>

namespace libdi
{

class CircularException : public std::exception
{
public:
    CircularException(const std::string& type)
    {
        stackTrace.push_back(type);
    }

    CircularException(const std::vector<std::string>& trace, const std::string& type)
        :stackTrace(trace)
    {
        stackTrace.push_back(type);
    }

    virtual char const* what() const override
    {
        return "Circular Exception detected";
    }

    const std::vector<std::string>& get()
    {
        return stackTrace;
    }

private:
    std::vector<std::string> stackTrace;
};

class TypeNotFoundException : public std::exception
{
public:
    TypeNotFoundException(const std::string& type)
    {
        errorMessage = std::move(type);
    }

    virtual char const* what() const override
    {
        return errorMessage.c_str();
    }

private:
    std::string errorMessage;
};

class TypeAlreadyRegistredException : public std::exception
{
public:
    TypeAlreadyRegistredException(const std::string& type)
    {
        errorMessage = std::move(type);
    }

    virtual char const* what() const override
    {
        return errorMessage.c_str();
    }

private:
    std::string errorMessage;
};

}
