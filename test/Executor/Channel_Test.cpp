#include <iostream>
#include <thread>
#include <atomic>
#include <map>
#include <cassert>

#include <MultiGenerator/Executor/Channel.hpp>

namespace Executor = MultiGenerator::Executor;

void testSenderReceiverCount() {
    auto [sender, receiver] = Executor::Channel<int>::create();
    assert(sender.receiverCount() == 1);
    assert(receiver.senderCount() == 1);

    auto sender2 = sender.share();
    assert(sender.receiverCount() == 1);
    assert(receiver.senderCount() == 2);

    auto receiver2 = receiver.share();
    assert(sender.receiverCount() == 2);
    assert(receiver.senderCount() == 2);

    sender.send(1);
    sender2.send(2);
    int res = receiver.receive().value();
    int res2 = receiver2.receive().value();
    assert((res == 1 && res2 == 2) || (res == 2 && res2 == 1));

    receiver.reset();
    receiver2.reset();
    assert(!sender.isOpen() && !sender2.isOpen() && !receiver.isOpen() && !receiver2.isOpen());
}

namespace TestChannel {
    using namespace Executor;
    using namespace std::literals::chrono_literals;

    struct Product {
        int data, from;
    };

    struct ProductComparator {
        bool operator()(const Product &lhs, const Product &rhs) const {
            return (lhs.from != rhs.from ? lhs.from < rhs.from : lhs.data < rhs.data);
        }
    };

    std::atomic_int sendTotal;
    std::atomic_int receiveTotal;
    std::map<Product, int, ProductComparator> remainCount;
    std::mutex mtx;

    void produce(Sender<Product> sender) {
        for (int i = 0; i < 100; ++i) {
            ++sendTotal;
            sender.send({ i * 10, i });

            std::lock_guard<std::mutex> lock(mtx);
            ++remainCount[{ i * 10, i }];
        }
    }

    std::optional<Product> get(Receiver<Product> &receiver) {
        std::optional<Product> res;

        do {
            res = receiver.receiveFor(20ms);
        } while (!res.has_value() && receiver.isOpen());

        return res;
    }

    bool output(std::optional<Product> res) {
        if (res.has_value()) {
            auto val = res.value();
            ++receiveTotal;

            std::lock_guard<std::mutex> lock(mtx);
            --remainCount[val];
            return true;
        } else {
            return false;
        }
    }

    void consume(Receiver<Product> receiver) {
        while (output(get(receiver))) {}
    }

    void start(int producerCount, int consumerCount) {
        std::vector<std::thread> producers(producerCount);
        std::vector<std::thread> consumers(consumerCount);

        auto [sender, receiver] = Channel<Product>::create();

        for (int i = 0; i < producerCount; ++i)
            producers[i] = std::thread(produce, sender.share());

        for (int i = 0; i < consumerCount; ++i)
            consumers[i] = std::thread(consume, receiver.share());

        sender.reset();
        receiver.reset();

        for (auto &h : producers)
            h.join();

        for (auto &h : consumers)
            h.join();

        assert(sendTotal == receiveTotal);

        for (auto [prod, cnt] : remainCount)
            assert(cnt == 0);

        sendTotal = 0;
        receiveTotal = 0;
        remainCount.clear();
    }
} // namespace TestChannel

void testChannel() {
    TestChannel::start(1, 1);
    TestChannel::start(1, 1000);
    TestChannel::start(1000, 1);
    TestChannel::start(1000, 1000);
}

int main() {
    testSenderReceiverCount();
    testChannel();
    return 0;
}