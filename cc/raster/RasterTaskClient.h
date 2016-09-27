#ifndef RasterTaskClient_h
#define RasterTaskClient_h

namespace cc {

class RasterTaskClient {
public:
    virtual void rasterTaskFinish() = 0;
    virtual bool willBeDelete() = 0;
};

}

#endif // RasterTaskClient_h