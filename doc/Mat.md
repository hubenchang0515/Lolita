# class Mat
Matrix template , belong to ``namespace lolita``.

# class Image
``using Image = Mat<Pixel>;`` , belong to ``namespace lolita``.


```C++
template<typename ElemType>
class Mat
{
public:
    ~Mat() = default;
    Mat(const Mat&) = default;
    Mat(Mat&&) = default;

    Mat(uint32_t width = 0, uint32_t height = 0);

    uint32_t width();
    uint32_t height();

    std::vector<ElemType>& operator [] (uint32_t raw)
    {
    	return mat[raw];
    }

    void resize(uint32_t width, uint32_t height);

    void map(std::function<void(ElemType&)> callback);
    
    template<typename T>
    T reduce(std::function<T(ElemType&)> callback)
    {
        T result = callback(mat[0][0]);
        for(uint32_t i = 1; i < width_; i++)
        {
            result += callback(mat[0][i]);
        }

        for(uint32_t raw = 1; raw < height_; raw++)
        {
            for(uint32_t i = 0; i < width_; i++)
            {
                result += callback(mat[raw][i]);
            }
        }
        
        return result;
    }

private:
    std::vector< std::vector<ElemType> > mat;
    uint32_t width_;
    uint32_t height_;
};

using Image = Mat<Pixel>;
```

## Public Functions
* [~Mat()](#1)  
* [Mat(const Mat&)](#2)  
* [Mat(Mat&&)](#3)  
* [Mat(uint32_t width = 0, uint32_t height = 0)](#4)  
* [void resize(uint32_t width, uint32_t height)](#5)  
* [void map(std::function<void(ElemType&)> callback)](#6)  
* [T reduce(std::function<T(ElemType&)> callback)](#7)

<span id="1"><span>
### ~Mat()
Destory.

<span id="2"><span>
### Mat(const Mat&)
Copy-Constuct.

<span id="3"><span>
### Mat(Mat&&)
Move-Construct.

<span id="4"><span>
### Mat(uint32_t width = 0, uint32_t height = 0)
Construct by width and height.

<span id="5"><span>
### void resize(uint32_t width, uint32_t height)
Resize this Mat.

<span id="6"><span>
### void map(std::function<void(ElemType&)> callback)
Invoke callback by every elements of Mat.

<span id="7"><span>
### T reduce(std::function<T(ElemType&)> callback)
Invoke callback by every elements of Mat , and return sum fo callback's return value.

## Demo
```C++
#include <lolita/lolita.h>
#include <iostream>

using namespace lolita;

int main()
{
    /* Create a 10*10 matrix of int */
	Mat<int> mat(10,10);

    /* Let every elements to be 1  */
	mat.map([](int& element)
	{
		element = 1;
	});
	
    /* Calculate sum of double element */
	std::cout << mat.reduce<int>([](int& element)
	{
		return 2*element;
	}) << std::endl;
}
```