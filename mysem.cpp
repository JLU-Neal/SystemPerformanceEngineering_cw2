//
// Created by NealShinoda on 2022/3/9.
//
#include "mysem.h"
#include<thread>
using namespace std;


mysem::mysem(uint32_t init_value)
{
    this->counter = init_value;
}

void mysem::acquire()
{
    this->counter.fetch_sub(1);
}

void mysem::release()
{
    this->counter.fetch_add(1);
}


void random_work()
{
    cout<<"Damn you mother fucker!~~~~"<<endl;
}


int main(int argc, char**argv)
{
    mysem s(2);
    std::thread t1([&](){
    s.acquire();
    std::cout << 1; random_work(); std::cout << 1;
    s.release();
    });
    std::thread t2([&](){
    s.acquire();
    std::cout << 2; random_work(); std::cout << 2;
    s.release();
    });
    t1.join(); t2.join();
    std::cout << std::endl;
}
