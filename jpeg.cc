#include <nan.h>
#include <stdio.h>
#include <string.h>
#include "libs/nanojpeg.h"
#include "libs/jpge.h"
#include "libs/demosaic.h"
using namespace Nan;  
using namespace v8;
typedef enum
{
    PIXFORMAT_RGB888,    // 3BPP/RGB565
    PIXFORMAT_RGB565,    // 2BPP/RGB565
    PIXFORMAT_GRAYSCALE, // 1BPP/GRAYSCALE
    PIXFORMAT_RAW,       // RAW
} pixformat_t;
/* Bytes per pixel of image output */

uint8_t* outBuffer;
uint8_t* jpegBuffer;
unsigned int jpegBufferSize;
int idx;
int jpegIndex;
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

    virtual bool put_buf(unsigned char *pBuf, int len)
    {
        if (!pBuf)
        {
            // end of image
            return true;
        }
        if ((size_t)len > (max_len - index))
        {
            printf( "JPG output overflow: %d bytes", len - (max_len - index));
            len = max_len - index;
        }
        if (len)
        {
            for(int i=0;i<len;i++)
            {
                out_buf[index+i] = pBuf[i];
            }
            index += len;
        }
        return true;
    }

    virtual unsigned int get_size() const
    {
        return index;
    }
};

NAN_METHOD(encode) {  
    
    v8::Isolate *isolate = Isolate::GetCurrent();
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    uint8_t* rgbBuffer = (uint8_t*) node::Buffer::Data(info[0]->ToObject(context).ToLocalChecked());
    unsigned int rgbBufferSize = info[1]->Uint32Value(context).ToChecked();

    unsigned int width = info[4]->Uint32Value(context).ToChecked();
    unsigned int height = info[5]->Uint32Value(context).ToChecked();

    outBuffer = (uint8_t*) node::Buffer::Data(info[2]->ToObject(context).ToLocalChecked());
    unsigned int outBufferSize = info[3]->Uint32Value(context).ToChecked();
    bool isRGB = info[6]->Uint32Value(context).ToChecked();
    unsigned int quality = info[7]->Uint32Value(context).ToChecked();
    idx = 0;
    pixformat_t format = isRGB?PIXFORMAT_RGB888:PIXFORMAT_GRAYSCALE;
    memory_stream dst_stream(outBuffer, outBufferSize);
    convert_image(rgbBuffer, width, height, format, quality, &dst_stream);
    info.GetReturnValue().Set(dst_stream.get_size());
}

NAN_METHOD(decode)
{
    v8::Isolate *isolate = Isolate::GetCurrent();
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    jpegBuffer = (uint8_t*) node::Buffer::Data(info[0]->ToObject(context).ToLocalChecked());
    unsigned int jpegBufferSize = info[1]->Uint32Value(context).ToChecked();
    outBuffer = (uint8_t*) node::Buffer::Data(info[2]->ToObject(context).ToLocalChecked());
    bool isRGB = info[3]->Uint32Value(context).ToChecked();

    njInit();
    njDecode(jpegBuffer,jpegBufferSize);
    memcpy(outBuffer, njGetImage(),njGetImageSize());
    printf("rgb:%d, len:%d\n",njIsColor(),njGetImageSize());
    njDone();
}

NAN_METHOD(demosaic)
{
    v8::Isolate *isolate = Isolate::GetCurrent();
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    uint8_t* rgbBuffer = (uint8_t*) node::Buffer::Data(info[0]->ToObject(context).ToLocalChecked());
    unsigned int rgbBufferSize = info[1]->Uint32Value(context).ToChecked();
    uint8_t* outBuffer = (uint8_t*) node::Buffer::Data(info[2]->ToObject(context).ToLocalChecked());
    Bayer bayer = (Bayer)info[3]->Uint32Value(context).ToChecked();
    unsigned int width = info[4]->Uint32Value(context).ToChecked();
    unsigned int height = info[5]->Uint32Value(context).ToChecked();
    bilinear(width, height, 8, BIG_ENDING, bayer, rgbBuffer, outBuffer);
}

NAN_MODULE_INIT(Init) {  
   Nan::Set(target, New<String>("encode").ToLocalChecked(),  GetFunction(New<FunctionTemplate>(encode)).ToLocalChecked());
   Nan::Set(target, New<String>("decode").ToLocalChecked(),  GetFunction(New<FunctionTemplate>(decode)).ToLocalChecked());
   Nan::Set(target, New<String>("demosaic").ToLocalChecked(),  GetFunction(New<FunctionTemplate>(demosaic)).ToLocalChecked());
}

NODE_MODULE(addon, Init)

/**
 * 


*/