#include "RW_monitor.hpp"

RW_Monitor::RW_Monitor(): num_readers(0), num_writers(0){
    pthread_cond_init( &ok_read, NULL);
    pthread_cond_init( &ok_write, NULL);
    pthread_mutex_init( &lock, NULL);
}

void RW_Monitor::requestRead(){
    // Wait until there are no more writers
    while(this->num_writers > 0) pthread_cond_wait(&ok_read, &lock);
    num_readers++;
}

void RW_Monitor::requestWrite(){
    // Wait until there are no more readers or writers
    while(this->num_writers > 0 || this->num_readers > 0) pthread_cond_wait(&ok_write, &lock);
    num_writers++;
}

void RW_Monitor::releaseRead(){
    num_readers--;
    // If no readers are left, notify the writer thread
    if (num_readers == 0)
    {
        pthread_cond_signal(&ok_write);
    }
}

void RW_Monitor::releaseWrite(){
    num_writers--;
    // Signal all reader threads
    pthread_cond_broadcast(&ok_read);
    // Signal any writer thread
    pthread_cond_signal(&ok_write);
}

 