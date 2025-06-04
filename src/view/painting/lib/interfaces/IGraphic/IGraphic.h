#ifndef IGRAPHICS_H
#define IGRAPHICS_H


class ITile;

class IGraphic{
public:
    ~IGraphic() = default;
    virtual void draw(ITile& tile) = 0;
};

#endif