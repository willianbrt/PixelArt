#ifndef ITOOLCONTEXT_H
#define ITOOLCONTEXT_H

class IToolContext{
protected:
public:
    virtual ~IToolContext() = default;
    
    bool isMirrorX;
    bool isMirrorY;
    int nTileX = 1;
    int nTileY = 1;
};

#endif
