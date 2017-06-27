#include "gl/Viewport.h"

Viewport::Shared Viewport::alloc(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    return std::make_shared<Viewport>(x, y, width, height);
}

Viewport::Viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    : x(x)
    , y(y)
    , width(width)
    , height(height)
{
}

void Viewport::resize(uint32_t nx, uint32_t ny, uint32_t nwidth, uint32_t nheight)
{
    x = nx;
    y = ny;
    width = nwidth;
    height = nheight;
}

bool operator==(const Viewport::Shared& a, const Viewport::Shared& b)
{
    return a->x == b->x && a->y == b->y && a->width == b->width && a->height == b->height;
}

bool operator!=(const Viewport::Shared& a, const Viewport::Shared& b)
{
    return !(a == b);
}
