//
// Created by NealShinoda on 2022/3/9.
//
#include "mysem.h"
#include<thread>
#include<linux/futex.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include<vector>
using namespace std;


static int futex(uint32_t *uaddr, int futex_op, uint32_t val, const struct timespec *timeout, uint32_t *uaddr2, uint32_t val3)
{
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

mysem::mysem(uint32_t init_value)
{
    this->counter = init_value;
}

void mysem::acquire()
{   
    uint32_t* counter_ptr = reinterpret_cast<uint32_t*>(&this->counter);
    uint32_t expected;
    
    //loop while expected is smaller than 0
    expected = counter.load(memory_order_seq_cst);
    
    if(expected<=0)
    {
        expected = 1;
    }
    //use cas to assign the value
    int count = 0;
    while (!counter.compare_exchange_strong(expected, expected-1, memory_order_seq_cst))
    {
        count++;
        if(expected<=0)
        {
            expected = 1;
        }
        //wait while the lock is occupied after tring 100 times
        if(count>=100)
        {
            long s = futex(counter_ptr, FUTEX_WAIT, 0, NULL, NULL, 0);
            count = 0;
        }
        
        
    }

}

void mysem::release()
{
    uint32_t* counter_ptr = reinterpret_cast<uint32_t*>(&this->counter);
    uint32_t expected = this->counter.load();
    while(!this->counter.compare_exchange_strong(expected, expected+1));
    if(expected==0)
    {        
        long s = futex(counter_ptr, FUTEX_WAKE, 1, NULL, NULL, 0);
    }   
}


void random_work()
{
    //use volatile to make sure the variable would not be optimized by the compiler
    for (volatile int i = 0; i < 10000; i++);
}


int main(int argc, char**argv)
{
    mysem s(1);
    vector<thread> vector_of_thread;
    for(int i = 0; i < 100; i++){
        
        std::thread t_temp([&](){
        s.acquire();
        int cur = i;
        // std::cout << cur; random_work(); std::cout << cur<<endl;
        std::cout << "Start from Thread ID : "<<std::this_thread::get_id() << "\n"<<endl;
        random_work();
        std::cout << "End from Thread ID :   "<<std::this_thread::get_id() << "\n"<<endl;

        s.release();
        });
        vector_of_thread.push_back(move(t_temp));
        
    }
    vector<thread>:: iterator itr = vector_of_thread.begin();
    for(; itr != vector_of_thread.end(); itr++)
    {
        itr->join();
    }
    
}
