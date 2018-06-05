#include "pixel.h"
#include <cmath>

namespace lolita
{

RgbPixel::RgbPixel()
{
    alpha = 0;
    red   = 0;
    green = 0;
    blue  = 0;
}


RgbPixel::RgbPixel(uint32_t ABGR)
{
    this->alpha = (uint8_t)(ABGR >> 24);
    this->blue  = (uint8_t)(ABGR >> 16);
    this->green = (uint8_t)(ABGR >> 8);
    this->red   = (uint8_t)(ABGR);
}



RgbPixel::operator uint32_t()
{
    return 
    (((uint32_t)(this->alpha)) << 24) |
    (((uint32_t)(this->blue))   << 16) |
    (((uint32_t)(this->green)) <<  8) |
    ((uint32_t)(this->red));
}


RgbPixel& RgbPixel::operator = (uint32_t ABGR)
{
    this->alpha = (uint8_t)(ABGR >> 24);
    this->blue  = (uint8_t)(ABGR >> 16);
    this->green = (uint8_t)(ABGR >> 8);
    this->red   = (uint8_t)(ABGR);

    return *this;
}

/* static member */
RgbPixel RgbPixel::RGB(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    RgbPixel pix;
    pix.red = red;
    pix.green = green;
    pix.blue = blue;
    pix.alpha = alpha;

    return pix;
}


HsvPixel rgb2hsv(RgbPixel color)
{
    HsvPixel p;
    p.value = maximum(color.red, color.green, color.blue);
    int16_t min = minimum(color.red, color.green, color.blue);
    int16_t delta = p.value - min;
    if(p.value == 0)
    {
        p.saturation = 0;
        p.hue = 0;
    }
    else
    {
        p.saturation = 255 - 255 * minimum(color.red, color.green, color.blue) / p.value;

        if(p.value == min)
            p.hue = 0;
        else if(p.value == color.red && color.green >= color.blue)
            p.hue = 60 * (color.green - color.blue) / delta;
        else if(p.value == color.red && color.green < color.blue)
            p.hue = 60 * (color.green - color.blue) / delta + 360;
        else if(p.value == color.green)
            p.hue = 60 * (color.blue - color.red) / delta + 120;
        else
            p.hue = 60 * (color.red - color.green) / delta + 240;
    }


    return p;

}


RgbPixel hsv2rgb(HsvPixel color)
{
    double H = color.hue;
    double V = color.value / 255.0;
    double S = color.saturation / 255.0; 


    double C = V * S;
    double X = C * (1 - fabs( fmod(H / 60 , 2) - 1));
    double m = V - C;

    double r=0,g=0,b=0;
    switch(int(H / 60))
    {
    case 0 :
        r = C;
        g = X;
        break;

    case 1 :
        r = X;
        g = C;
        break;

    case 2 :
        g = C;
        b = X;
        break;

    case 3 :
        g = X;
        b = C;
        break;

    case 4 :
        r = X;
        b = C;
        break;

    case 5 :
        r = C;
        b = X;
        break;
    }

    RgbPixel p;
    p.red = (r + m) * 255;
    p.green = (g + m) * 255;
    p.blue = (b + m) * 255;

    return p;
}


uintmax_t distance(HsvPixel p1, HsvPixel p2)
{
    static const double pi = 3.1415926;
    int32_t x1,y1,z1;
    int32_t x2,y2,z2;

    x1 = p1.saturation * cos(p1.hue / 360.0 * pi);
    y1 = p1.saturation * sin(p1.hue / 360.0 * pi);
    z1 = p1.value;

    x2 = p2.saturation * cos(p2.hue / 360.0 * pi);
    y2 = p2.saturation * sin(p2.hue / 360.0 * pi);
    z2 = p2.value;

    return pow(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2), 0.5);
}

}; // namespace lolita