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
	Image image;
	Bmp::read(image, "24.bmp");
	
	/* reduce color space */
	Bmp::write(image, "16.bmp", 16);
	
	
	/* graying */
	image.map([](Pixel& pix)
	{
		pix.red = pix.green  = pix.blue = 0.30 * pix.red + 0.59 * pix.green + 0.11 * pix.blue; 
	});
	
	/* save gray scale image as 8 bit */
	Bmp::write(image, "8.bmp", 8);
	
	
	/* binaryzation */
	uint64_t histogram[256] = { 0 };
	image.map([&histogram](Pixel& pix)
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
	
	image.map([threshold](Pixel& pix)
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

	/* save binary image as 1 bit */
	Bmp::write(image, "1.bmp", 1);

}
```
