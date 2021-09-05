#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 
#include <pthread.h>
#include <thread>
#include <set>
#include <unordered_map>

#include <cstdlib>
#include <iostream>
#include <utility>

#include "fb-core-lib/FbCoreLib.h"
#include "FbEvents.h"
#include "mmap.h"

MMap* pMMap1;
MMap* pMMap2;

int main(int argc, char** argv) {

    MultiThreadFBExecutor executor(2);

    pMMap1 = new MMap(&executor, "MMap1", 1);
    pMMap2 = new MMap(&executor, "MMap2", 2);

    executor.add(pMMap1);
    executor.add(pMMap2);

    executor.startTimer("MMap1", ONE_TIME, new FbMsg(START), 1000);

    usleep(10000 * 1000);
        
    executor.stopSystem();
        
    usleep(5000 * 1000);
        
    executor.releaseSystem();    

    while (1) {        
        usleep(10000 * 1000);        
    }
    return 0;
}