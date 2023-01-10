#pragma once
#include "math.h"
typedef enum Bayer {
    RGGB = 0, 
    GRBG, 
    GBRG, 
    BGGR
} Bayer;
typedef enum Endings {
    LITTLE_ENDING = 0, 
    BIG_ENDING, 
} Endings;

struct bayer_t
{
    unsigned char depth;
    Bayer bayer;
    unsigned char endianness;
    unsigned short width;
    unsigned short height;
    unsigned char* data;
    unsigned char* result;
    unsigned long size;
};

// Dereference all these functions up front.  Speeds up benchmarks significantly.

unsigned int reflect(unsigned int x, unsigned int p1, unsigned int p2) 
{
    unsigned int r;
    if (x >= p1 && x <= p2) {
        r = x;
    } else if (x < p1) {
        r = p1 + (p1 > x?p1-x:(x-p1));
    } else {
        r = p2 - (p2 > x?p2-x:(x-p2));
    }
    return r;
};

int read(int i, bayer_t o)
{
    switch (o.depth) {
        case 16:
            if (o.endianness == LITTLE_ENDING) {
                return o.data[i * 2]+o.data[i * 2+1]<<8;
            } else {
                return o.data[i * 2+1]+o.data[i * 2]<<8;
            }
        default:
            return o.data[i];
    }
};

void write(int x, int i, bayer_t o)
{
    switch (o.depth) {
        case 16:
            if (o.endianness == LITTLE_ENDING) {
                o.result[i * 2] = x>>8;
                o.result[i * 2+1] = x&0xff;
            } else {
                o.result[i * 2+1] = x>>8;
                o.result[i * 2] = x&0xff;
            }
            break;
        default:
            o.result[i] = x;
            break;
    }
};

int pixel(int i, int j, bayer_t o) 
{
    unsigned int x = reflect(i, 0, o.height - 1);
    unsigned int y = reflect(j, 0, o.width - 1);
    return read(x * o.width + y, o);
};

bool isRed(int i, int j, Bayer bayer)
{
    switch (bayer) {
        case RGGB:
            return (i % 2 == 0 && j % 2 == 0);
        case GRBG:
            return (i % 2 == 0 && j % 2 == 1);
        case GBRG:
            return (i % 2 == 1 && j % 2 == 0);
        case BGGR:
            return (i % 2 == 1 && j % 2 == 1);
    }
};

bool isGreenR(int i, int j, Bayer bayer)
{
    switch (bayer) {
        case RGGB:
            return (i % 2 == 0 && j % 2 == 1);
        case GRBG:
            return (i % 2 == 0 && j % 2 == 0);
        case GBRG:
            return (i % 2 == 1 && j % 2 == 1);
        case BGGR:
            return (i % 2 == 1 && j % 2 == 0);
    }
};

bool isGreenB(int i, int j, Bayer bayer)
 {
    switch (bayer) {
        case RGGB:
            return (i % 2 == 1 && j % 2 == 0);
        case GRBG:
            return (i % 2 == 1 && j % 2 == 1);
        case GBRG:
            return (i % 2 == 0 && j % 2 == 0);
        case BGGR:
            return (i % 2 == 0 && j % 2 == 1);
    }
};

bool isBlue(int i, int j, Bayer bayer)
{
    switch (bayer) {
        case RGGB:
            return (i % 2 == 1 && j % 2 == 1);
        case GRBG:
            return (i % 2 == 1 && j % 2 == 0);
        case GBRG:
            return (i % 2 == 0 && j % 2 == 1);
        case BGGR:
            return (i % 2 == 0 && j % 2 == 0);
    }
};

int red(int i, int j, Bayer b, bayer_t o)
{
    if (isGreenR(i, j, b)) return round((pixel(i, j - 1, o) + pixel(i, j + 1, o)) / 2);
    if (isGreenB(i, j, b)) return round((pixel(i - 1, j, o) + pixel(i + 1, j, o)) / 2);
    if (isBlue(i, j, b))
        return round((pixel(i - 1, j - 1, o) + pixel(i - 1, j + 1, o) + pixel(i + 1, j - 1, o) + pixel(i + 1, j + 1, o)) / 4);
    return pixel(i, j, o);
};

int green(int i, int j, Bayer b, bayer_t o)
{
    if (isRed(i, j, b) || isBlue(i, j, b))
    {
        return ((pixel(i, j - 1, o) + pixel(i, j + 1, o) + pixel(i - 1, j, o) + pixel(i + 1, j, o)) / 4)>>0;
    }
    return pixel(i, j, o);
};

int blue(int i, int j, Bayer b, bayer_t o){
    if (isRed(i, j, b))
        return ((pixel(i - 1, j - 1, o) + pixel(i - 1, j + 1, o) + pixel(i + 1, j - 1, o) + pixel(i + 1, j + 1, o)) >>2);
    if (isGreenR(i, j, b)) return ((pixel(i - 1, j, o) + pixel(i + 1, j, o)) >>1);
    if (isGreenB(i, j, b)) return ((pixel(i, j - 1, o) + pixel(i, j + 1, o)) >>1);
    return pixel(i, j, o);
};

void bilinear(int width, int height, int depth,int endianness,Bayer bayer, unsigned char* data, unsigned char* result) 
{
    bayer_t options;
    

    int h = height;
    int w = width;

    // h * w * 3 * (options.depth / 8);
    options.data = data;
    options.result = result;
    options.depth = depth;
    options.endianness = endianness;
    options.bayer = bayer;
    options.height = height;
    options.width = width;
    for (int i = 0; i < h; i++) {
        int l = i * w * 3;
        for (int j = 0; j < w; j++) {
            int k = l + j * 3;
            write(red(i, j, options.bayer, options), k, options);
            write(green(i, j, options.bayer, options), k + 1, options);
            write(blue(i, j, options.bayer, options), k + 2, options);
        }
    }
}
