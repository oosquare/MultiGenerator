#include <iostream>
#include <cassert>

#include <MultiGenerator/Context/Stream.hpp>
#include <MultiGenerator/Context/Environment.hpp>

namespace Context = MultiGenerator::Context;

void testEnvironment() {
    using Context::Environment;
    using Context::StandardInputStream;
    using Context::StandardOutputStream;

    {
        Environment env(
            std::make_unique<StandardInputStream>(),
            std::make_unique<StandardOutputStream>()
        );

        assert(env.hasInputStream() == true);
        assert(env.hasOutputStream() == true);
        assert(std::addressof(env.getInputStream()) == std::addressof(std::cin));
        assert(std::addressof(env.getOutputStream()) == std::addressof(std::cout));
    }

    {

        std::shared_ptr<Context::InputStream> pi;
        std::shared_ptr<Context::OutputStream> po;

        /**
         * Add () around "std::unique_ptr<Context::InputStream>()" to
         * avoid parse it to a function pointer.
         */
        Environment env(
            (std::unique_ptr<Context::InputStream>()),
            (std::unique_ptr<Context::OutputStream>())
        );

        assert(env.hasInputStream() == false);
        assert(env.hasOutputStream() == false);

        // std::string str;
        // env.getInputStream() >> str;
    }
}

int main() {
    testEnvironment();
    return 0;
}