# class Bmp
Bmp file accesser , belong to ``namespace lolita``.

```C++
class Bmp
{
public:
    static bool read(Image& mat, std::string file);
    static bool write(Image& mat, std::string file, uint8_t bits=24);
};
```

## Public Functions
* [static bool read(Image& mat, std::string file)](#1)
* [static bool write(Image& mat, std::string file, uint8_t bits=24)](#2)

<span id="1"><span>
### static bool read(Image& mat, std::string file)
Read file into mat , Convert to 24 bits color automatically.

<span id="2"><span>
### static bool write(Image& mat, std::string file, uint8_t bits=24)
Write mat into file.  
* ``bits = 24`` , 24 bits color image , DEFAULT.   
* ``bits = 16`` , 16 bits color image , convert automatically.  
* ``bits = 8`` , 8 bits color image , only for gray-scale image.  
* ``bits = 1`` , 1 bit color image , only for binary image.  
* ``bits = 32`` , 32 bit color image with alpha channel . 
  * most picture shower will ignore alpha channel of BMP file.  
  * ``Eye of gnome`` doesn't ignore alpha channel of BMP file.