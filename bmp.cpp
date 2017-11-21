#include <stdio.h>
#include "bmp.h"

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



namespace lolita
{

/* 24bit color BGR888 */
static bool readBgr24(Mat& mat, FILE* fp, uint32_t offset, uint32_t w, uint32_t h)
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
static bool readRgb16(Mat& mat, FILE* fp, uint32_t offset, uint32_t w, uint32_t h)
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
static bool readPalette8(Mat& mat, FILE* fp, uint32_t offset, uint32_t w, uint32_t h)
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
static bool readPalette4(Mat& mat, FILE* fp, uint32_t offset, uint32_t w, uint32_t h)
{
    mat.resize(w,h);
    uint8_t index = 0;
    BGRPalette color;
    for(uint32_t i = 0; i < h; i++)
    {
        uint32_t j = 0;
        for(; j < w; j+=2) 
        {
            if(fseek(fp, offset + i * w + j, SEEK_SET) != 0)
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

            if(fseek(fp, 14 + 40 + 4 * (index & 0x0f), SEEK_SET) != 0 ||
                fread(&color, 4, 1, fp) != 1)
            {
                return false;
            }
            mat[h-i-1][j+1].red = color.red;
            mat[h-i-1][j+1].green = color.green;
            mat[h-i-1][j+1].blue = color.blue;
        }

        /* byets of line should be multiple of 4 , otherwise filled by 0 */
        /* skip these data */
        size_t skip = (j / 2 + 3)/4 * 4 - j/2;
        if(fseek(fp, skip, SEEK_CUR) != 0)
        {
            return false;
        }
    }

    return true;
}

/* 1bit color , 2 palettes */
static bool readPalette1(Mat& mat, FILE* fp, uint32_t offset, uint32_t w, uint32_t h)
{
    mat.resize(w,h);
    uint8_t index = 0;
    BGRPalette color;
    for(uint32_t i = 0; i < h; i++)
    {
        uint32_t j = 0;
        for(; j < w; j+=8) 
        {
            if(fseek(fp, offset + i * w + j, SEEK_SET) != 0)
            {
                return false;
            }
            index = fgetc(fp);
            for(uint8_t k = 0; k < 8; k++)
            {
                if(fseek(fp, 14 + 40 + 4 * ((index >> k)  & 0x01), SEEK_SET) != 0 ||
                    fread(&color, 4, 1, fp) != 1)
                {
                    return false;
                }
                mat[h-i-1][j+7-k].red = color.red;
                mat[h-i-1][j+7-k].green = color.green;
                mat[h-i-1][j+7-k].blue = color.blue;
            }
        }

        /* byets of line should be multiple of 4 , otherwise filled by 0 */
        /* skip these data */
        size_t skip = (j / 8 + 3)/4 * 4 - j/8;
        if(fseek(fp, skip, SEEK_CUR) != 0)
        {
            return false;
        }
    }

    return true;
}



bool Bmp::read(Mat& mat, std::string file)
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


bool Bmp::write(Mat& mat, std::string file)
{
    FILE* fp = fopen(file.c_str(),"wb");
    if(fp == NULL)
    {
        return false;
    }

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

    for(uint32_t i = 0; i < h; i++)
    {
        for(uint32_t j = 0; j < w; j++) 
        {
            fputc(mat[h - i - 1][j].blue , fp);
            fputc(mat[h - i - 1][j].green , fp);
            fputc(mat[h - i - 1][j].red , fp);
        }

        /* byets of line should be multiple of 4 , otherwise filled by 0 */
        size_t skip = (w * 3 + 3)/4 * 4 - w * 3;
        while(skip--)
        {
            fputc(0,fp);
        }
    }


    fclose(fp);
    return true;
}

}; // namespace lolita
