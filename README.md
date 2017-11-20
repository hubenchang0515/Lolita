# Lolita
Lolita is a Computer Vision library of C++

## Warning
* Doesn't Ready

## Demo
```C++
#include <iostream>
#include "mat.h"
#include "bmp.h"

using namespace lolita;

int main()
{
    Mat mat;
    Bmp::read(mat,"color.bmp");
    
    /* Gray Scale */
    mat.map([](Pixel& pix)
    {
        pix.red = pix.green = pix.blue = (0.30 * pix.red + 0.59 * pix.green + 0.11 * pix.blue);
    });
    Bmp::write(mat,"gray.bmp");

    /* Binaryzation */
    uint64_t sum = mat.reduce<uint64_t>([](Pixel& pix)
    {
        return pix.red;
    });
    uint8_t average = sum / mat.width()/mat.height();
    mat.map([average](Pixel& pix)
    {
        if(pix.red >= average)
        {
            pix.red = pix.green = pix.blue = 0xff;
        }
        else
        {
            pix.red = pix.green = pix.blue = 0;
        }
    });
    Bmp::write(mat,"bin.bmp");

    return 0; 
}
```
