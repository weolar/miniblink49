#ifndef mc_RasterTaskClient_h
#define mc_RasterTaskClient_h

namespace mc {

class RasterTaskClient {
public:
    virtual void rasterTaskFinish() = 0;
    virtual bool willBeDelete() = 0;
};

}

#endif // RasterTaskClient_h