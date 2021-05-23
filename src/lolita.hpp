/************************************************************
 * @file lolita.hpp
 * @brief header-only computer vision template library
 * @author https://github.com/hubenchang0515
 * @date 2021-05-20
 * @version 0.2
 ************************************************************/

#ifndef LOLITA_HPP
#define LOLITA_HPP

#include <cstdio>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <vector>
#include <map>
#include <functional>
#include <type_traits>

#if __cplusplus < 201103L
    #error "please add the compiling option '-std=c++11' or higher"
#endif

namespace lolita
{
    namespace utils
    {
        /************************************************************
        * @brief request the compiler calculate a constexpr while compiling
        *        like the key work 'consteval' in C++20
        * @param[in] T the return type
        * @param[in] expr a const expression
        * @return the value of expr
        ************************************************************/
        template<typename T, T expr>
        struct eval
        {
            static constexpr const T value = expr;
        };

        /************************************************************
        * @brief calculate the padding length of alignment 
        * @param[in] len   data length
        * @param[in] align alignment bytes
        * @return the padding length
        ************************************************************/
        constexpr static inline size_t padding(size_t len, size_t align)
        {
            return (align - (len % align)) % align;
        }

        /************************************************************
        * @brief let n.x -> n+1
        * @param[in] x the dividend
        * @param[in] y the divisor
        * @return the padding length
        ************************************************************/
        template<typename T>
        typename std::enable_if<std::is_integral<T>::value, T>::type
        constexpr static inline ceil(T x, T y)
        {
            return x % y > 0 ? x / y + 1 : x / y;
        }

        /************************************************************
        * @brief let n.4 -> n and n.5 -> n+! 
        * @param[in] x the dividend
        * @param[in] y the divisor
        * @return the padding length
        ************************************************************/
        template<typename T>
        typename std::enable_if<std::is_integral<T>::value, T>::type
        constexpr static inline round(T x, T y)
        {
            return (y > (x % y * 2)) ? (x / y) : (x / y + 1);
        }

    }; // ::lolita::utils

    namespace _private
    {
        /************************************************************
        * @brief calculate the length of string 
        * @param[in] str a C-style string
        * @return length of str
        ************************************************************/
        constexpr static inline size_t _stringLength(const char* str)
        {
            return *str != 0 ? 1 + _stringLength(str + 1) : 0;
        }

        /************************************************************
        * @brief find the index of the firsct char in the string 
        * @param[in] str a C-style string
        * @param[in] ch  the char to be found
        * @param[in] index the start point
        * @return index of ch in str
        ************************************************************/
        constexpr static inline size_t _stringFind(const char* str, char ch, size_t index=0)
        {
            return *str == ch ? index : (*str == 0 ? SIZE_MAX : _stringFind(str+1, ch, index+1));
        }

        /************************************************************
        * @brief decide if two string equal 
        * @param[in] str1 a C-style string
        * @param[in] str2 a C-style string
        * @return if queal
        ************************************************************/
        constexpr static inline bool _stringEqual(const char* str1, const char* str2)
        {
            return *str1 == 0 && *str2 == 0 ? true : (*str1 != *str2 ? false : (_stringEqual(str1+1, str2+1))); 
        }

        /************************************************************
        * @brief combine a string to be an uint64_t 
        * @param[in] str a C-style string
        * @param[in] n   the initial value
        * @return the combined value
        ************************************************************/
        constexpr static inline uint64_t _string2Uint64(const char* str, uint64_t n = 0, size_t index = 0)
        {
            return *str == 0 ? n : _string2Uint64(str+1, n | (static_cast<uint64_t>(*str) << (8 * index)), index+1);
        }

        /************************************************************
        * @brief find the index of a char in an combined uint64_t value 
        * @param[in] n  a combined uint64_t value
        * @param[in] ch the char to be found
        * @return the index
        ************************************************************/
        constexpr static inline size_t _findCharInUint64(uint64_t n, char ch, size_t index = 0)
        {
            return n == 0 ? SIZE_MAX : ((n & 0xff) == static_cast<uint8_t>(ch) ? index : _findCharInUint64(n >> 8, ch, index+1));
        }

        /************************************************************
        * @brief calculate the length of a combined uint64_t value 
        * @param[in] n  a combined uint64_t value
        * @param[in] len the initial value
        * @return the length
        ************************************************************/
        constexpr static inline size_t _uint64StringLength(uint64_t n, size_t len = 0)
        {
            return n == 0 ? len : _uint64StringLength(n >> 8, len+1);
        }

        /************************************************************
        * @brief check does the combined uint64_t value have the char
        * @param[in] n  a combined uint64_t value
        * @param[in] ch the char
        * @return does have
        ************************************************************/
        constexpr static inline bool _uint64StringHas(uint64_t n, char ch)
        {
            return (n & 0xff) == static_cast<uint64_t>(ch) ? true : (n == 0 ? false : _uint64StringHas(n >> 8, ch));
        }

        /************************************************************
        * @brief check does the combined uint64_t value have all the char in a string
        * @param[in] n  a combined uint64_t value
        * @param[in] str the string
        * @return does have
        ************************************************************/
        constexpr static inline bool _uint64StringHas(uint64_t n, const char* str)
        {
            return *str == 0 ? true : (!_uint64StringHas(n, *str) ? false : _uint64StringHas(n, str+1));
        }
    }; // namespace ::lolita::_private

    namespace ColorSpace
    {
        /* NOTE: These color names must have to be unique */
    
        /* Placeholder */
        constexpr const char PLACEHOLDER = 'X';

        /* Binary Pixel */
        constexpr const char BINARY = '2';

        /* Gray Scale */
        constexpr const char GRAYSCALE = 'Y';

        /* RGB(A) */
        constexpr const char RED   = 'R';
        constexpr const char GREEN = 'G';
        constexpr const char BLUE  = 'B';
        constexpr const char ALPHA = 'A';

        /* HSL */
        constexpr const char HUE        = 'H';
        constexpr const char SATURATION = 'S';
        constexpr const char LIGHTNESS  = 'L';

        /************************************************************
        * @brief get the color space ID 
        * @param[in] str a C-style string
        * @return the color space ID
        ************************************************************/
        constexpr static inline uint64_t ID(const char* str)
        {
            return _private::_string2Uint64(str);
        }

        /************************************************************
        * @brief check is the color space compatible
        * @param[in] id the color space
        * @param[in] str a C-style string
        * @return is similiar
        ************************************************************/
        constexpr static inline bool compatible(uint64_t id, const char* str)
        {
            return _private::_uint64StringHas(id, str);
        }

        /************************************************************
        * @brief get the color index 
        * @param[in] n  color space ID
        * @param[in] ch color
        * @return the color index
        ************************************************************/
        constexpr static inline size_t _index(uint64_t n, char ch)
        {
            return _private::_findCharInUint64(n, ch);
        }

        /************************************************************
        * @brief get the length pf color space 
        * @param[in] n  color space ID
        * @return the length
        ************************************************************/
        constexpr static inline size_t _length(uint64_t n)
        {
            return _private::_uint64StringLength(n);
        }
    }; // namespace ::lolita::ColorSpace

    #pragma pack(push)
    #pragma pack(1)
    namespace Pixel
    {   
        template<uint64_t format>
        class BasicPixel
        {
        public:
            constexpr static const uint64_t id = format;
            constexpr static const size_t length = ColorSpace::_length(format);

            ~BasicPixel() = default;
            BasicPixel(const BasicPixel&) = default;
            BasicPixel(BasicPixel&&) = default;

            /************************************************************
            * @brief init the RGBA pixel by uint64_t value
            * @param[in] n the uint64_t value
            ************************************************************/
            explicit BasicPixel(uint64_t n = 0)
            {
                for(size_t i = 0; i < BasicPixel::length; i++)
                {
                    m_data[i] = (n >> (BasicPixel::length - i - 1)) & 0xff;
                }
            }

            /************************************************************
            * @brief static cast to uint64_t
            * @return uint64_t value
            ************************************************************/
            operator uint64_t() const
            {
                uint64_t n = 0;
                for(size_t i = 0; i < BasicPixel::length; i++)
                {
                    n = (n << 8) | m_data[i];
                }
                return n;
            }

            /************************************************************
            * @brief get the binary value
            * @return the binary value
            ************************************************************/
            bool binary() const
            {
                return m_get<ColorSpace::BINARY>() != 0;
            }

            /************************************************************
            * @brief set the binary value
            * @param[in] v the binary value
            ************************************************************/
            void setBinary(bool v)
            {
                m_set<ColorSpace::BINARY>(v ? 1 : 0);
            }

            /************************************************************
            * @brief get the gray scale value
            * @return the gray scale value
            ************************************************************/
            uint8_t grayScale() const
            {
                return m_get<ColorSpace::GRAYSCALE>();
            }

            /************************************************************
            * @brief set the gray scale value
            * @param[in] v the gray scale value
            ************************************************************/
            void setGrayScale(uint8_t v)
            {
                m_set<ColorSpace::GRAYSCALE>(v);
            }

            /************************************************************
            * @brief get the red component value
            * @return the red component value
            ************************************************************/
            uint8_t red() const
            {
                return m_get<ColorSpace::RED>();
            }

            /************************************************************
            * @brief get the green component value
            * @return the green component value
            ************************************************************/
            uint8_t green() const
            {
                return m_get<ColorSpace::GREEN>();
            }

            /************************************************************
            * @brief get the blue component value
            * @return the blue component value
            ************************************************************/
            uint8_t blue() const
            {
                return m_get<ColorSpace::BLUE>();
            }

            /************************************************************
            * @brief get the alpha component value
            * @return the alpha component value
            ************************************************************/
            uint8_t alpha() const
            {
                return m_get<ColorSpace::ALPHA>();
            }

            /************************************************************
            * @brief set the red component value
            * @param[in] v the red component value
            ************************************************************/
            void setRed(uint8_t v)
            {
                m_set<ColorSpace::RED>(v);
            }

            /************************************************************
            * @brief set the green component value
            * @param[in] v the green component value
            ************************************************************/
            void setGreen(uint8_t v)
            {
                m_set<ColorSpace::GREEN>(v);
            }

            /************************************************************
            * @brief set the blue component value
            * @param[in] v the blue component value
            ************************************************************/
            void setBlue(uint8_t v)
            {
                m_set<ColorSpace::BLUE>(v);
            }

            /************************************************************
            * @brief set the alpha component value
            * @param[in] v the alpha component value
            ************************************************************/
            void setAlpha(uint8_t v)
            {
                m_set<ColorSpace::ALPHA>(v);
            }

            /************************************************************
            * @brief get the hue value (HSL)
            * @return the hue value
            ************************************************************/
            uint8_t hue() const
            {
                return m_get<ColorSpace::HUE>();
            }

            /************************************************************
            * @brief get the saturation value (HSL)
            * @return the saturation value
            ************************************************************/
            uint8_t saturation() const
            {
                return m_get<ColorSpace::SATURATION>();
            }

            /************************************************************
            * @brief get the lightness value (HSL)
            * @return the lightness value
            ************************************************************/
            uint8_t lightness() const
            {
                return m_get<ColorSpace::LIGHTNESS>();
            }

            /************************************************************
            * @brief set the hue value (HLS)
            * @param[in] v the hue value (HLS)
            ************************************************************/
            void setHue(uint8_t v)
            {
                m_set<ColorSpace::HUE>(v);
            }

            /************************************************************
            * @brief set the saturation value (HLS)
            * @param[in] v the saturation value (HLS)
            ************************************************************/
            void setSaturation(uint8_t v)
            {
                m_set<ColorSpace::SATURATION>(v);
            }

            /************************************************************
            * @brief set the lightness value (HLS)
            * @param[in] v the lightness value (HLS)
            ************************************************************/
            void setLightness(uint8_t v)
            {
                m_set<ColorSpace::LIGHTNESS>(v);
            }
            

        private:
            uint8_t m_data[ColorSpace::_length(format)];
            
            /************************************************************
            * @brief get a color component value
            * @param[in] color the color component name
            * @return the color component value
            ************************************************************/
            template<char color>
            uint8_t m_get() const
            {
                static char message[256];
                constexpr size_t i = ColorSpace::_index(format, color);
                if(i >= length)
                {
                    // NOTE: Do not implement any logic by catching this exception.
                    //       That will reduce the performance very much.
                    //       Because this exception will throw very many times in matrix
                    sprintf(message, "the pixel does't have the %c color", color);
                    throw std::out_of_range(message);
                }
                return m_data[i];
            }

            /************************************************************
            * @brief get a color component value
            * @param[in] color the color component name
            * @param[in] v the color component value
            ************************************************************/
            template<char color>
            void m_set(uint8_t v)
            {
                static char message[256];
                constexpr size_t i = ColorSpace::_index(format, color);
                if(i >= length)
                {
                    // NOTE: Do not implement any logic by catching this exception.
                    //       That will reduce the performance very much.
                    //       Because this exception will throw very many times in matrix
                    sprintf(message, "the pixel does't have the %c color", color);
                    throw std::out_of_range(message);
                }
                m_data[i] = v;
            }
        }; // class BasicPixel

        using Binary = BasicPixel<ColorSpace::ID("2")>;

        using GrayScale = BasicPixel<ColorSpace::ID("Y")>;

        using RGB24 = BasicPixel<ColorSpace::ID("RGB")>;
        using RBG24 = BasicPixel<ColorSpace::ID("RBG")>;
        using GRB24 = BasicPixel<ColorSpace::ID("GRB")>;
        using GBR24 = BasicPixel<ColorSpace::ID("GBR")>;
        using BRG24 = BasicPixel<ColorSpace::ID("BRG")>;
        using BGR24 = BasicPixel<ColorSpace::ID("BGR")>;

        using RGBX32 = BasicPixel<ColorSpace::ID("RGBX")>;
        using RBGX32 = BasicPixel<ColorSpace::ID("RBGX")>;
        using GRBX32 = BasicPixel<ColorSpace::ID("GRBX")>;
        using GBRX32 = BasicPixel<ColorSpace::ID("GBRX")>;
        using BRGX32 = BasicPixel<ColorSpace::ID("BRGX")>;
        using BGRX32 = BasicPixel<ColorSpace::ID("BGRX")>;

        using XRGB32 = BasicPixel<ColorSpace::ID("XRGB")>;
        using XRBG32 = BasicPixel<ColorSpace::ID("XRBG")>;
        using XGRB32 = BasicPixel<ColorSpace::ID("XGRB")>;
        using XGBR32 = BasicPixel<ColorSpace::ID("XGBR")>;
        using XBRG32 = BasicPixel<ColorSpace::ID("XBRG")>;
        using XBGR32 = BasicPixel<ColorSpace::ID("XBGR")>;

        using RGBA32 = BasicPixel<ColorSpace::ID("RGBA")>;
        using RBGA32 = BasicPixel<ColorSpace::ID("RBGA")>;
        using GRBA32 = BasicPixel<ColorSpace::ID("GRBA")>;
        using GBRA32 = BasicPixel<ColorSpace::ID("GBRA")>;
        using BRGA32 = BasicPixel<ColorSpace::ID("BRGA")>;
        using BGRA32 = BasicPixel<ColorSpace::ID("BGRA")>;

        using ARGB32 = BasicPixel<ColorSpace::ID("ARGB")>;
        using ARBG32 = BasicPixel<ColorSpace::ID("ARBG")>;
        using AGRB32 = BasicPixel<ColorSpace::ID("AGRB")>;
        using AGBR32 = BasicPixel<ColorSpace::ID("AGBR")>;
        using ABRG32 = BasicPixel<ColorSpace::ID("ABRG")>;
        using ABGR32 = BasicPixel<ColorSpace::ID("ABGR")>;

        using HSL24 = BasicPixel<ColorSpace::ID("HSL")>;
        // using HLS24 = BasicPixel<ColorSpace::ID("HLS")>;
        // using SHL24 = BasicPixel<ColorSpace::ID("SHL")>;
        // using SLH24 = BasicPixel<ColorSpace::ID("SLH")>;
        // using LHS24 = BasicPixel<ColorSpace::ID("LHS")>;
        // using LSH24 = BasicPixel<ColorSpace::ID("LSH")>;

    }; // namespace ::lolita::Pixel
    #pragma pack(pop)

    template<typename ElemType>
    class Mat
    {
    public:
        /************************************************************
        * @brief delete the matrix
        ************************************************************/
        ~Mat()
        {
            if(m_data != nullptr)
            {
                free(m_data);
                m_data = nullptr;
            }
        }

        /************************************************************
        * @brief create a matrix with the size and padding
        * @param[in] width the width of the matrix
        * @param[in] height the height of the matrix
        * @param[in] rowPadding the row padding as bytes of the matrix for alignment
        ************************************************************/
        Mat(size_t width = 0, size_t height = 0, size_t rowPadding = 0) :
            m_width(width),
            m_height(height),
            m_rowPadding(rowPadding),
            m_data(nullptr)
        {
            if(size() == 0)
                return;

            void* data = malloc(size());
            if(data == nullptr)
            {
                throw std::bad_alloc();
            }
            memset(data, 0, size());
            this->m_data = reinterpret_cast<ElemType*>(data);
        }

        /************************************************************
        * @brief create a matrix by deep copy
        * @param[in] another the matrix to be copied
        ************************************************************/
        Mat(const Mat& another) :
            m_width(another.m_width),
            m_height(another.m_height),
            m_rowPadding(another.m_rowPadding),
            m_data(nullptr)
        {
            if(size() == 0 || another.m_data == nullptr)
                return;

            void* data = malloc(another.size());
            if(data == nullptr)
            {
                throw std::bad_alloc();
            }
            memcpy(data, another.m_data, another.size());
            this->m_data = reinterpret_cast<ElemType*>(data);
        }

        /************************************************************
        * @brief create a matrix by data moving
        * @param[in] another the matrix to be moved data
        ************************************************************/
        Mat(Mat&& another):
            m_width(another.m_width),
            m_height(another.m_height),
            m_rowPadding(another.m_rowPadding),
            m_data(another.m_data)
        {
            another.m_data = nullptr;
        }

        /************************************************************
        * @brief get if the matrix valid
        * @return is valid
        ************************************************************/
        bool valid() const
        {
            return m_data != nullptr;
        }

        /************************************************************
        * @brief get the data address of matrix
        * @return data address
        ************************************************************/
        ElemType* data()
        {
            return m_data;
        }

        /************************************************************
        * @brief get the data address of matrix
        * @return data address
        ************************************************************/
        const ElemType* data() const
        {
            return m_data;
        }

        /************************************************************
        * @brief get the width of the matrix
        * @return the width
        ************************************************************/
        size_t width() const
        {
            return m_width;
        }

        /************************************************************
        * @brief get the height of the matrix
        * @return the height
        ************************************************************/
        size_t height() const
        {
            return m_height;
        }

        /************************************************************
        * @brief get the row padding of the matrix
        * @return the row padding as bytes
        ************************************************************/
        size_t rowPadding() const
        {
            return m_rowPadding;
        }

        /************************************************************
        * @brief set the row padding of the matrix
        *        element data will keep valid
        * @return is success
        ************************************************************/
        bool setRowPadding(uint8_t rowPadding)
        {
            if(rowPadding == m_rowPadding)
            {
                return true;
            }

            if(m_data == nullptr)
            {
                m_rowPadding = rowPadding;
                return true;
            }

            size_t oldRowSize = sizeof(ElemType) * m_width + m_rowPadding;
            size_t newRowSize = sizeof(ElemType) * m_width + rowPadding;
            size_t newSize = newRowSize * m_height;

            if(rowPadding > m_rowPadding)
            {
                uint8_t* data = reinterpret_cast<uint8_t*>(m_data);
                data = reinterpret_cast<uint8_t*>(realloc(data, newSize));
                if(data == nullptr)
                {
                    return false;
                }
                
                size_t row = m_height - 1;
                do
                {
                    memcpy(data + row * newRowSize, data + row * oldRowSize, oldRowSize);
                    row--;
                }while(row > 0);
                m_data = reinterpret_cast<ElemType*>(data);
                m_rowPadding = rowPadding;
            }
            else
            {
                uint8_t* data = reinterpret_cast<uint8_t*>(m_data);
                for(size_t row = 0; row < m_height; row++)
                {
                    memcpy(data + row * newRowSize, data + row * oldRowSize, oldRowSize);
                }
                data = reinterpret_cast<uint8_t*>(realloc(data, newSize));
                if(data == nullptr)
                {
                    return false;
                }
                m_data = reinterpret_cast<ElemType*>(data);
                m_rowPadding = rowPadding;
            }
            return true;
        }

        /************************************************************
        * @brief get a row of the matrix
        * @param[in] row the row index
        * @return the row
        ************************************************************/
        ElemType* operator [] (size_t row)
        {
            uint8_t* data = reinterpret_cast<uint8_t*>(m_data);
            data = data + ((sizeof(ElemType) * m_width + m_rowPadding) * row);
            return reinterpret_cast<ElemType*>(data);
        }

        /************************************************************
        * @brief get a row of the const matrix
        * @param[in] row the row index
        * @return the row
        ************************************************************/
        const ElemType* operator [] (size_t row) const
        {
            uint8_t* data = reinterpret_cast<uint8_t*>(m_data);
            data = data + ((sizeof(ElemType) * m_width + m_rowPadding) * row);
            ElemType* ret = reinterpret_cast<ElemType*>(data);
            return const_cast<const ElemType*>(ret);
        }

        /************************************************************
        * @brief assign a matrix by deep copy
        * @param[in] another the matrix to be copied
        * @return the matrix assigned
        ************************************************************/
        Mat& operator = (const Mat& another)
        {
            if(another.size() == 0 || another.m_data == nullptr)
            {
                m_data = nullptr;
            }
            else
            {
                void* data = malloc(another.size());
                if(data == nullptr)
                {
                    throw std::bad_alloc();
                }
                memcpy(data, another.m_data, another.size());
                m_data = reinterpret_cast<ElemType*>(data);
            }
            m_width = another.m_width;
            m_height = another.m_height;
            m_rowPadding = another.m_rowPadding;
        }

        /************************************************************
        * @brief assign a matrix by data moving
        * @param[in] another the matrix to be moved data
        * @return the matrix assigned
        ************************************************************/
        Mat& operator = (Mat&& another)
        {
            m_data = another.m_data;
            m_width = another.m_width;
            m_height = another.m_height;
            another.m_data = nullptr;
        }

        /************************************************************
        * @brief get memory size of matrix data
        * @return the size as bytes
        ************************************************************/
        size_t size() const
        {
            return (sizeof(ElemType) * m_width + m_rowPadding) * m_height;
        }

        /************************************************************
        * @brief get count of matrix elements
        * @return the count 
        ************************************************************/
        size_t count() const
        {
            return m_width * m_height;
        }

        /************************************************************
        * @brief reset the width and height of the matrix
        *        element data will be invalid
        * @param[in] width the new width
        * @param[in] height the new height
        ************************************************************/
        void resize(size_t width, size_t height)
        {
            m_width  = width;
            m_height = height;
            if(size() == 0)
            {
                free(m_data);
                m_data = nullptr;
                return;
            }

            void* data = reinterpret_cast<void*>(m_data);
            data = realloc(data, size());
            if(data == nullptr)
            {
                throw std::bad_alloc();
            }
            this->m_data = reinterpret_cast<ElemType*>(data);
        }

        /************************************************************
        * @brief reset the width and height of the matrix with row padding, 
        *        element data will be invalid
        * @param[in] width the new width
        * @param[in] height the new height
        * @param[in] rowPadding the row padding
        ************************************************************/
        void resize(size_t width, size_t height, size_t rowPadding)
        {
            m_width  = width;
            m_height = height;
            m_rowPadding = rowPadding;

            if(size() == 0)
            {
                free(m_data);
                m_data = nullptr;
                return;
            }

            void* data = reinterpret_cast<void*>(m_data);
            data = realloc(data, size());
            if(data == nullptr)
            {
                throw std::bad_alloc();
            }
            this->m_data = reinterpret_cast<ElemType*>(data);
        }

        /************************************************************
        * @brief handle the every elements of the matrix
        * @param[in] callback callback funtion to hanlde the elements
        ************************************************************/
        void map(std::function<void(ElemType&)> callback)
        {
            for(size_t row = 0; row < m_height; row++)
            {
                for(size_t col = 0; col < m_width; col++)
                {
                    callback((*this)[row][col]);
                }
            }
        }

        /************************************************************
        * @brief handle the every elements of the matrix
        * @param[in] callback callback funtion to hanlde the elements
        ************************************************************/
        void map(std::function<void(ElemType&, size_t, size_t)> callback)
        {
            for(size_t row = 0; row < m_height; row++)
            {
                for(size_t col = 0; col < m_width; col++)
                {
                    callback((*this)[row][col], row, col);
                }
            }
        }
        
        /************************************************************
        * @brief handle the every elements of the matrix and sum the result
        * @param[in] callback callback funtion to hanlde the elements
        ************************************************************/
        template<typename T>
        T reduce(std::function<T(ElemType&)> callback)
        {
            T n{0};
            for(size_t row = 0; row < m_height; row++)
            {
                for(size_t col = 0; col < m_width; col++)
                {
                    n += callback((*this)[row][col]);
                }
            }
            return n;
        }

        /************************************************************
        * @brief handle the every elements of the matrix and sum the result
        * @param[in] callback callback funtion to hanlde the elements
        ************************************************************/
        template<typename T>
        T reduce(std::function<T(ElemType&, size_t, size_t)> callback)
        {
            T n{0};
            for(size_t row = 0; row < m_height; row++)
            {
                for(size_t col = 0; col < m_width; col++)
                {
                    n += callback((*this)[row][col], row, col);
                }
            }
            return n;
        }

    private:
        size_t m_width;
        size_t m_height;
        size_t m_rowPadding;
        ElemType* m_data;
    }; // class Mat

    namespace PixelConvert
    {
        /************************************************************
        * @brief convert any type pixel to a binary pixel
        * @param[out] out the output binary pixel
        * @param[in] in the input pixel
        * @return is success
        ************************************************************/
        template<typename BinPixel, typename AnyPixel>
        bool BINARY(BinPixel& out, const AnyPixel& in, uint8_t threshold=0x7f)
        {
            // RGB(A)
            if(utils::eval<bool, ColorSpace::compatible(AnyPixel::id, "RGB")>::value)
            {
                uint8_t gray = (in.red()*299 + in.green()*587 + in.blue()*114 + 500) / 1000;
                out.setBinary(gray > threshold);
                return true;
            }

            // gray scale
            if(utils::eval<bool, ColorSpace::compatible(AnyPixel::id, "Y")>::value)
            {
                out.setBinary(in.grayScale() > threshold);
                return true;
            }

            // binary
            if(utils::eval<bool, ColorSpace::compatible(AnyPixel::id, "2")>::value)
            {
                out.setBinary(in.binary());
                return true;
            }

            return false;
        }

        /************************************************************
        * @brief convert any type pixel to a gray scale pixel
        * @param[out] out the output gray scale pixel
        * @param[in] in the input pixel
        * @return is success
        ************************************************************/
        template<typename GrayPixel, typename AnyPixel>
        bool GRAY(GrayPixel& out, const AnyPixel& in)
        {
            // RGB(A)
            if(utils::eval<bool, ColorSpace::compatible(AnyPixel::id, "RGB")>::value)
            {
                out.setGrayScale((in.red()*299 + in.green()*587 + in.blue()*114 + 500) / 1000);
                return true;
            }

            // gray scale
            if(utils::eval<bool, ColorSpace::compatible(AnyPixel::id, "Y")>::value)
            {
                out.setGrayScale(in.grayScale());
                return true;
            }

            // binary
            if(utils::eval<bool, ColorSpace::compatible(AnyPixel::id, "2")>::value)
            {
                out.setGrayScale(in.binary() ? 0xff : 0);
                return true;
            }

            return false;
        }

        /************************************************************
        * @brief convert any type pixel to a RGB type pixel
        * @param[out] out the output RGB pixel
        * @param[in] in the input pixel
        * @return is success
        ************************************************************/
        template<typename RGBPixel, typename AnyPixel>
        bool RGB(RGBPixel& out, const AnyPixel& in)
        {
            // RGB(A)
            if(utils::eval<bool, ColorSpace::compatible(AnyPixel::id, "RGB")>::value)
            {
                
                out.setRed(in.red());
                out.setGreen(in.green());
                out.setBlue(in.blue());
                return true;
            }

            // gray scale
            if(utils::eval<bool, ColorSpace::compatible(AnyPixel::id, "Y")>::value)
            {
                out.setRed(in.grayScale());
                out.setGreen(in.grayScale());
                out.setBlue(in.grayScale());
                return true;
            }

            // binary
            if(utils::eval<bool, ColorSpace::compatible(AnyPixel::id, "2")>::value)
            {
                out.setRed(in.binary() ? 0xff : 0);
                out.setGreen(in.binary() ? 0xff : 0);
                out.setBlue(in.binary() ? 0xff : 0);
                return true;
            }

            return false;
        }

        /************************************************************
        * @brief convert a RGBA type pixel to another RGBA type pixel
        * @param[out] out the output RGBA pixel
        * @param[in] in the input RGBA pixel
        * @return the reference to out
        ************************************************************/
        template<typename RGBAPixel, typename AnyPixel>
        bool RGBA(RGBAPixel& out, const AnyPixel& in)
        {
            // RGBA
            if(utils::eval<bool, ColorSpace::compatible(AnyPixel::id, "RGBA")>::value)
            {
                out.setRed(in.red());
                out.setGreen(in.green());
                out.setBlue(in.blue());
                out.setAlpha(in.alpha());
                return true;
            }

            // RGB
            if(utils::eval<bool, ColorSpace::compatible(AnyPixel::id, "RGB")>::value)
            {
                out.setRed(in.red());
                out.setGreen(in.green());
                out.setBlue(in.blue());
                out.setAlpha(0);
                return true;
            }

            // geay scale
            if(utils::eval<bool, ColorSpace::compatible(AnyPixel::id, "Y")>::value)
            {
                out.setRed(in.grayScale());
                out.setGreen(in.grayScale());
                out.setBlue(in.grayScale());
                out.setAlpha(0xff);
                return true;
            }

            // binary
            if(utils::eval<bool, ColorSpace::compatible(AnyPixel::id, "2")>::value)
            {
                out.setRed(in.binary() ? 0xff : 0);
                out.setGreen(in.binary() ? 0xff : 0);
                out.setBlue(in.binary() ? 0xff : 0);
                out.setAlpha(0xff);
                return true;
            }

            return false;
        }
    }; // namespace ::lolita::PixelConvert

    namespace MatConvert
    {
        /************************************************************
        * @brief convert any type matrix to a binary matrix
        * @param[out] out the output binary matrix
        * @param[in] in the input matrix
        * @param[in] keepRowPadding is keep the row padding
        * @return is success
        ************************************************************/
        template<typename BinPixel, typename AnyPixel>
        bool BINARY(Mat<BinPixel>& out, const Mat<AnyPixel> in, uint8_t threshold=0x7f, bool keepRowPadding=false)
        {
            if(keepRowPadding)
                out.resize(in.width(), in.height(), in.rowPadding());
            else
                out.resize(in.width(), in.height());

            for(size_t row = 0; row < in.height(); row++)
            {
                for(size_t col = 0; col < in.width(); col++)
                {
                    if(PixelConvert::BINARY(out[row][col], in[row][col], threshold) == false)
                    {
                        return false;
                    }
                }
            }
            return true;
        }

        /************************************************************
        * @brief convert any type matrix to a gray scale matrix
        * @param[out] out the output gray scale matrix
        * @param[in] in the input matrix
        * @param[in] keepRowPadding is keep the row padding
        * @return is success
        ************************************************************/
        template<typename GrayPixel, typename AnyPixel>
        bool GRAY(Mat<GrayPixel>& out, const Mat<AnyPixel> in, bool keepRowPadding=false)
        {
            if(keepRowPadding)
                out.resize(in.width(), in.height(), in.rowPadding());
            else
                out.resize(in.width(), in.height());

            for(size_t row = 0; row < in.height(); row++)
            {
                for(size_t col = 0; col < in.width(); col++)
                {
                    if(PixelConvert::GRAY(out[row][col], in[row][col]) == false)
                    {
                        return false;
                    }
                }
            }
            return true;
        }

        /************************************************************
        * @brief convert any type matrix to a RGB matrix
        * @param[out] out the output gray scale matrix
        * @param[in] in the input RGB matrix
        * @param[in] keepRowPadding is keep the row padding
        * @return is success
        ************************************************************/
        template<typename RGBPixel, typename AnyPixel>
        bool RGB(Mat<RGBPixel>& out, const Mat<AnyPixel> in, bool keepRowPadding=false)
        {
            if(keepRowPadding)
                out.resize(in.width(), in.height(), in.rowPadding());
            else
                out.resize(in.width(), in.height());
                
            for(size_t row = 0; row < in.height(); row++)
            {
                for(size_t col = 0; col < in.width(); col++)
                {
                    if(PixelConvert::RGB(out[row][col], in[row][col]) == false)
                    {
                        return false;
                    }
                }
            }
            return true;
        }

        /************************************************************
        * @brief convert any type matrix to a RGBA matrix
        * @param[out] out the output gray scale matrix
        * @param[in] in the input RGB matrix
        * @param[in] keepRowPadding is keep the row padding
        * @return is success
        ************************************************************/
        template<typename RGBPixel, typename AnyPixel>
        bool RGBA(Mat<RGBPixel>& out, const Mat<AnyPixel> in, bool keepRowPadding=false)
        {
            if(keepRowPadding)
                out.resize(in.width(), in.height(), in.rowPadding());
            else
                out.resize(in.width(), in.height());
                
            for(size_t row = 0; row < in.height(); row++)
            {
                for(size_t col = 0; col < in.width(); col++)
                {
                    if(PixelConvert::RGBA(out[row][col], in[row][col]) == false)
                    {
                        return false;
                    }
                }
            }
            return true;
        }

        /************************************************************
        * @brief cast any type matrix to another type matrix
        * @param[in] in the input matrix
        * @param[in] keepRowPadding is keep the row padding
        * @return casted matrix
        ************************************************************/
        template<typename T, typename AnyPixel>
        const typename std::enable_if<!std::is_same<T, AnyPixel>::value, Mat<T>>::type
        cast(const Mat<AnyPixel>& in, bool keepRowPadding=false)
        {   
            Mat<T> out;
            
            if(utils::eval<bool, ColorSpace::compatible(T::id, "RGBA")>::value)
            {
                RGBA(out, in, keepRowPadding);
            }

            if(utils::eval<bool, ColorSpace::compatible(T::id, "RGB")>::value)
            {
                RGB(out, in, keepRowPadding);
            }

            if(utils::eval<bool, ColorSpace::compatible(T::id, "Y")>::value)
            {
                GRAY(out, in, keepRowPadding);
            }


            if(utils::eval<bool, ColorSpace::compatible(T::id, "2")>::value)
            {
                BINARY(out, in, 0x7f, keepRowPadding);
            }

            return out;
        }

        /************************************************************
        * @brief cast same type matrix
        * @param[in] in the input matrix
        * @return copied matrix
        ************************************************************/
        template<typename T, typename AnyPixel>
        const typename std::enable_if<std::is_same<T, AnyPixel>::value, Mat<T>>::type
        cast(const Mat<AnyPixel>& in)
        {   
            return in;
        }
    }; // namespace ::lolita::MatConvert

    namespace BMP
    {
        enum class Format : int
        {
            Bit24 = 24,
            Bit16 = 16,
            Palette = 8,
            Binary = 1,
        };

        #pragma pack(push)
        #pragma pack(1)

        struct InfoHeader
        {
            uint32_t size = 40;             // sizeof(InfoHeader)
            uint32_t width;
            uint32_t height;
            uint16_t planes = 1;
            uint16_t bitCount = 24;
            uint32_t compression = 0;
            uint32_t pixelSize;             // as bytes
            uint32_t xPixelPerMeter = 3780;
            uint32_t yPixelPerMeter = 3780;
            uint32_t colorUsed;             // count of palettes
            uint32_t colorImportant;

            /************************************************************
            * @brief create InfoHeader
            * @param[in] width the width of image
            * @param[in] height the height of image
            * @param[in] pixelSize the size of pixel data
            * @param[in] color the count of color
            ************************************************************/
            InfoHeader(uint32_t width=0, uint32_t height=0, uint32_t pixelSize=0, uint16_t bits=24, uint32_t color=0):
                width(width), 
                height(height), 
                bitCount(bits),
                pixelSize(pixelSize),
                colorUsed(color), 
                colorImportant(color)
            {}
        };

        struct FileHeader
        {
            uint8_t type[2] = {'B', 'M'};   // must be "BM"
            uint32_t filesize;                  // file size
            uint16_t reserved1 = 0;
            uint16_t reserved2 = 0;
            uint32_t offset;                // offset of pixel data, default sizeof(FileHeader) + sizeof(InfoHeader)

            /************************************************************
            * @brief create FileHeader
            * @param[in] pixelSize the size of pixel data
            * @param[in] offset the offset of pixel data
            ************************************************************/
            FileHeader(uint32_t pixelSize=0, uint32_t offset=sizeof(FileHeader) + sizeof(InfoHeader)):
                filesize(offset + pixelSize), offset(offset)
            {}
        };

        struct RGBPalette
        {
            uint8_t blue;
            uint8_t green;
            uint8_t red;
            uint8_t reserved;

            RGBPalette(uint8_t r, uint8_t g, uint8_t b):
                blue(b), green(g), red(r), reserved(0)
            {}

            RGBPalette(uint64_t rgb = 0):
                blue(rgb & 0xff), 
                green((rgb >> 8) & 0xff), 
                red((rgb >> 16) & 0xff), 
                reserved(0)
            {}
        };

        #pragma pack(pop)

        constexpr static const uint32_t HeaderSize = sizeof(FileHeader) + sizeof(InfoHeader);

        namespace _BMP_private
        {
            /************************************************************
            * @brief read the file header of BMP image
            * @param[in] fp FILE pointer to a BPM image
            * @param[out] fileHeader the file header
            * @return is success
            ************************************************************/
            static inline bool readFileHeader(FILE* fp, FileHeader& fileHeader)
            {
                return fseek(fp, 0, SEEK_SET) == 0 &&
                        fread(&fileHeader, sizeof(fileHeader), 1, fp) == 1;
            }

            /************************************************************
            * @brief read the info header of BMP image
            * @param[in] fp FILE pointer to a BPM image
            * @param[out] infoHeader the info header
            * @return is success
            ************************************************************/
            static inline bool readInfoHeader(FILE* fp, InfoHeader& infoHeader)
            {
                return fseek(fp, sizeof(FileHeader), SEEK_SET) == 0 &&
                        fread(&infoHeader, sizeof(infoHeader), 1, fp) == 1;
            }

            /************************************************************
            * @brief read a 24bit BMP image
            * @param[out] image image matrix
            * @param[in] fp FILE pointer to a BPM image
            * @param[in] width the width of image
            * @param[in] height the height of image
            * @return is success
            ************************************************************/
            static inline bool read24(Mat<Pixel::BGR24>& image, FILE* fp, uint32_t offset, uint32_t width, uint32_t height)
            {
                if(fp == nullptr || fseek(fp, offset, SEEK_SET) != 0)
                {
                    return false;
                }

                image.resize(width, height, utils::padding(3*width, 4));
                return fread(image.data(), image.size(), 1, fp) == 1;
            }

            /************************************************************
            * @brief write a 24 bits BMP image
            * @param[in] image the image matrix
            * @param[in] file the BMP file name
            * @return is success
            ************************************************************/
            template<typename AnyPixel>
            static inline bool write24(const Mat<AnyPixel>& image, const char* file)
            {
                FILE* fp = fopen(file, "wb");
                if(fp == nullptr)
                {
                    return false;
                }

                Mat<Pixel::BGR24> out = MatConvert::cast<Pixel::BGR24>(image);
                out.setRowPadding(utils::padding(3*out.width(), 4));

                FileHeader fileHeader(out.size());
                InfoHeader infoHeader(out.width(), out.height(), out.size());
                
                fwrite(&fileHeader, sizeof(fileHeader), 1, fp);
                fwrite(&infoHeader, sizeof(infoHeader), 1, fp);
                fwrite(out.data(), out.size(), 1, fp);
                fclose(fp);
                return true;
            }

            /************************************************************
            * @brief write a 16 bits BMP image (RGB1555)
            * @param[in] image the image matrix
            * @param[in] file the BMP file name
            * @return is success
            ************************************************************/
            template<typename AnyPixel>
            static inline bool write16(const Mat<AnyPixel>& image, const char* file)
            {
                FILE* fp = fopen(file, "wb");
                if(fp == nullptr)
                {
                    return false;
                }

                Mat<Pixel::BGR24> out = MatConvert::cast<Pixel::BGR24>(image);

                uint32_t rowSize = out.width() * 2;
                uint32_t rowPadding = utils::padding(rowSize, 4);
                rowSize = rowSize + rowPadding;
                uint32_t pixelSize = rowSize * out.height();
                FileHeader fileHeader(pixelSize);
                InfoHeader infoHeader(out.width(), out.height(), pixelSize, 16);
                
                fwrite(&fileHeader, sizeof(fileHeader), 1, fp);
                fwrite(&infoHeader, sizeof(infoHeader), 1, fp);
                out.map([fp, rowPadding, &out](Pixel::BGR24& pix, size_t row, size_t col){
                    (void)(row);
                    uint16_t color = 0;
                    color |= static_cast<uint16_t>(pix.red()) >> 3 << 10;
                    color |= static_cast<uint16_t>(pix.green()) >> 3 << 5;
                    color |= static_cast<uint16_t>(pix.blue()) >> 3;
                    fwrite(&color, 2, 1, fp);
                    
                    if(col == out.width() - 1)
                    {
                        uint8_t pad = 0;
                        fwrite(&pad, 1, rowPadding, fp);
                    }
                });
                fclose(fp);
                return true;
            }

            /************************************************************
            * @brief generate RGB palettes
            * @param[in] image the image matrix
            * @param[out] out the index matrix
            * @param[out] palettes the BMP RGB palettes
            * @param[in] max the max count of palettes
            * @return is success
            ************************************************************/
            template<typename RGBPixel>
            static inline bool generatePalette(const Mat<RGBPixel>& image, Mat<uint8_t>& out, std::vector<RGBPalette>& palettes, size_t max=256)
            {
                out.resize(image.width(), image.height());
                std::map<uint64_t, size_t> cache;

                for(size_t row = 0; row < image.height(); row++)
                {
                    for(size_t col = 0; col < image.width(); col++)
                    {
                        RGBPixel pix = image[row][col];
                        uint64_t color = static_cast<uint64_t>(pix);
                        try
                        {
                            size_t index = cache.at(color);
                            out[row][col] = index;
                        }
                        catch(std::out_of_range& e)
                        {
                            size_t index = palettes.size();
                            if(index >= max)
                            {
                                return false;
                            }
                            palettes.emplace_back(pix.red(), pix.green(), pix.blue());
                            cache[color] = index;
                            out[row][col] = index;
                        }
                    }
                }

                return true;
            }

            /************************************************************
            * @brief write a 24bit BMP image with 8 bits palette
            * @param[in] image the index matrix
            * @param[in] palettes the palettes
            * @param[in] file the BMP file name
            * @return is success
            ************************************************************/
            static inline bool writeWithPalette8(Mat<uint8_t>& image, const std::vector<RGBPalette>& palettes, const char* file)
            {
                FILE* fp = fopen(file, "wb");
                if(fp == nullptr)
                {
                    return false;
                }

                image.setRowPadding(utils::padding(image.width(), 4));

                uint32_t offset = sizeof(FileHeader) + sizeof(InfoHeader) + sizeof(RGBPalette) * palettes.size();
                FileHeader fileHeader(image.size(), offset);
                InfoHeader infoHeader(image.width(), image.height(), image.size(), 8, palettes.size());

                fwrite(&fileHeader, sizeof(fileHeader), 1, fp);
                fwrite(&infoHeader, sizeof(infoHeader), 1, fp);
                fwrite(palettes.data(), sizeof(RGBPalette) * palettes.size(), 1, fp);
                fwrite(image.data(), image.size(), 1, fp);
                printf("%zu %zu %zu\n",utils::padding(image.width(), 4), image.rowPadding(), image.size());
                fclose(fp);
                return true;
            }

            /************************************************************
            * @brief write a binary image with palettes
            * @param[in] image the image matrix
            * @param[in] file the BMP file name
            * @param[in] binary0 the color of 0 in binary image
            * @param[in] binary1 the color of 1 in binary image
            * @return is success
            ************************************************************/
            template<typename AnyPixel>
            static inline bool writeBinary(const Mat<AnyPixel>& image, const char* file, 
                                            RGBPalette binary0 = 0, RGBPalette binary1 = 0xffffff)
            {
                FILE* fp = fopen(file, "wb");
                if(fp == nullptr)
                {
                    return false;
                }

                Mat<Pixel::Binary> out = MatConvert::cast<Pixel::Binary>(image);
                
                size_t rowSize = utils::ceil(image.width(), size_t(8));
                size_t rowPadding = utils::padding(rowSize, 4);
                rowSize = rowSize + rowPadding;

                uint32_t offset = sizeof(FileHeader) + sizeof(InfoHeader) + 2 * sizeof(RGBPalette);
                uint32_t pixelSize = rowSize * out.height();
                FileHeader fileHeader(pixelSize, offset);
                InfoHeader infoHeader(out.width(), out.height(), pixelSize, 1);
                
                fwrite(&fileHeader, sizeof(fileHeader), 1, fp);
                fwrite(&infoHeader, sizeof(infoHeader), 1, fp);
                fwrite(&binary0, sizeof(RGBPalette), 1, fp);
                fwrite(&binary1, sizeof(RGBPalette), 1, fp);
                for(size_t row = 0; row < out.height(); row++)
                {
                    for(size_t col = 0; col < out.width(); col+=8)
                    {
                        uint8_t color = 0;
                        for(size_t bit = 0; bit < 8 && col + bit < out.width(); bit++)
                        {
                            if(out[row][col+bit].binary())
                            {
                                color |= 1 << (7-bit);
                            }
                        }
                        fwrite(&color, 1, 1, fp);
                    }
                    static uint8_t n = 0;
                    fwrite(&n, rowPadding, 1, fp);
                }

                fclose(fp);
                return true;
            }

            /************************************************************
            * @brief write a any BMP image with palettes
            * @param[in] image the image matrix
            * @param[in] file the BMP file name
            * @return is success
            ************************************************************/
            template<typename AnyPixel>
            static inline bool writeWithPalette(const Mat<AnyPixel>& image, const char* file)
            {
                Mat<Pixel::BGR24> BGR = MatConvert::cast<Pixel::BGR24>(image);
                Mat<uint8_t> out; 
                std::vector<RGBPalette> palettes;
                if(!generatePalette(BGR, out, palettes))
                {
                    return false;
                }
                
                return writeWithPalette8(out, palettes, file);
            }

        }; // namespace ::lolita::BMP::_BMP_private

        /************************************************************
        * @brief read a BMP image
        * @param[in] fp FILE pointer to a BPM image
        * @param[in] width the width of image
        * @param[in] height the height of image
        * @return a image matrix
        ************************************************************/
        static inline Mat<Pixel::BGR24> read(const char* file)
        {
            Mat<Pixel::BGR24> image;
            FILE* fp = fopen(file, "rb");
            if(fp == nullptr)
            {
                return image;
            }

            FileHeader fileHeader;
            InfoHeader infoHeader;

            _BMP_private::readFileHeader(fp, fileHeader);
            _BMP_private::readInfoHeader(fp, infoHeader);

            switch(infoHeader.bitCount)
            {
            case 32:
                break;

            case 24:
                _BMP_private::read24(image, fp, fileHeader.offset, infoHeader.width, infoHeader.height);
                break;

            case 16:
                break;

            case 8:
                break;

            case 4:
                break;

            case 1:
                break;
            }

            fclose(fp);

            return image;
        }

        /************************************************************
        * @brief write a BMP image
        * @param[in] image the image matrix
        * @param[in] file the file path
        * @param[in] binary0 the color of 0 in binary image
        * @param[in] binary1 the color of 1 in binary image
        * @return is success
        ************************************************************/
        template<typename AnyPixel>
        static inline bool write(const Mat<AnyPixel>& image, const char* file, Format format=Format::Bit24, 
                                    RGBPalette binary0 = 0, RGBPalette binary1 = 0xffffff)
        {
            if(!image.valid() || file == nullptr)
            {
                return false;
            }

            switch(format)
            {
            case Format::Bit24:
                return _BMP_private::write24(image, file);

            case Format::Bit16:
                return _BMP_private::write16(image, file);

            case Format::Palette:
                return _BMP_private::writeWithPalette(image, file);

            case Format::Binary:
                return _BMP_private::writeBinary(image, file, binary0, binary1);
            
            default:
                return false;
            }
        }
        
    }; // namespace ::lolita::BMP

    #ifdef LOLITA_USE_JPEG
    namespace JPEG
    {

    };
    #endif

    namespace algorithm
    {

    }; // namespace algorithm

}; // namespace lolita

#endif
