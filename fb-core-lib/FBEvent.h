#ifndef FBEVENT_H
#define FBEVENT_H

class FBEvent {
};

// ...........................................................................................
// ... Class EventStruct .....................................................................
// ...........................................................................................

class FbMsg : public FBEvent {
public:

    FbMsg(int Event_, void* pData_ = NULL) {
        event = Event_;
        pData = pData_;
    }

    ~FbMsg() {
    };
    int event;
    void* pData;
};

#endif /* FBEVENT_H */

