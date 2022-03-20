#include <iostream>
#include <cassert>

#include <MultiGenerator/Stream.hpp>
#include <MultiGenerator/Environment.hpp>

void testEnvironment() {
    using MulitGenerator::Environment;
    using MulitGenerator::StandardInputStream;
    using MulitGenerator::StandardOutputStream;

    {
        Environment env(
            std::make_shared<StandardInputStream>(),
            std::make_shared<StandardOutputStream>()
        );

        assert(env.hasInputStream() == true);
        assert(env.hasOutputStream() == true);
        assert(std::addressof(env.getInputStream()) == std::addressof(std::cin));
        assert(std::addressof(env.getOutputStream()) == std::addressof(std::cout));
    }

    {

        std::shared_ptr<MulitGenerator::InputStream> pi;
        std::shared_ptr<MulitGenerator::OutputStream> po;

        /**
         * Add () around "std::shared_ptr<MulitGenerator::InputStream>()" to
         * avoid parse it to a function pointer.
         */
        Environment env(
            (std::shared_ptr<MulitGenerator::InputStream>()),
            (std::shared_ptr<MulitGenerator::OutputStream>())
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