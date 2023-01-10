#include <stdio.h>
#include <stdint.h>
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
int encode_jpeg(int in_ptr, int in_count, int out_ptr, int out_count)
{
    printf("%d %d\n", in_ptr, out_ptr);
    uint8_t *in_buf = (uint8_t *)in_ptr;
    uint8_t *out_buf = (uint8_t *)out_ptr;
    printf("%d %d\n", in_buf[0], in_buf[1]);
    out_buf[0] = 0x18;
    out_buf[1] = 0x24;
    return out_count;
}
#include <emscripten/bind.h>
using namespace emscripten;
// Binding code
EMSCRIPTEN_BINDINGS(image)
{
  function("encode_jpeg", &encode_jpeg);
}