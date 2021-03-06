#ifndef MLBARRIER
#define MLBARRIER

#include <iostream> 
#include <mutex> 
#include <atomic> 
#include <thread>

class BarrierWait
{

    private:
        std::atomic<int> passed_barrier;
        std::atomic<int> iteration_number;
        int passed_barrier_reset_value;
        std::mutex update_value;

    public:
        BarrierWait(){};
        BarrierWait(const BarrierWait &) = delete;
        BarrierWait &operator=(const BarrierWait &) = delete;
        explicit BarrierWait(int count) : passed_barrier(0), iteration_number(0),
                                                passed_barrier_reset_value(count)
        {      
        }     

        void setCount(int count) {
            passed_barrier_reset_value = count;
        }

        int getCount()
        {
            return passed_barrier_reset_value;
        }

        void decrease_active_threads()
        {
            update_value.lock();
            passed_barrier_reset_value = (passed_barrier_reset_value == 0 ? 0 : passed_barrier_reset_value - 1);
            int gen = iteration_number.load();
            bool passed = passed_barrier >= passed_barrier_reset_value;
            update_value.unlock();
            if (passed) 
            {
                if(iteration_number.compare_exchange_weak(gen, gen + 1)) {
                    passed_barrier = 0;
                }
                return;
            }
        }

        void count_down_and_wait()
        {
            update_value.lock();
            int gen = iteration_number.load();
            bool passed = ++passed_barrier >= passed_barrier_reset_value;
            update_value.unlock();
            if (passed)
            {
                if (iteration_number.compare_exchange_weak(gen, gen + 1))
                {
                    passed_barrier = 0;
                }
                return;
            }
            while((gen== iteration_number)&&(passed_barrier==passed_barrier_reset_value))
                std::this_thread::yield();  
        }                                                                                  
};

#endif