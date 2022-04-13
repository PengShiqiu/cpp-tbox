#include <gtest/gtest.h>
#include "async_channel.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace tbox::log;

class TestAsyncChannel : public AsyncChannel {
  protected:
    virtual void onLogBackEnd(const std::string &log_text) override {
        cout << log_text << endl;
    }
};

class EmptyTestAsyncChannel : public AsyncChannel {
  protected:
    virtual void onLogBackEnd(const std::string &log_text) override { }
};


TEST(AsyncChannel, Format)
{
    TestAsyncChannel ch;

    ch.initialize(TestAsyncChannel::Config());
    ch.enable();

    LogInfo("%s, %d, %f", "hello", 123456, 12.345);
    LogInfo("%d, %f, %s", 123456, 12.345, "world");

    ch.cleanup();
}

TEST(AsyncChannel, LongString)
{
    TestAsyncChannel ch;

    ch.initialize(TestAsyncChannel::Config());
    ch.enable();
    std::string tmp(4096, 'x');
    LogInfo("%s", tmp.c_str());

    ch.cleanup();
}

TEST(AsyncChannel, TimeCast1)
{
    TestAsyncChannel ch;
    ch.initialize(TestAsyncChannel::Config());
    ch.enable();
    std::string tmp(30, 'm');

    auto start_ts = chrono::steady_clock::now();

    for (int i = 0; i < 10000; ++i)
        LogInfo("%s", tmp.c_str());

    auto end_ts = chrono::steady_clock::now();

    ch.cleanup();
    chrono::microseconds time_span = chrono::duration_cast<chrono::microseconds>(end_ts - start_ts);
    cout << "timecost: " << time_span.count() << " us" << endl;
}

TEST(AsyncChannel, TimeCast1_Empty)
{
    EmptyTestAsyncChannel ch;
    ch.initialize(TestAsyncChannel::Config());
    ch.enable();
    std::string tmp(30, 'm');

    auto start_ts = chrono::steady_clock::now();

    for (int i = 0; i < 10000; ++i)
        LogInfo("%s", tmp.c_str());

    auto end_ts = chrono::steady_clock::now();

    ch.cleanup();
    chrono::microseconds time_span = chrono::duration_cast<chrono::microseconds>(end_ts - start_ts);
    cout << "timecost: " << time_span.count() << " us" << endl;
}

TEST(AsyncChannel, TimeCast1_NotEnable)
{
    EmptyTestAsyncChannel ch;
    ch.initialize(TestAsyncChannel::Config());
    std::string tmp(30, 'm');

    auto start_ts = chrono::steady_clock::now();

    for (int i = 0; i < 10000; ++i)
        LogInfo("%s", tmp.c_str());

    auto end_ts = chrono::steady_clock::now();

    ch.cleanup();
    chrono::microseconds time_span = chrono::duration_cast<chrono::microseconds>(end_ts - start_ts);
    cout << "timecost: " << time_span.count() << " us" << endl;
}


TEST(AsyncChannel, TimeCast2)
{
    TestAsyncChannel ch;
    ch.initialize(TestAsyncChannel::Config());
    ch.enable();
    std::string tmp(30, 'm');

    auto start_ts = chrono::steady_clock::now();

    for (int i = 0; i < 10000; ++i) {
        LogInfo("%s", tmp.c_str());
        if (i % 100 == 0) {
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    }

    auto end_ts = chrono::steady_clock::now();

    ch.cleanup();
    chrono::microseconds time_span = chrono::duration_cast<chrono::microseconds>(end_ts - start_ts);
    cout << "timecost: " << time_span.count() << " us" << endl;
}

#include <tbox/event/loop.h>
using namespace tbox::event;

TEST(AsyncChannel, Benchmark)
{
    TestAsyncChannel ch;
    ch.initialize(TestAsyncChannel::Config());
    ch.enable();
    std::string tmp(30, 'x');

    auto sp_loop = Loop::New();

    int counter = 0;
    function<void()> func = [&] {
        for (int i = 0; i < 100; ++i)
            LogInfo("%d %s", i, tmp.c_str());
        sp_loop->runInLoop(func);
        counter += 100;
    };
    sp_loop->runInLoop(func);

    sp_loop->exitLoop(chrono::seconds(10));
    sp_loop->runLoop();

    delete sp_loop;
    cout << "count in sec: " << counter/10 << endl;
    ch.cleanup();
}

