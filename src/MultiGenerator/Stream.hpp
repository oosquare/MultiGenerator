/**
 * @file Stream.hpp
 * @author Justin Chen (ctj12461@163.com)
 * @brief This file contains some wrappers of IO streams in C++ standard library.
 * @version 0.1
 * @date 2022-03-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <iostream>
#include <fstream>
#include <optional>
#include <string>
#include <exception>

namespace MulitGenerator {
    class FileOpenFailedException : std::exception {
    public:
        const char *what() const noexcept override {
            return "FileOpenFailedException: Failed to open this file.";
        }
    };
    
    /**
     * @brief An interface which declares a standard behavior to get an input stream.
     * 
     */
    class InputStream {
    public:
        InputStream() {}

        virtual ~InputStream() {}
        
        virtual std::istream &getStream() = 0;
    };

    class StandardInputStream : public InputStream {
    public:
        StandardInputStream() :
            InputStream() {}

        ~StandardInputStream() {}
        
        virtual std::istream &getStream() override {
            return std::cin;
        }
    };

    class FileInputStream : public InputStream {
    public:
        FileInputStream(const std::string &fileName) :
            ifs(fileName) {
            if (ifs.fail())
                throw FileOpenFailedException();
        }

        ~FileInputStream() {}

        virtual std::istream &getStream() override {
            return ifs;
        }
    private:
        std::ifstream ifs;
    };

    /**
     * @brief An interface which declares a standard behavior to get an output stream.
     * 
     */
    class OutputStream {
    public:
        OutputStream() {}

        virtual ~OutputStream() {}

        virtual std::ostream &getStream() = 0;
    };

    class StandardOutputStream : public OutputStream {
    public:
        StandardOutputStream() :
            OutputStream() {}

        ~StandardOutputStream() {}
        
        virtual std::ostream &getStream() override {
            return std::cout;
        }
    };

    class FileOutputStream : public OutputStream {
    public:
        FileOutputStream(const std::string &fileName) :
            ofs(fileName) {
            if (ofs.fail())
                throw FileOpenFailedException();
        }

        ~FileOutputStream() {}

        virtual std::ostream &getStream() override {
            return ofs;
        }
    private:
        std::ofstream ofs;
    };
} // namespace MulitGenerator