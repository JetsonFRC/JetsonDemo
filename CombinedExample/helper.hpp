#ifndef HELPER_HPP
#define HELPER_HPP

#include <iostream>
#include <stdio>

using namespace std;
using namespace std::chrono;

typedef unsigned long long ui64;
typedef long long i64;


extern inline ui64 millis_since_epoch() {
    return (ui64) (time_point_cast<microseconds>(system_clock::now()));
}

#endif