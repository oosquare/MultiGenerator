#include <iostream>
#include <cassert>

#include <MultiGenerator/DataConfig.hpp>
#include <MultiGenerator/Argument.hpp>

void testNormalArgument() {
    using MultiGenerator::DataConfig;
    using MultiGenerator::NormalArgument;

    {
        NormalArgument arg;
        assert(arg.getID() == arg.getUninitializedID());
    }

    {
        NormalArgument arg(1);
        assert(arg.getID() == "1");
    }

    {
        DataConfig config({ {"one", "1"}, {"four", "4"} });
        NormalArgument arg(2, config);
        assert(arg.getID() == "2");
        assert(arg.getConfig().get("one").value() == "1");
        assert(arg.getConfig().getOr("two", "2") == "2");
        assert(arg.getConfig().get("three") == std::nullopt);
    }
}

void testSubtaskArgument() {
    using MultiGenerator::DataConfig;
    using MultiGenerator::SubtaskArgument;

    {
        SubtaskArgument arg;
        assert(arg.getID() == arg.getUninitializedID());
    }

    {
        SubtaskArgument arg(1, 1);
        assert(arg.getID() == "1-1");
    }

    {
        DataConfig config({ {"one", "1"}, {"four", "4"} });
        SubtaskArgument arg(1, 2, config);
        assert(arg.getID() == "1-2");
        assert(arg.getConfig().get("one").value() == "1");
        assert(arg.getConfig().getOr("two", "2") == "2");
        assert(arg.getConfig().get("three") == std::nullopt);
    }
}

void testArgument() {
    using MultiGenerator::Argument;
    using MultiGenerator::NormalArgument;
    using MultiGenerator::SubtaskArgument;

    {
        std::shared_ptr<Argument> ptrA = std::make_shared<NormalArgument>(1);
        std::shared_ptr<Argument> ptrB = std::make_shared<SubtaskArgument>(2, 1);
        assert(ptrA->getID() == "1");
        assert(ptrB->getID() == "2-1");
    }
}

void testNormalArgumentList() {
    using MultiGenerator::NormalArgumentList;

    {
        NormalArgumentList list;
        list.insert(1);
        list.insert(2);
        list.insert(3);

        assert(list.next().value()->getID() == "3");
        assert(list.next().value()->getID() == "2");
        assert(list.next().value()->getID() == "1");
        assert(list.next() == std::nullopt);
    }

    {
        NormalArgumentList list;

        for (int i = 1; i <= 5; ++i)
            list.insert(i);

        std::string str;

        for (auto res = list.next(); res != std::nullopt; res = list.next())
            str += res.value()->getID() + " ";

        assert(str == "5 4 3 2 1 ");
    }
}

void testSubtaskArgumentList() {
    using MultiGenerator::SubtaskArgumentList;

    {
        SubtaskArgumentList list;
        list.insert(1, 1);
        list.insert(1, 2);
        list.insert(1, 3);

        assert(list.next().value()->getID() == "1-3");
        assert(list.next().value()->getID() == "1-2");
        assert(list.next().value()->getID() == "1-1");
        assert(list.next() == std::nullopt);
    }

    {
        SubtaskArgumentList list;

        for (int i = 1; i <= 5; ++i)
            list.insert(1, i);

        std::string str;

        for (auto res = list.next(); res != std::nullopt; res = list.next())
            str += res.value()->getID() + " ";

        assert(str == "1-5 1-4 1-3 1-2 1-1 ");
    }
}

void testArgumentList() {
    using MultiGenerator::ArgumentList;
    using MultiGenerator::NormalArgumentList;
    using MultiGenerator::SubtaskArgumentList;

    {
        std::shared_ptr<ArgumentList> list;
        auto normalList = std::make_shared<NormalArgumentList>();
        auto subtaskList = std::make_shared<SubtaskArgumentList>();

        normalList->insert(1);
        normalList->insert(2);

        subtaskList->insert(1, 1);
        subtaskList->insert(1, 2);

        std::string str;

        list = normalList;
        str += list->next().value()->getID() + " ";
        str += list->next().value()->getID() + " ";
        list = subtaskList;
        str += list->next().value()->getID() + " ";
        str += list->next().value()->getID() + " ";

        assert(str == "2 1 1-2 1-1 ");
    }
}

int main() {
    testNormalArgument();
    testSubtaskArgument();
    testArgument();
    testNormalArgumentList();
    testSubtaskArgumentList();
    testArgumentList();
    return 0;
}

