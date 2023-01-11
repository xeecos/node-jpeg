#include <malloc.h>
#include <string.h>

#include "libs/demosaic.h"
#include "libs/jpge.h"
#include "libs/nanojpeg.h"

typedef enum
{
    PIXFORMAT_RGB888,    // 3BPP/RGB565
    PIXFORMAT_RGB565,    // 2BPP/RGB565
    PIXFORMAT_GRAYSCALE, // 1BPP/GRAYSCALE
    PIXFORMAT_RAW,       // RAW
} pixformat_t;
/* Bytes per pixel of image output */

/* Session identifier for input/output functions (name, members and usage are as user defined) */

/**
 * jpge
*/

void convert_line_format(unsigned char *src, pixformat_t format, unsigned char *dst, size_t width, size_t in_channels, size_t line)
{
    if(format==PIXFORMAT_GRAYSCALE)
    {
        memcpy(dst, src + (line * width), width);
    }
    else if(format==PIXFORMAT_RGB888)
    {
        memcpy(dst, src + (line * width * in_channels), width * in_channels);
    }
}
bool convert_image(unsigned char *src, unsigned short width, unsigned short height, pixformat_t format, unsigned char quality, jpge::output_stream *dst_stream)
{
    int num_channels = 3;
    jpge::subsampling_t subsampling = jpge::H2V2;

    if(format == PIXFORMAT_GRAYSCALE)
    {
        num_channels = 1;
        subsampling = jpge::Y_ONLY;
    }

    if (!quality)
    {
        quality = 1;
    }
    else if (quality > 100)
    {
        quality = 100;
    }

    jpge::params comp_params = jpge::params();
    comp_params.m_subsampling = subsampling;
    comp_params.m_quality = quality;

    jpge::jpeg_encoder dst_image;

    if (!dst_image.init(dst_stream, width, height, num_channels, comp_params))
    {
        return false;
    }

    unsigned char *line = (unsigned char *)malloc(width * num_channels);
    if (!line)
    {
        return false;
    }
    for (int i = 0; i < height; i++)
    {
        convert_line_format(src, format, line, width, num_channels, i);
        if (!dst_image.process_scanline(line))
        {
            free(line);
            return false;
        }
    }
    free(line);

    if (!dst_image.process_scanline(NULL))
    {
        return false;
    }
    dst_image.deinit();
    return true;
}

class memory_stream : public jpge::output_stream
{
protected:
    unsigned char *out_buf;
    size_t max_len, index;

public:
    memory_stream(void *pBuf, unsigned int buf_size) : out_buf(static_cast<unsigned char *>(pBuf)), max_len(buf_size), index(0) {}

    virtual ~memory_stream() {}

    virtual bool put_buf(const void *pBuf, int len)
    {
        if (!pBuf)
        {
            // end of image
            return true;
        }
        if ((size_t)len > (max_len - index))
        {
            len = max_len - index;
        }
        if (len)
        {
            memcpy(out_buf + index, pBuf, len);
            index += len;
        }
        return true;
    }

    virtual unsigned int get_size() const
    {
        return index;
    }
};
extern "C" 
{
    void demosaic(int in_ptr, int out_ptr, int bayer, int width, int height)
    {
        bilinear(width, height, 8, BIG_ENDING, (Bayer)bayer, (unsigned char *)in_ptr, (unsigned char *)out_ptr);
    }
    int encode_jpg(int in_ptr,  int width, int height, int out_ptr, int out_count, int isRGB, int quality)
    {
        unsigned char* rgbBuffer = (unsigned char*) in_ptr;
        unsigned char* outBuffer = (unsigned char*) out_ptr;
        unsigned int maxBufferCount = out_count;
        pixformat_t format = isRGB>0?PIXFORMAT_RGB888:PIXFORMAT_GRAYSCALE;
        memory_stream dst_stream(outBuffer, maxBufferCount);
        convert_image(rgbBuffer, width, height, format, quality, &dst_stream);
       
        return dst_stream.get_size();
    }

    int decode_jpg(int in_ptr, int in_count, int out_ptr)
    {
        njInit();
        njDecode((unsigned char*) in_ptr,in_count);
        int* size = (int*)malloc(sizeof(int)*2);
        size[0] = njGetWidth();
        size[1] = njGetHeight();
        memcpy((unsigned char*) out_ptr, njGetImage(),njGetImageSize());
        njDone();
        return (int)size;
    }

    int js_malloc(int size)
    {
        unsigned char *ptr = (unsigned char*)malloc(size);
        return (int)ptr;
    }

    void js_free(int ptr)
    {
        return free((void*)ptr);
    }
    void js_write(int ptr,int offset, unsigned char val)
    {
        unsigned char*buf = (unsigned char*)ptr;
        buf[offset] = val;
    }
    unsigned char js_read(int ptr,int offset)
    {
        unsigned char*buf = (unsigned char*)ptr;
        return buf[offset];
    }
    int js_read_int(int ptr,int offset)
    {
        int*buf = (int*)ptr;
        return buf[offset];
    }
}