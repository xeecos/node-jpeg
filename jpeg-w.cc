#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "libs/demosaic.h"
#include "libs/jpge.h"
#include "libs/nanojpeg.h"
using namespace emscripten;
#ifndef EM_PORT_API
#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#if defined(__cplusplus)
#define EM_PORT_API(rettype) extern "C" rettype EMSCRIPTEN_KEEPALIVE
#else
#define EM_PORT_API(rettype) rettype EMSCRIPTEN_KEEPALIVE
#endif
#else
#if defined(__cplusplus)
#define EM_PORT_API(rettype) extern "C" rettype
#else
#define EM_PORT_API(rettype) rettype
#endif
#endif
#endif

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

void convert_line_format(uint8_t *src, pixformat_t format, uint8_t *dst, size_t width, size_t in_channels, size_t line)
{
    uint8_t r = 0, g = 0, b = 0;
    if(format==PIXFORMAT_GRAYSCALE)
    {
        memcpy(dst, src + line * width, width);
    }
    else if(format==PIXFORMAT_RGB888)
    {
        memcpy(dst, src + line * width * in_channels, width * in_channels);
    }
}
bool convert_image(uint8_t *src, uint16_t width, uint16_t height, pixformat_t format, uint8_t quality, jpge::output_stream *dst_stream)
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

    uint8_t *line = (uint8_t *)malloc(width * num_channels);
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
        printf( "JPG image finish failed");
        return false;
    }
    dst_image.deinit();
    return true;
}

class memory_stream : public jpge::output_stream
{
protected:
    uint8_t *out_buf;
    size_t max_len, index;

public:
    memory_stream(void *pBuf, unsigned int buf_size) : out_buf(static_cast<uint8_t *>(pBuf)), max_len(buf_size), index(0) {}

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
            printf( "JPG output overflow: %zu bytes", len - (max_len - index));
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


int encode(int in_ptr,  int width, int height, int out_ptr, int out_count, bool isRGB, int quality)
{
    uint8_t* rgbBuffer = (uint8_t*) in_ptr;
    uint8_t* outBuffer = (uint8_t*) out_ptr;
    unsigned int outBufferSize = out_count;
    pixformat_t format = isRGB?PIXFORMAT_RGB888:PIXFORMAT_GRAYSCALE;
    memory_stream dst_stream(outBuffer, outBufferSize);
    convert_image(rgbBuffer, width, height, format, quality, &dst_stream);
    return dst_stream.get_size();
}
val decode(int in_ptr, int in_count, int out_ptr, bool isRGB)
{
    uint8_t* jpegBuffer = (uint8_t*) in_ptr;
    unsigned int jpegBufferSize = in_count;
    uint8_t* outBuffer = (uint8_t*) out_ptr;

    val ret = val::object();
    njInit();
    njDecode(jpegBuffer,jpegBufferSize);
    ret.set("width", njGetWidth());
    ret.set("height", njGetHeight());
    memcpy(outBuffer, njGetImage(),njGetImageSize());
    njDone();
	  return ret;
}
void demosaic(int in_ptr, int out_ptr, int bayer, int width, int height)
{
    bilinear(width, height, 8, BIG_ENDING, (Bayer)bayer, (uint8_t *)in_ptr, (uint8_t *)out_ptr);
}
// Binding code
EMSCRIPTEN_BINDINGS(image)
{
  function("encode", &encode);
  function("decode", &decode);
  function("demosaic", &demosaic);
}