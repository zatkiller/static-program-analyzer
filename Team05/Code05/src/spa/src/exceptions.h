#pragma once

#include <string>
#include <exception>

#include "messages.h"

namespace exceptions {

struct Exception : public std::exception {
    const char* message;

    explicit Exception(const char* message) : message(message) {}
    const char * what () const noexcept override{
        return message;
    }
};

struct PqlException : Exception {
    PqlException() : Exception(messages::qps::parser::defaultPqlExceptionMesssage) {}
    explicit PqlException(const char* message) : Exception(message) {}
};

struct PqlSyntaxException : PqlException  {
    PqlSyntaxException() : PqlException(messages::qps::parser::defaultPqlSyntaxExceptionMessage) {}
    explicit PqlSyntaxException(const char* message) : PqlException(message) {}
};

struct PqlSemanticException : PqlException  {
    PqlSemanticException() : PqlException(messages::qps::parser::defaultPqlSemanticExceptionMessage) {}
    explicit PqlSemanticException(const char* message) : PqlException(message) {}
};

}  // namespace exceptions

