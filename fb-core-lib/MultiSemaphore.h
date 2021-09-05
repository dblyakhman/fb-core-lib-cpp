#ifndef MULTISEMAPHORE_H
#define MULTISEMAPHORE_H

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <stdio.h> 

class MultiSemaphore {
private:
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<int> events;
    
public:

    MultiSemaphore() {
        events = 0;
    }

    inline void notify() {        
        cv.notify_one();
    }    

    void take() {
        //std::unique_lock<std::mutex> lock(mtx);
        events++;
        //std::unique_lock<std::mutex> unlock(mtx);
        notify();
    }

    void release() {
        //std::unique_lock<std::mutex> lock(mtx);
        //if (events.load() > 0) {
            events--;
            notify();
        //}
        //std::unique_lock<std::mutex> unlock(mtx);
    }
    
    inline void waiting() {
        std::unique_lock<std::mutex> lock(mtx);        
        while (events.load() == 0) {            
            cv.wait(lock);            
        }                
    }

//    void waiting() {
//        while (events.get() <= 0) {
//            wait();
//        }
//    }

    void add(int delta) {
        //std::unique_lock<std::mutex> lock(mtx);
        events.store( events.load() + delta );
        //if (events.load() < 0) {
        //    events = 0;
        //}
        //std::unique_lock<std::mutex> unlock(mtx);
    }

    int get() {
        return events.load();
    }
};

extern MultiSemaphore eventSemaphore;

#endif /* MULTISEMAPHORE_H */

