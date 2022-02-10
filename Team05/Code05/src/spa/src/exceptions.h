#pragma once

#include <string>
#include <exception>

namespace exceptions {
    struct Exception : public std::exception {
        std::string message;

        Exception(std::string message) { this->message = message; };
        const char * what () const noexcept override{
            return message.c_str();
        }
    };

    struct PqlException : Exception {
        inline static std:: string DEFAULT_MESSAGE = "PqlExcaption is thrown!";
        PqlException() : Exception(DEFAULT_MESSAGE) {};
        PqlException(std::string message) : Exception(message) {} ;
    };


    struct PqlSyntaxException : PqlException  {
        inline static std:: string DEFAULT_MESSAGE = "PqlSyntaxExcaption is thrown!";
        PqlSyntaxException() : PqlException(DEFAULT_MESSAGE) {};
        PqlSyntaxException(std::string message) : PqlException(message) {} ;
    };
}
