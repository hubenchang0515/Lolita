# Basic Tools
Belong to ``namespace lolita`` , some basic functions.

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