#include "pixel.h"

namespace lolita
{

Pixel::Pixel()
{
    alpha = 0;
    red   = 0;
    green = 0;
    blue  = 0;
}


Pixel::Pixel(uint32_t ABGR)
{
    this->alpha = (uint8_t)(ABGR >> 24);
    this->blue  = (uint8_t)(ABGR >> 16);
    this->green = (uint8_t)(ABGR >> 8);
    this->red   = (uint8_t)(ABGR);
}



Pixel::operator uint32_t()
{
    return 
    (((uint32_t)(this->alpha)) << 24) |
    (((uint32_t)(this->blue))   << 16) |
    (((uint32_t)(this->green)) <<  8) |
    ((uint32_t)(this->red));
}


Pixel& Pixel::operator = (uint32_t ABGR)
{
    this->alpha = (uint8_t)(ABGR >> 24);
    this->blue  = (uint8_t)(ABGR >> 16);
    this->green = (uint8_t)(ABGR >> 8);
    this->red   = (uint8_t)(ABGR);

    return *this;
}

/* static member */
Pixel Pixel::RGB(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    Pixel pix;
    pix.red = red;
    pix.green = green;
    pix.blue = blue;
    pix.alpha = alpha;

    return pix;
}


}; // namespace lolita