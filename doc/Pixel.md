# class Pixel
Belong to ``namespace lolita`` , used to save color of pixel.
```C++
class Pixel
{
public:

    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha; 

    ~Pixel() = default;
    Pixel(const Pixel& other) = default;
    Pixel(Pixel&& other) = default;
    
    Pixel();
    Pixel(uint32_t ABGR);

    operator uint32_t();
    Pixel& operator = (uint32_t ABGR);
    Pixel& operator = (const Pixel& other) = default;
    Pixel& operator = (Pixel&& other) = default;

    static Pixel RGB(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha=0);
};
```

## Public Properties
* ``uint8_t`` red;
* ``uint8_t`` green;
* ``uint8_t`` blue;
* ``uint8_t`` alpha;

## Public Functions
* [~Pixel()](#1)  
* [Pixel()](#2) 
* [Pixel(uint32_t ABGR)](#3) 
* [Pixel(const Pixel& other)](#4)  
* [Pixel(Pixel&& other)](#5)  
* [operator uint32_t()](#6)
* [Pixel& operator = (uint32_t ABGR)](#7)  
* [Pixel& operator = (const Pixel& other)](#8)  
* [Pixel& operator = (Pixel&& other)](#9)  
* [static Pixel RGB(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha=0)](#10)


<span id="1"><span>
### ~Pixel()
Destory the Pixel.

<span id="2"><span>
### Pixel()
Constructs a Pixel , red green blue and alpha are 0.

<span id="3"><span>
### Pixel(uint32_t ABGR)
Constructs a Pixel by ABGR(``8bitAlpha-8bitBlue-8bitGrenn-8bitRed``)

<span id=4"><span>
### Pixel(const Pixel& other)
Constructs a copy of other.

<span id="5"><span>
### Pixel(Pixel&& other)
Move-constructs a Pixel.

<span id="6"><span>
### operator uint32_t()
Convert to uint32_t as ABGR(``8bitAlpha-8bitBlue-8bitGrenn-8bitRed``)

<span id="7"><span>
### Pixel& operator = (uint32_t ABGR)
Assigned by ABGR(``8bitAlpha-8bitBlue-8bitGrenn-8bitRed``)

<span id="8"><span>
### Pixel& operator = (const Pixel& other)
Assigned by other

<span id="9"><span>
### Pixel& operator = (Pixel&& other)
Move-Assigment

<span id="10"><span>
### ``static`` Pixel RGB(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha=0)
Create a Pixel by RGB(A) channels


