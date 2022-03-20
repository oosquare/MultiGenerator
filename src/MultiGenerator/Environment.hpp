/**
 * @file Environment.hpp
 * @author Justin Chen (ctj12461@163.com)
 * @brief This file contains the class Environment which is used to manage
 * some resources such as IO devices.
 * @version 0.1
 * @date 2022-03-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <exception>
#include <memory>

#include "Stream.hpp"

namespace MulitGenerator {
    class StreamNotOpenException : public std::exception {
    public:
        const char *what() const noexcept override {
            return "StreamNotOpenException: IO Stream in Environment hasn't opened yet.";
        }
    };
    
    /**
     * @brief A class which provides IO devices
     * 
     */
    class Environment {
    public:
        /**
         * @brief Construct a new Environment object. Leave params empty std::shared_ptr
         * if needing ot keep the streams closed.
         *
         * @param is std::shared_ptr of a InputStream 
         * @param os std::shared_ptr of a OutputStream
         */
        Environment(std::shared_ptr<InputStream> is, std::shared_ptr<OutputStream> os) :
            is(is),
            os(os) {}

        ~Environment() {}

        bool hasInputStream() const {
            return static_cast<bool>(is);
        }

        std::istream &getInputStream() {
            if (!hasInputStream())
                throw StreamNotOpenException();
            
            return is->getStream();
        }

        bool hasOutputStream() const {
            return static_cast<bool>(os);
        }

        std::ostream &getOutputStream() {
            if (!hasOutputStream())
                throw StreamNotOpenException();
            
            return os->getStream();
        }
    private:
        std::shared_ptr<InputStream> is;
        std::shared_ptr<OutputStream> os;
    };
} // namespace MulitGenerator