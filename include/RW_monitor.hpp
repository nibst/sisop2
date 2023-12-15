#include <iostream>
#include <atomic>
#include <pthread.h>

class RW_Monitor{
public:
    RW_Monitor();
    void requestRead();
    void requestWrite();
    void releaseRead();
    void releaseWrite();
private:
    std::atomic<int> num_readers, num_writers;  // Current number of readers and writers
    pthread_cond_t ok_read, ok_write;           // Condition variables for reading and writing
    pthread_mutex_t lock; 
};