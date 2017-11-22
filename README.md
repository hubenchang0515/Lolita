# Lolita
Lolita is a Computer Vision library of C++

## Warning
* Need C++11
* Doesn't Ready

## Demo
```C++
#include <lolita/lolita.h>

using namespace lolita;

int main()
{
	Image Image;
	Bmp::read(Image, "color.bmp");
	Image.map([](Pixel& pix)
	{
		pix.red = pix.green  = pix.blue = 0.30 * pix.red + 0.59 * pix.green + 0.11 * pix.blue; 
	});
	Bmp::write(Image, "gray.bmp");
	
	uint64_t histogram[256] = { 0 };
	Image.map([&histogram](Pixel& pix)
	{
		histogram[pix.red] += 1;
	});
	
	uint16_t summit1st = 0,summit2nd = 0;
	for(uint16_t i = 0; i < 256; i++)
	{
		if(histogram[i] > summit1st)
		{
			summit1st = i;
		}
		else if(histogram[i] > summit2nd)
		{
			summit2nd = i;
		}
	}
	
	uint8_t threshold = (summit1st + summit2nd) / 2;
	
	Image.map([threshold](Pixel& pix)
	{
		if(pix.red >= threshold)
		{
			pix = 0xffffff;
		}
		else
		{
			pix = 0;
		}
	});
	Bmp::write(Image, "bin.bmp");
}
```
