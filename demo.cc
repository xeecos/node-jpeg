#include <nan.h>
using namespace Nan;  
using namespace v8;
NAN_METHOD(process_data)
{
    v8::Isolate *isolate = Isolate::GetCurrent();
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    uint8_t* in_buf = (uint8_t*) node::Buffer::Data(info[0]->ToObject(context).ToLocalChecked());
    unsigned int in_count= info[1]->Uint32Value(context).ToChecked();
    uint8_t* out_buf= (uint8_t*) node::Buffer::Data(info[2]->ToObject(context).ToLocalChecked());
    // 处理数据
    for(int i=0;i<in_count;i++)
    {
        out_buf[i] = in_buf[i] + 30;
    }
    // 返回结果
    info.GetReturnValue().Set(0);
}

NAN_MODULE_INIT(Init) {  
   Nan::Set(target, New<String>("process_data").ToLocalChecked(),  GetFunction(New<FunctionTemplate>(process_data)).ToLocalChecked());
}

NODE_MODULE(addon, Init)