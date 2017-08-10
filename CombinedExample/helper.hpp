#ifndef HELPER_HPP
#define HELPER_HPP

#include <iostream>
#include <stdio.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

typedef unsigned long long ui64;
typedef long long i64;


extern inline ui64 millis_since_epoch() {
    ui64 time = (time_point_cast<milliseconds>
        (system_clock::now()).time_since_epoch()).count();
    return time;
}

#endif