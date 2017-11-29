#ifndef LOLITA_BMP_H
#define LOLITA_BMP_H

#include <stdint.h>
#include <string>
#include "mat.hpp"

namespace lolita
{

class Bmp
{
public:
    static std::string error();
    static bool read(Image& mat, std::string file);
    static bool write(Image& mat, std::string file, uint8_t bits=24);

private:
    static std::string errorMessage;
};

}; // namespace lolita

#endif