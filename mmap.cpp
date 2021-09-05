#include <unistd.h> 

#include "mmap.h"

extern MMap* pMMap1;
extern MMap* pMMap2;

void MMap::task(FBEvent* event) {
    if (num == 1) executor->put("MMap2", event);
    if (num == 2) executor->put("MMap1", event);

    if (num == 1) {

        struct timespec ts;

        clock_gettime(CLOCK_REALTIME, &ts);

        if (ts.tv_sec - timeStamp > 1) {
            timeStamp = ts.tv_sec;
            printf("Events count per sec: %i\n", i);
            i = 0;
        }
        i++;
    }
}
