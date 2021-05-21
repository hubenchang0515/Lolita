# Lolita

**Lolita** is a header-only Computer Vision template library of C++

## Install

`lolite.hpp` is the only source file. 

You can copy it into your project directly.

Or install it into `/usr/local/include` by `make install`.

## Dependency

* C++11

> **Lolita** does't depends any library, except C++ standard library.

## Usage

Demo:  

```cpp
#include <lolita.hpp>

using namespace lolita;

int main(void)
{
	Mat<Pixel::BGR24> in = BMP::read("in.bmp");
	Mat<Pixel::GrayScale> out;
	MatConvert::GRAY(out, in);
	BMP::write(out, "out.bmp");
	return 0;
}
```