# Basic Tools
Belong to ``namespace lolita`` , some basic functions.

# Demo
```C++
#include "include/lolita.h"
#include <iostream>
#include <cstdio>

using namespace lolita;

int main()
{
	Image mat;
	Bmp::read(mat,"24.bmp");
	Image backup = mat;
	averageBlur(mat, 3);
	Bmp::write(mat,"blur.bmp");
	detectEdge(backup);
	Bmp::write(backup,"edge.bmp");
}
```
![24](res/24.bmp)
![blur](res/blur.bmp)
![edge](res/edge.bmp)

## Gray Scale
```C++
void grayScale(Image& mat);
```
Convert a image to be a gray-scale image.
* ``mat`` - Image to be converted.

## Binaryzation
```C++
void binaryzation(Image& mat, uint8_t threshold = 0);
```
Convert a gray-scale image to be a binaryzation image.
* ``mat`` - Image to be converted , must be a gray-scale image.
* ``threshold`` - Threshold to convert image.
  * Convert ``[0,threshold)`` to be black.
  * Convert ``[threshold,255]`` to be white.
  * If ``threshold`` is 0 , this function will calculate a threshold by ``Kittler`` .

## Convolution
```C++
bool convolution(Image& mat, Mat<double>& kernel);
```
Filter image by convolution.
* ``mat`` - Image to fileter
* ``kernel`` -- convolution kernel

## DetectEdge
```C++
void detectEdge(Image& mat);
```
Detect the dege of image.
* ``mat`` - Image to detect edge.

## Blur
```C++
void averageBlur(Image& mat, uint32_t radius);
```
Blur image by average.
* ``mat`` - Image to blur.
* ``radius`` - Radius of convolution kernel.

