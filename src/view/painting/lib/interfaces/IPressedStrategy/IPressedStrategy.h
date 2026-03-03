#ifndef IPRESSEDSTRATEGY_H
#define IPRESSEDSTRATEGY_H

class IPressedStrategy{
protected:
public:
    virtual ~IPressedStrategy() = default;
    virtual void onPressed(int x, int y) = 0;
    virtual void onTracking(int x, int y) = 0;
    virtual void onRelease(int x, int y) = 0;
};

#endif
