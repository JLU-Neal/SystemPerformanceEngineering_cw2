//
// Created by NealShinoda on 2022/3/9.
//
#include "mysem.h"
#include<thread>
#include<linux/futex.h>
using namespace std;


mysem::mysem(uint32_t init_value)
{
    this->counter = init_value;
}

void mysem::acquire()
{   
    uint32_t expected;
    
    //loop while expected is smaller than 0
    while(expected = counter.load(memory_order_seq_cst)<=0);
    
    //use cas to assign the value
    while (!counter.compare_exchange_strong(expected, expected-1, memory_order_seq_cst))
    {
        if(expected<=0)
        {
            expected = 1;
        }
    }
    
    // this->counter.fetch_sub(1); 
}

void mysem::release()
{
    this->counter.fetch_add(1);
}


void random_work()
{
    for (volatile int i = 0; i < 10000; i++);
    // cout<<"Some random works"<<endl;
}


int main(int argc, char**argv)
{
    for(int i = 0; i < 100; i++){
        mysem s(1);
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
        std::thread t3([&](){
        s.acquire();
        std::cout << 3; random_work(); std::cout << 3;
        s.release();
        });
        t1.join(); t2.join(); t3.join();
        std::cout << std::endl;
    }
    
}
