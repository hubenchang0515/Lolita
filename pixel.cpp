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


Pixel::Pixel(uint32_t rgb)
{
    this->alpha = 0;
    this->red   = (uint8_t)(rgb >> 16);
    this->green = (uint8_t)(rgb >> 8);
    this->blue  = (uint8_t)rgb;
}



Pixel::operator uint32_t()
{
    return 
    (((uint32_t)(this->alpha)) << 24) |
    (((uint32_t)(this->red))   << 16) |
    (((uint32_t)(this->green)) <<  8) |
    ((uint32_t)(this->blue));
}


Pixel& Pixel::operator = (uint32_t rgb)
{
    this->alpha = 0;
    this->red   = (uint8_t)(rgb >> 16);
    this->green = (uint8_t)(rgb >> 8);
    this->blue  = (uint8_t)rgb;

    return *this;
}

/* static member */
Pixel Pixel::rgb(uint8_t red, uint8_t green, uint8_t blue)
{
    Pixel pix;
    pix.red = red;
    pix.green = green;
    pix.blue = blue;

    return pix;
}


}; // namespace lolita