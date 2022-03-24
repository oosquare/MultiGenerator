/**
 * @file Callable.hpp
 * @author Justin Chen (ctj12461@163.com)
 * @brief This file declares the common interface of all callable objects.
 * @version 0.1
 * @date 2022-03-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

namespace MultiGenerator::Workflow {
    /**
     * @brief A interface of all callable classes.
     * 
     */
    class Callable {
    public:
        Callable() {}

        virtual ~Callable() {}

        virtual void call() = 0;
    };
} // namespace MultiGenerator::Workflow
