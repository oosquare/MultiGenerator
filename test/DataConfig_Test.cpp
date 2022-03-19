#include <iostream>
#include <cassert>

#include <MultiGenerator/DataConfig.hpp>

int main() {
    {
        MultiGenerator::DataConfig config;

        assert(config.insert("one", "1") == true);
        assert(config.insert("two", "2") == true);
        assert(config.insert("two", "II") == false);

        config.change("one", "I");
        config.change("two", "II");
        config.change("three", "III");

        assert(config.get("one").value() == "I");
        assert(config.get("two").value() == "II");
        assert(config.get("three").value() == "III");
        assert(config.get("four") == std::nullopt);
        assert(config.getOr("five", "V") == "V");

        assert(config.erase("three") == true);
        assert(config.erase("four") == false);

        assert(config.contain("two") == true);
        assert(config.contain("three") == false);
    }

    {
        MultiGenerator::DataConfig config({ {"one", "1"}, {"two", "2"}, {"three", "3"} });
        assert(config.get("one").value() == "1");
        assert(config.get("two").value() == "2");
        assert(config.get("three").value() == "3");
    }
    
    return 0;
}