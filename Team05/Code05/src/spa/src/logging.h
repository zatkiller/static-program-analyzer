/*
 * Small Header only logging utility written for CS3203 T05
 * Example Usage:
 * Logger(Level::INFO) << "Testing INFO Logger\n" << "It works similar to std::cout";
 * 
 */

#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

enum class Level {
    INFO,
    WARN,
    DEBUG,
    ERROR,
    OK,
};

class Logger {
public:
    template<typename T>
    Logger& operator<< (const T& msg) {
        this->oss << msg;
        return *this;
    }

    Logger(Level level) {
        this->level = level;
        this->oss << this->getHeader();
    }

    ~Logger() {
        // Using stdout for now. May consider log file later.
        switch (this->level) {
        case Level::ERROR:
            std::cerr << this->oss.str() << std::endl;
            break;
        default:
            std::cout << this->oss.str() << std::endl;
            break;
        }
        
    }

private:
    std::stringstream oss;
    Level level;

    std::string getLabel() {
        std::string label;
        switch (this->level) {
        case Level::INFO: label = "[INFO]"; break;
        case Level::WARN: label = "[WARN]"; break;
        case Level::DEBUG: label = "[DEBUG]"; break;
        case Level::ERROR: label = "[ERROR]"; break;
        case Level::OK: label = "[OK]"; break;
        }
        return label;
    }

    // Referenced from https://stackoverflow.com/questions/16357999/current-date-and-time-as-string/16358264
    std::string getTime() {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::stringstream oss;
        oss << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
        return oss.str();
    }

    std::string getHeader() {
        std::stringstream oss;
        oss << getTime() << " " << getLabel() << " ";
        return oss.str();
    }
};
