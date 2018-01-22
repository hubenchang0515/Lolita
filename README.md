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
	Image mat;
	Bmp::read(mat, "24.bmp");
	Bmp::write(mat, "16.bmp", 16);
	grayScale(mat);
	Bmp::write(mat, "8.bmp", 8);
	binaryzation(mat);
	Bmp::write(mat, "1.bmp", 1);
	return 0;
}
```

![24](doc/res/24.bmp)
![16](doc/res/16.bmp)
![8](doc/res/8.bmp)
![1](doc/res/1.bmp)


## Document
* [RGBA Pixel](doc/Pixel.md)
* [Matrix and Image](doc/Mat.md)  
* [Bmp File IO](doc/Bmp.md)  
* [Basic Tools](doc/Tools.md)