#pragma once

#include <string>
#include <exception>

#include "messages.h"

namespace exceptions {
    using namespace messages;

    struct Exception : public std::exception {
        const char* message;

        Exception(const char* message) : message(message) {};
        const char * what () const noexcept override{
            return message;
        }
    };

    struct PqlException : Exception {
        PqlException() : Exception(qps::parser::defaultPqlExceptionMesssage) {};
        PqlException(const char* message) : Exception(message) {} ;
    };

    struct PqlSyntaxException : PqlException  {
        PqlSyntaxException() : PqlException(qps::parser::defaultPqlSyntaxExceptionMessage) {};
        PqlSyntaxException(const char* message) : PqlException(message) {} ;
    };

    struct PqlSemanticException : PqlException  {
        PqlSemanticException() : PqlException(qps::parser::defaultPqlSemanticExceptionMessage) {};
        PqlSemanticException(const char* message) : PqlException(message) {} ;
    };
}

