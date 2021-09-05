#ifndef MMAP_H
#define MMAP_H

#include <string.h> 

using namespace std;

#include "fb-core-lib/FbCoreLib.h"

class MMap : public FuncBlock
{
public:
    unsigned int num;
    int i;
    int timeStamp;

    MMap( FBExecutor* executor, string name, unsigned int num_ ) : FuncBlock( executor, name, 1000 ) {
        num = num_;
        i = 0;
        timeStamp = 0;
        //Start();
    }

    void task( FBEvent* event );
};

#endif /* MMAP_H */

