#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 

#include "FuncBlock.h"

MultiSemaphore eventSemaphore;

// ...........................................................................................

// ... Class FuncBlock .......................................................................
// ...........................................................................................

//void* ThreadTaskFunc(void* p) {
//    ((FuncBlock*) p)->Task();
//}
// ...........................................................................................

FuncBlock::FuncBlock(FBExecutor* executor, std::string name, unsigned mailboxSize ) {    
    this->mailboxSize = mailboxSize;
    
    mailbox = new Mailbox(mailboxSize);    

    this->name = name;    
    
    this->executor = executor;
    
    executeLock = false;
    executeTaskLock = false;
    markedForDelete = false;
}
// ...........................................................................................

void operator>>(int e, FuncBlock& b) {
    FBEvent* event = new FbMsg(e);
    b.put(event);
}
// ........................................................................................... 
