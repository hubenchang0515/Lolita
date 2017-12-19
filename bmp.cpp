#include <stdio.h>
#include "bmp.h"

namespace lolita
{

typedef struct BitMapFileHeader 
{ 
    uint8_t  bfType[2];//'BM'
    uint32_t bfSize;//file size
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;//offset of pixel data
}BitMapFileHeader;

typedef struct BitMapInfoHeader 
{ 
    uint32_t biSize;//28h(40)
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;//bytes of pixel data
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
}BitMapInfoHeader; 

typedef struct BGRPalette
{ 
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t rgbReserved;
}BGRPalette; 


static bool BMP_ReadFileHeader(FILE *bmpfile,BitMapFileHeader& buf) 
{ 
	return
	fseek(bmpfile,0,SEEK_SET) == 0 &&
	fread(buf.bfType,2,1,bmpfile) == 1 &&
	fread(&buf.bfSize,4,1,bmpfile) == 1 &&
	fread(&buf.bfReserved1,2,1,bmpfile) == 1 &&
	fread(&buf.bfReserved2,2,1,bmpfile) == 1 &&
	fread(&buf.bfOffBits,4,1,bmpfile) == 1;
} 
  
static bool BMP_ReadInfoHeader(FILE *bmpfile,BitMapInfoHeader& buf) 
{ 
	return 
	fseek(bmpfile,14,SEEK_SET) == 0 &&
	fread(&buf.biSize,4,1,bmpfile) == 1 &&
	fread(&buf.biWidth,4,1,bmpfile) == 1 &&
	fread(&buf.biHeight,4,1,bmpfile) == 1 &&
	fread(&buf.biPlanes,2,1,bmpfile) == 1 &&
	fread(&buf.biBitCount,2,1,bmpfile) == 1 &&
	fread(&buf.biCompression,4,1,bmpfile) == 1 &&
	fread(&buf.biSizeImage,4,1,bmpfile) == 1 &&
	fread(&buf.biXPelsPerMeter,4,1,bmpfile) == 1 &&
	fread(&buf.biYPelsPerMeter,4,1,bmpfile) == 1 &&
	fread(&buf.biClrUsed,4,1,bmpfile) == 1 &&
	fread(&buf.biClrImportant,4,1,bmpfile) == 1;
} 
  

static bool BMP_WriteFileHeader(FILE *bmpfile,BitMapFileHeader& buf) 
{ 
	return 
	fseek(bmpfile,0,SEEK_SET) == 0 &&
	fwrite(buf.bfType,2,1,bmpfile) == 1 &&
	fwrite(&buf.bfSize,4,1,bmpfile) == 1 &&
	fwrite(&buf.bfReserved1,2,1,bmpfile) == 1 &&
	fwrite(&buf.bfReserved2,2,1,bmpfile) == 1 &&
	fwrite(&buf.bfOffBits,4,1,bmpfile) == 1;
} 
  
static bool BMP_WriteInfoHeader(FILE *bmpfile,BitMapInfoHeader& buf) 
{
	return 
	fseek(bmpfile,14,SEEK_SET) == 0 &&
	fwrite(&buf.biSize,4,1,bmpfile) == 1 &&
	fwrite(&buf.biWidth,4,1,bmpfile) == 1 &&
	fwrite(&buf.biHeight,4,1,bmpfile) == 1 &&
	fwrite(&buf.biPlanes,2,1,bmpfile) == 1 &&
	fwrite(&buf.biBitCount,2,1,bmpfile) == 1 &&
	fwrite(&buf.biCompression,4,1,bmpfile) == 1 &&
	fwrite(&buf.biSizeImage,4,1,bmpfile) == 1 &&
	fwrite(&buf.biXPelsPerMeter,4,1,bmpfile) == 1 &&
	fwrite(&buf.biYPelsPerMeter,4,1,bmpfile) == 1 &&
	fwrite(&buf.biClrUsed,4,1,bmpfile) == 1 &&
	fwrite(&buf.biClrImportant,4,1,bmpfile) == 1;
}  


/**********************************************************************************/
/* 32bit color BGRA8888 */
static bool readBgra32(Image& mat, FILE* fp, uint32_t offset, uint32_t w, uint32_t h)
{
    mat.resize(w,h);
    if(fseek(fp, offset, SEEK_SET) != 0)
    {
        return false;
    }
    for(uint32_t i = 0; i < h; i++)
    {
        for(uint32_t j = 0; j < w; j++) 
        {
            mat[h-i-1][j].blue = fgetc(fp);
            mat[h-i-1][j].green = fgetc(fp);
            mat[h-i-1][j].red = fgetc(fp);
            mat[h-i-1][j].alpha = fgetc(fp);
        }
    }

    return true;
}

/* 24bit color BGR888 */
static bool readBgr24(Image& mat, FILE* fp, uint32_t offset, uint32_t w, uint32_t h)
{
    mat.resize(w,h);
    if(fseek(fp, offset, SEEK_SET) != 0)
    {
        return false;
    }
    for(uint32_t i = 0; i < h; i++)
    {
        for(uint32_t j = 0; j < w; j++) 
        {
            mat[h-i-1][j].blue = fgetc(fp);
            mat[h-i-1][j].green = fgetc(fp);
            mat[h-i-1][j].red = fgetc(fp);
        }

        /* byets of line should be multiple of 4 , otherwise filled by 0 */
        /* skip these data */
        size_t skip = (w * 3 + 3)/4 * 4 - w * 3;
        if(fseek(fp, skip, SEEK_CUR) != 0)
        {
            return false;
        }
    }

    return true;
}

/* 16bit color RGB565 */
static bool readRgb16(Image& mat, FILE* fp, uint32_t offset, uint32_t w, uint32_t h)
{
    mat.resize(w,h);
    uint16_t color = 0;
    if(fseek(fp, offset, SEEK_SET) != 0)
    {
        return false;
    }
    for(uint32_t i = 0; i < h; i++)
    {
        for(uint32_t j = 0; j < w; j++) 
        {
            if(fread(&color, 2, 1, fp) != 1)
            {
            	return false;
            }
            mat[h-i-1][j].red  = ((color & 0xf800) >> 11) << 3;
            mat[h-i-1][j].green = ((color & 0x07e0) >> 5) << 2;
            mat[h-i-1][j].blue   =  (color & 0x001f) << 3;
        }

        /* byets of line should be multiple of 4 , otherwise filled by 0 */
        /* skip these data */
        size_t skip = (w * 2 + 3)/4 * 4 - w * 2;
        if(fseek(fp, skip, SEEK_CUR) != 0)
        {
        	return false;
        }
    }

    return true;
}

/* 8bit color , 256 palettes */
static bool readPalette8(Image& mat, FILE* fp, uint32_t offset, uint32_t w, uint32_t h)
{
    mat.resize(w,h);
    uint8_t index = 0;
    BGRPalette color;
    for(uint32_t i = 0; i < h; i++)
    {
        for(uint32_t j = 0; j < w; j++) 
        {
            if(fseek(fp, offset + i * w + j, SEEK_SET) != 0)
            {
            	return false;
            }
            index = fgetc(fp);
            if(fseek(fp, 14 + 40 + 4 * index, SEEK_SET) != 0 ||
                fread(&color, 4, 1, fp) != 1)
            {
                return false;
            }
            mat[h-i-1][j].red = color.red;
            mat[h-i-1][j].green = color.green;
            mat[h-i-1][j].blue = color.blue;
        }

        /* byets of line should be multiple of 4 , otherwise filled by 0 */
        /* skip these data */
        size_t skip = (w + 3)/4 * 4 - w;
        if(fseek(fp, skip, SEEK_CUR) != 0)
        {
            return false;
        }
    }

    return true;
}

/* 4bit color , 16 palettes */
static bool readPalette4(Image& mat, FILE* fp, uint32_t offset, uint32_t w, uint32_t h)
{
    uint64_t bytesOfLine = ((w+1)/2 + 3)/4 * 4; // skip filled 0
    mat.resize(w,h);
    uint8_t index = 0;
    BGRPalette color;
    for(uint32_t i = 0; i < h; i++)
    {
        uint32_t j = 0;
        for(; j < w; j+=2) 
        {
            if(fseek(fp, offset + i * bytesOfLine + j/4, SEEK_SET) != 0)
            {
                return false;
            }
            index = fgetc(fp);
            if(fseek(fp, 14 + 40 + 4 * ((index & 0xf0) >> 4), SEEK_SET) != 0 ||
                fread(&color, 4, 1, fp) != 1)
            {
                return false;
            }
            mat[h-i-1][j].red = color.red;
            mat[h-i-1][j].green = color.green;
            mat[h-i-1][j].blue = color.blue;

            if(j+1 < w)
            {
                if(fseek(fp, 14 + 40 + 4 * (index & 0x0f), SEEK_SET) != 0 ||
                    fread(&color, 4, 1, fp) != 1)
                {
                    return false;
                }
                mat[h-i-1][j+1].red = color.red;
                mat[h-i-1][j+1].green = color.green;
                mat[h-i-1][j+1].blue = color.blue;
            }
        }
    }

    return true;
}

/* 1bit color , 2 palettes */
static bool readPalette1(Image& mat, FILE* fp, uint32_t offset, uint32_t w, uint32_t h)
{
    uint64_t bytesOfLine = ((w+7)/8 + 3)/4 * 4;
    mat.resize(w,h);
    uint8_t index = 0;
    BGRPalette color;
    for(uint32_t i = 0; i < h; i++)
    {
        uint32_t j = 0;
        for(; j < w; j+=8) 
        {
            if(fseek(fp, offset + i * bytesOfLine + j/8, SEEK_SET) != 0)
            {
                return false;
            }
            index = fgetc(fp);

            for(uint8_t k = 0; k < 8; k++)
            {
                /* width isn't multiple of 8 */
                if(j+k >= w)
                {
                    break;
                }

                if(fseek(fp, 14 + 40 + 4 * ((index >> (7-k)) & 0x01), SEEK_SET) != 0 ||
                    fread(&color, 4, 1, fp) != 1)
                {
                    return false;
                }
                mat[h-i-1][j+k].red = color.red;
                mat[h-i-1][j+k].green = color.green;
                mat[h-i-1][j+k].blue = color.blue;
                
            }
        }
    }

    return true;
}


/* 32bit color , BGRA8888 */
static bool writeBgra32(Image& mat, FILE* fp)
{

    uint32_t w = mat.width();
    uint32_t h = mat.height(); 

    BitMapFileHeader fileHeader;
    BitMapInfoHeader infoHeader;

    fileHeader.bfType[0] = 'B'; 
    fileHeader.bfType[1] = 'M'; 
    fileHeader.bfReserved1 = 0; 
    fileHeader.bfReserved2 = 0; 
    fileHeader.bfOffBits = 14 + 124;
    fileHeader.bfSize = 4*w*h + fileHeader.bfOffBits;
    infoHeader.biSize = 124;
    infoHeader.biWidth =  w;
    infoHeader.biHeight = h;
    infoHeader.biPlanes = 1; 
    infoHeader.biBitCount = 32;
    infoHeader.biCompression = 3; 
    infoHeader.biSizeImage = ((w*4+3)/4*4)*h;
    infoHeader.biXPelsPerMeter = 2835; 
    infoHeader.biYPelsPerMeter = 2835; 
    infoHeader.biClrUsed = 0; 
    infoHeader.biClrImportant = 0;

    if(!BMP_WriteFileHeader(fp, fileHeader) || !BMP_WriteInfoHeader(fp, infoHeader))
    {
        return false;
    }

    /* fathomless rule , let them happy */
    for(int i = 14 + 40; i < 14 + 124; i++)
    {
        fputc(0, fp);
    }

    /* write color data */
    for(uint32_t i = 0; i < h; i++)
    {
        /* color data of a line */
        for(uint32_t j = 0; j < w; j++) 
        {
            if(EOF == fputc(mat[h - i - 1][j].blue , fp)   ||
                EOF == fputc(mat[h - i - 1][j].green , fp) ||
                EOF == fputc(mat[h - i - 1][j].red , fp)   ||
                EOF == fputc(mat[h - i - 1][j].alpha , fp) )
            {
                return false;
            }
        }

        /* byets of line should be multiple of 4 , otherwise filled by 0 */
        size_t skip = (w * 4 + 3)/4 * 4 - w * 4;
        while(skip--)
        {
            if(EOF == fputc(0,fp))
            {
                return false;
            }
        }
    }

    return true;
}


/* 24bit color , BGR888 */
static bool writeBgr24(Image& mat, FILE* fp)
{

    uint32_t w = mat.width();
    uint32_t h = mat.height(); 

    BitMapFileHeader fileHeader;
    BitMapInfoHeader infoHeader;

    fileHeader.bfType[0] = 'B'; 
    fileHeader.bfType[1] = 'M'; 
    fileHeader.bfReserved1 = 0; 
    fileHeader.bfReserved2 = 0; 
    fileHeader.bfOffBits = 14 + 40;
    fileHeader.bfSize = 3*w*h + fileHeader.bfOffBits;
    infoHeader.biSize = 40;
    infoHeader.biWidth =  w;
    infoHeader.biHeight = h;
    infoHeader.biPlanes = 1; 
    infoHeader.biBitCount = 24;
    infoHeader.biCompression = 0; 
    infoHeader.biSizeImage = ((w*3+3)/4*4)*h;
    infoHeader.biXPelsPerMeter = 3780; 
    infoHeader.biYPelsPerMeter = 3780; 
    infoHeader.biClrUsed = 0; 
    infoHeader.biClrImportant = 0;
    


    if(!BMP_WriteFileHeader(fp, fileHeader) || !BMP_WriteInfoHeader(fp, infoHeader))
    {
        return false;
    }

    /* write color data */
    for(uint32_t i = 0; i < h; i++)
    {
        /* color data of a line */
        for(uint32_t j = 0; j < w; j++) 
        {
            if(EOF == fputc(mat[h - i - 1][j].blue , fp) ||
                EOF == fputc(mat[h - i - 1][j].green , fp) ||
                EOF == fputc(mat[h - i - 1][j].red , fp) )
            {
                return false;
            }
        }

        /* byets of line should be multiple of 4 , otherwise filled by 0 */
        size_t skip = (w * 3 + 3)/4 * 4 - w * 3;
        while(skip--)
        {
            if(EOF == fputc(0,fp))
            {
                return false;
            }
        }
    }

    return true;
}


/* 16bit color , BGR565 */
static bool writeRgb16(Image& mat, FILE* fp)
{
    uint32_t w = mat.width();
    uint32_t h = mat.height(); 

    BitMapFileHeader fileHeader;
    BitMapInfoHeader infoHeader;

    fileHeader.bfType[0] = 'B'; 
    fileHeader.bfType[1] = 'M'; 
    fileHeader.bfReserved1 = 0; 
    fileHeader.bfReserved2 = 0; 
    fileHeader.bfOffBits = 14 + 40 + 12;
    fileHeader.bfSize = 2*w*h + fileHeader.bfOffBits;
    infoHeader.biSize = 40;
    infoHeader.biWidth =  w;
    infoHeader.biHeight = h;
    infoHeader.biPlanes = 1; 
    infoHeader.biBitCount = 16;
    infoHeader.biCompression = 3; 
    infoHeader.biSizeImage = ((w*2+3)/4*4)*h;
    infoHeader.biXPelsPerMeter = 3780; 
    infoHeader.biYPelsPerMeter = 3780; 
    infoHeader.biClrUsed = 0; 
    infoHeader.biClrImportant = 0;
    


    if(!BMP_WriteFileHeader(fp, fileHeader) || !BMP_WriteInfoHeader(fp, infoHeader))
    {
        return false;
    }

    uint32_t redMass   = 0xf800;
    uint32_t greenMass = 0x07e0;
    uint32_t blueMass  = 0x001f; 
    if(fwrite(&redMass, 4, 1, fp) != 1 || fwrite(&greenMass, 4, 1, fp) != 1 || fwrite(&blueMass, 4, 1, fp) !=1 )
    {
        return false;
    }

    for(uint32_t i = 0; i < h; i++)
    {
        for(uint32_t j = 0; j < w; j++) 
        {
            uint16_t color = 
            (((uint16_t)((mat[h - i - 1][j].red)   & 0xf8) ) << 8) |
            (((uint16_t)((mat[h - i - 1][j].green) & 0xfc) ) << 3) |
            (((uint16_t)((mat[h - i - 1][j].blue)  & 0xf8) ) >> 3) ;
            if(fwrite(&color, 2, 1, fp) != 1)
            {
                return false;
            }
        }

        /* byets of line should be multiple of 4 , otherwise filled by 0 */
        size_t skip = (w * 2 + 3)/4 * 4 - w * 2;
        while(skip--)
        {
            if(EOF == fputc(0,fp))
            {
                return false;
            }
        }
    }

    return true;
}


/* 8bit color , only for gray scale image */
static bool writeGray8(Image& mat, FILE* fp)
{
    uint32_t w = mat.width();
    uint32_t h = mat.height(); 

    BitMapFileHeader fileHeader;
    BitMapInfoHeader infoHeader;

    fileHeader.bfType[0] = 'B'; 
    fileHeader.bfType[1] = 'M'; 
    fileHeader.bfReserved1 = 0; 
    fileHeader.bfReserved2 = 0; 
    fileHeader.bfOffBits = 14 + 40 + 256 * 4;
    fileHeader.bfSize = w*h + fileHeader.bfOffBits;
    infoHeader.biSize = 40;
    infoHeader.biWidth =  w;
    infoHeader.biHeight = h;
    infoHeader.biPlanes = 1; 
    infoHeader.biBitCount = 8;
    infoHeader.biCompression = 0; 
    infoHeader.biSizeImage = ((w+3)/4*4)*h;
    infoHeader.biXPelsPerMeter = 3780; 
    infoHeader.biYPelsPerMeter = 3780; 
    infoHeader.biClrUsed = 0; 
    infoHeader.biClrImportant = 0;

    if(!BMP_WriteFileHeader(fp, fileHeader) || !BMP_WriteInfoHeader(fp, infoHeader))
    {
        return false;
    }

    /* write 256 pallete */
    BGRPalette color;
    for(uint16_t gray = 0; gray < 256; gray++)
    {
        color.blue = color.green = color.red = (uint8_t)gray;
        if(fwrite(&color, 4, 1, fp) != 1)
        {
            return false;
        }
    }


    /* write color data */
    for(uint32_t i = 0; i < h; i++)
    {
        /* color data of a line */
        for(uint32_t j = 0; j < w; j++) 
        {
            /* Not gray scale image */
            if(mat[h - i - 1][j].blue != mat[h - i - 1][j].green || 
                mat[h - i - 1][j].blue != mat[h - i - 1][j].red)
            {
                return false;
            }

            if(EOF == fputc(mat[h - i - 1][j].blue, fp))
            {
                return false;
            }
        }

        /* byets of line should be multiple of 4 , otherwise filled by 0 */
        size_t skip = (w+3)/4 * 4 - w;
        while(skip--)
        {
            if(EOF == fputc(0,fp))
            {
                return false;
            }
        }
    }

    return true;
}


/* 1bit color , only for binary image */
static bool writeBinary1(Image& mat, FILE* fp)
{
    uint32_t w = mat.width();
    uint32_t h = mat.height(); 

    BitMapFileHeader fileHeader;
    BitMapInfoHeader infoHeader;

    fileHeader.bfType[0] = 'B'; 
    fileHeader.bfType[1] = 'M'; 
    fileHeader.bfReserved1 = 0; 
    fileHeader.bfReserved2 = 0; 
    fileHeader.bfOffBits = 14 + 40 + 2 * 4;
    fileHeader.bfSize = (w*h+7)/8 + fileHeader.bfOffBits;
    infoHeader.biSize = 40;
    infoHeader.biWidth =  w;
    infoHeader.biHeight = h;
    infoHeader.biPlanes = 1; 
    infoHeader.biBitCount = 1;
    infoHeader.biCompression = 0; 
    infoHeader.biSizeImage = ((w+3)/4*4)*h;
    infoHeader.biXPelsPerMeter = 3780; 
    infoHeader.biYPelsPerMeter = 3780; 
    infoHeader.biClrUsed = 0; 
    infoHeader.biClrImportant = 0;

    if(!BMP_WriteFileHeader(fp, fileHeader) || !BMP_WriteInfoHeader(fp, infoHeader))
    {
        return false;
    }

    /* write 2 pallete */
    BGRPalette color;
    color.blue = color.green = color.red = 0;
    if(fwrite(&color, 4, 1, fp) != 1)
    {
        return false;
    }
    color.blue = color.green = color.red = 0xff;
    if(fwrite(&color, 4, 1, fp) != 1)
    {
        return false;
    }
    


    /* write color data */
    for(uint32_t i = 0; i < h; i++)
    {
        /* color data of a line */
        for(uint32_t j = 0; j < w; j+=8) 
        {
            uint8_t color = 0;
            for(int bit = 0; bit < 8; bit++)
            {
                /* width isn't multiple of 8 */
                if(j+bit >= w)
                {
                    break;
                }

                if(mat[h-i-1][j+bit].red == 0    && 
                    mat[h-i-1][j+bit].green == 0 && 
                    mat[h-i-1][j+bit].blue == 0)
                {
                    continue;
                }
                else if(mat[h-i-1][j+bit].red == 0xff && 
                    mat[h-i-1][j+bit].green == 0xff   && 
                    mat[h-i-1][j+bit].blue == 0xff)
                {
                    color |= 1 << (7-bit);
                }
                else // not binary image
                {
                    return false;
                }
            }
            
            if(EOF == fputc(color, fp))
            {
                return false;
            }
        }

        /* byets of line should be multiple of 4 , otherwise filled by 0 */
        size_t skip = ((w+7)/8 + 3)/4 * 4 - (w+7)/8;
        while(skip--)
        {
            if(EOF == fputc(0,fp))
            {
                return false;
            }
        }
    }

    return true;
}

/*******************************************************************/

bool Bmp::read(Image& mat, std::string file)
{
    FILE* fp = fopen(file.c_str(),"rb");
    if(fp == NULL)
    {
        return false;
    }

    BitMapFileHeader fileHeader;
    BitMapInfoHeader infoHeader;

    if(!BMP_ReadFileHeader(fp, fileHeader) || !BMP_ReadInfoHeader(fp, infoHeader))
    {
        return false;
    }

    mat.resize(infoHeader.biWidth, infoHeader.biHeight);

    bool rval = false;
    switch(infoHeader.biBitCount)
    {
    case 32:
        rval = readBgra32(mat, fp, fileHeader.bfOffBits, infoHeader.biWidth, infoHeader.biHeight);
        break;

    case 24:
        rval = readBgr24(mat, fp, fileHeader.bfOffBits, infoHeader.biWidth, infoHeader.biHeight);
        break;

    case 16:
        rval = readRgb16(mat, fp, fileHeader.bfOffBits, infoHeader.biWidth, infoHeader.biHeight);
        break;
    
    case 8:
        rval = readPalette8(mat, fp, fileHeader.bfOffBits, infoHeader.biWidth, infoHeader.biHeight);
        break;

    case 4:
        rval = readPalette4(mat, fp, fileHeader.bfOffBits, infoHeader.biWidth, infoHeader.biHeight);
        break;

    case 1:
        rval = readPalette1(mat, fp, fileHeader.bfOffBits, infoHeader.biWidth, infoHeader.biHeight);
        break;
    }
    fclose(fp);
    return rval;
}

bool Bmp::write(Image& mat, std::string file, uint8_t bits)
{
    FILE* fp = fopen(file.c_str(),"wb");
    if(fp == NULL)
    {
        return false;
    }

    bool rval = false;
    switch(bits)
    {
    case 32 :
        rval = writeBgra32(mat, fp);
        break;
    case 24 :
        rval = writeBgr24(mat, fp);
        break;
    case 16 : 
        rval = writeRgb16(mat, fp);
        break;
    case 8:
        rval = writeGray8(mat, fp);
        break;
    case 1:
        rval = writeBinary1(mat, fp);
        break;
    }

    fclose(fp);
    return rval;
}


}; // namespace lolita
