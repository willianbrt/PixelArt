#ifndef ITRACE_H
#define ITRACE_H

class ITrace{
public:
    virtual ~ITrace() = default;
    virtual void trace(int x, int y, int cx, int cy) = 0;
};
#endif