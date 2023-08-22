//
// Created by nz on 22.08.2023.
//
#ifndef GTS_CORE_MIRROR_H
#define GTS_CORE_MIRROR_H
#include "mongoose.h"
#include "json.hpp"
#include <array>
#include <thread>
#include <chrono>
#include <functional>
#include <cstdio>
#include <atomic>

using Json = nlohmann::json;

class Timer {
public:
    ~Timer() {
        if (mRunning) {
            stop();
        }
    }
    typedef std::chrono::milliseconds Interval;
    typedef std::function<void(void)> Timeout;

    void start(const Interval &interval, const Timeout &timeout) {
        mRunning = true;

        mThread = std::thread([this, interval, timeout] {
            while (mRunning) {
                std::this_thread::sleep_for(interval);

                timeout();
            }
        });
    }
    void stop() {
        mRunning = false;
        mThread.join();
    }

private:
    std::thread mThread{};
    std::atomic_bool mRunning{};
};

class Mirror{
private:
    const static inline char req_arm_10V[] = "\x02\x05\x00\x2c\x02";
    std::array<std::string, 1> addresses = {"udp://192.168.10.70:8888"};
    Timer timer;
    void update(int index);

    struct mg_mgr mgr;
    struct mg_connection *c;

    static void eventProcessorUDP(struct mg_connection *c, int ev, void *ev_data, void *fn_data);

    char outBuff[6];


public:
    Json requestHandler(Json req);
    //bool connect(){return false;};
};

#endif //GTS_CORE_MIRROR_H
