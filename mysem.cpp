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
    // cout<<"expected"<<expected<<endl;
    //use cas to assign the value
    while (!counter.compare_exchange_strong(expected, expected-1, memory_order_seq_cst))
    {
        if(expected<=0)
        {
            expected = 1;
        }
        
        long s = futex(counter_ptr, FUTEX_WAIT, 0, NULL, NULL, 0);
        cout<<"thread wake up"<<endl;
    }

}

void mysem::release()
{
    // cout<<"Current val of counter:"<<this->counter<<endl;
    uint32_t* counter_ptr = reinterpret_cast<uint32_t*>(&this->counter);
    this->counter.fetch_add(1);
    long s = futex(counter_ptr, FUTEX_WAKE, 1, NULL, NULL, 0);
}


void random_work()
{
    // cout<<endl;
    for (volatile int i = 0; i < 1000000; i++);
    // cout<<"Some random works"<<endl;
}


int main(int argc, char**argv)
{
    mysem s(1);
    vector<thread> vector_of_thread;
    for(int i = 0; i < 5; i++){
        
        std::thread t_temp([&](){
        s.acquire();
        // std::cout << i; random_work(); std::cout << i<<endl;
        std::cout << "From Thread ID : "<<std::this_thread::get_id() << "\n"<<endl;
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
