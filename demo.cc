#include <emscripten/bind.h>
#include <emscripten/val.h>
using namespace emscripten;
val process_data(int in_ptr, int in_count, int out_ptr)
{
    // 指针转换
    unsigned char* in_buf = (unsigned char *) in_ptr;
    unsigned char * out_buf = (unsigned char *) out_ptr;
    
    // 处理数据
    for(int i=0;i<in_count;i++)
    {
        out_buf[i] = in_buf[i] + 30;
    }
    /*
        emscripten::val 返回js对象
    */
    val ret = val::object();
    ret.set("result", 0);
    return ret;
}
EMSCRIPTEN_BINDINGS(image)
{
  function("process_data", &process_data);
}