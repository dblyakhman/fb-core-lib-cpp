#include "FuncBlock.h"
#include "FBExecutor.h"

FuncBlock* pFb;
atomic<bool> isStopSystem;

void* threadTaskFunc( void* p ){ ( ( FBExecutor* ) p )->run(); }

void FBExecutor::add(FuncBlock* fb) {
    if (!(funcBlocks.find(fb->getName()) == funcBlocks.end())) {
        return;
    }

    funcBlocks.insert(make_pair(fb->getName(), fb));
}

FuncBlock* FBExecutor::get(string name) {
    FuncBlock* fb = NULL;
    unordered_map<string, FuncBlock*>::iterator itr;
    itr = funcBlocks.find(name);

    if (!(itr == funcBlocks.end())) {
        fb = itr->second;
        if (fb->isMarkedForDelete()) {
            fb = NULL;
        }
    }

    return fb;
}

void FBExecutor::markForDelete(string name) {
    FuncBlock* f = get(name);

    if (f != NULL) {
        f->markForDelete();
    }
}

void FBExecutor::offer(string name, FBEvent* event) {
    FuncBlock* f = get(name);

    if (f != NULL) {
        f->offer(event);
    }
}

void FBExecutor::put(string name, FBEvent* event) {
    FuncBlock* f = get(name);

    if (f != NULL) {
        f->put(event);
    }
}

FBTimer* FBExecutor::startTimer(string name, FBTimerType type, FBEvent* callbackEvent, int expires) {
    FuncBlock* f = get(name);

    if (f != NULL) {
        FBTimer* timer = new FBTimer(name, type, callbackEvent, expires);
        timers.insert(make_pair(timer, timer));
        return timer;
    }

    return NULL;
}

void FBExecutor::deleteTimer(FBTimer* timer) {
    unordered_map<FBTimer*, FBTimer*>::iterator itr;
    itr = timers.find(timer);

    if (!(itr == timers.end())) {
        timer->setMarkedForDelete(true);
    }
}

void FBExecutor::calculate() {
    set<FBTimer*> markedFromDelete;

    for (auto t : timers) {
        FBTimer* timer = t.second;
        if (timer->isMarkedForDelete()) {
            markedFromDelete.insert(timer);
            continue;
        }

        long currentTimeMillis;
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        currentTimeMillis = ts.tv_sec * 1000 + (ts.tv_nsec / 1000000);

        if ((currentTimeMillis - timer->getStartTime()) > timer->getExpires()) {
            FuncBlock* f = get(timer->getName());
            if (f != NULL) {
                f->put(timer->getCallbackEvent());
            } else {
                delete timer->getCallbackEvent();
                markedFromDelete.insert(timer);
                continue;
            }

            timer->setStartTime(currentTimeMillis);
            if (timer->getType() == ONE_TIME) {
                markedFromDelete.insert(timer);
            }
        }
    }

    for (auto t : markedFromDelete) {
        timers.erase(t);
        delete t;
    }
}
