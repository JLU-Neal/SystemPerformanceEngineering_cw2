//
// Created by NealShinoda on 2022/3/9.
//

#ifndef SYSTEMPERFORMANCEENGINEERING_CW2_MYSEM_H
#define SYSTEMPERFORMANCEENGINEERING_CW2_MYSEM_H
#include<iostream>
#include<atomic>
using namespace std;
class mysem {

public:
    mysem(uint32_t init_value);
    void acquire();
    void release();
private:
    std::atomic<uint32_t> counter;
};
#endif //SYSTEMPERFORMANCEENGINEERING_CW2_MYSEM_H
